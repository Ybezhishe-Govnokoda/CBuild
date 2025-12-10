#include "build.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

int exec_command(const char *cmd) {
   if (!cmd) return 0;

   // Print command like make does
   printf("%s\n", cmd);
   fflush(stdout);

   int rc = system(cmd);
   if (rc == -1) {
      fprintf(stderr, "exec_command: failed to run shell for command: %s\n", cmd);
      return BUILD_ERR_CMD;
   }

#ifdef _WIN32
   return rc != 0;
#else
   // POSIX: interpret status
   if (WIFEXITED(rc)) {
      int exit_code = WEXITSTATUS(rc);
      return exit_code != 0;
   }
   else if (WIFSIGNALED(rc)) {
      int sig = WTERMSIG(rc);
      fprintf(stderr, "exec_command: command '%s' terminated by signal %d\n", cmd, sig);
      return BUILD_ERR_CMD;
   }
   else {
      return BUILD_ERR_CMD;
   }
#endif
}

// Clear cached flags for all vertices in graph.
void build_reset_flags(Graph *g) {
   if (!g) return;
   for (int i = 0; i < g->count; ++i) {
      Vertex *v = g->vertices[i];
      v->checked = 0;
      v->outdated = 0;
      v->built = 0;
   }
}


// Caches is_outdated results in vertex struct to avoid redundant checks.
static int is_outdated_cached(Vertex *v) {
   if (!v) return true; // treat missing vertex as outdated

   if (v->checked) return v->outdated;

   v->checked = true; // mark as checked to avoid re-checking
   v->outdated = false;

	// If no rule: outdated if file missing
   if (!v->rule) {
      if (!file_exists(v->name)) {
         v->outdated = true;
         return true;
      }
      else {
         v->outdated = false;
         return false;
      }
   }

   // if target file missing -> outdated
   if (!file_exists(v->name)) {
      v->outdated = true;
      return true;
   }

   time_t target_mtime = get_mtime(v->name);

   // check dependencies
   for (int i = 0; i < v->dep_count; ++i) {
      Vertex *d = v->deps[i];

      // If dependency has a rule — recursively evaluate it
      if (d->rule) {
         if (is_outdated_cached(d)) {
            v->outdated = true;
            return true;
         }
      }

      // If dependency file exists and is newer than target -> outdated
      if (file_exists(d->name)) {
         time_t dep_mtime = get_mtime(d->name);
         if (dep_mtime > target_mtime) {
            v->outdated = true;
            return true;
         }
      }
      else {
			// Dependency file missing: if it has no rule -> target outdated
         if (!d->rule) {
            v->outdated = true;
            return true;
         }
      }
   }

   // all checks passed - target fresh
   v->outdated = false;
   return false;
}

// Recursive builder in DFS order
// Returns BUILD_OK on success; otherwise appropriate error code.
int build_vertex(Graph *g, Vertex *v) {
   if (!v) return BUILD_ERR_MISSING;

   if (v->built) return BUILD_OK;

   // Recursively build dependencies that have rules
   for (int i = 0; i < v->dep_count; ++i) {
      Vertex *d = v->deps[i];
      if (d->rule) {
         int rc = build_vertex(g, d);
         if (rc != BUILD_OK) return rc;
      }
   }

   // Now decide if we need to build this vertex
   int outdated = is_outdated_cached(v);

   if (outdated) {
      if (!v->rule) {
         fprintf(stderr, "build_vertex: missing rule to build target '%s'\n", v->name);
         return BUILD_ERR_MISSING;
      }

      // Execute each command in rule->commands
      for (int c = 0; c < v->rule->cmd_count; ++c) {
         const char *cmd = v->rule->commands[c];
         if (!cmd) continue;
         int rc = exec_command(cmd);
         if (rc != 0) {
            fprintf(stderr, "build_vertex: command failed for target '%s': %s\n", v->name, cmd);
            return BUILD_ERR_CMD;
         }
      }
   } // else not outdated -> nothing to do

   v->built = 1;
   return BUILD_OK;
}


// Find vertex by name (via hash table) and build it.
int build_by_name(Graph *g, const char *target_name) {
   if (!g || !target_name) return BUILD_ERR_OTHER;

   Vertex *v = hash_get(&g->map, target_name);
   if (!v) {
      if (!file_exists(target_name)) {
         fprintf(stderr, "build_by_name: target '%s' not found and file missing\n", target_name);
         return BUILD_ERR_MISSING;
      }
      return BUILD_OK;
   }

   // reset flags before build (so previous is_outdated results are cleared)
   build_reset_flags(g);

   return build_vertex(g, v);
}
