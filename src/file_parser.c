#include "../include/file_parser.h"
#include <malloc.h> // For capability with VCIC001

#define SKIP_SPACES(p) while (*p == ' ' || *p == '\t') p++;

inline short parse_command_line(const char *line, Rule *rule) {
   rule->commands = (char **)realloc(
      rule->commands, 
      sizeof(char *) * (size_t)(rule->cmd_count + 1)
   );

   if (rule->commands == NULL) {
      fprintf(stderr, "Error realloc for commands\n");
      return PARSE_NOMEM_ERROR;
   }

   rule->commands[rule->cmd_count] = _strdup(line);
   rule->cmd_count++;

	return PARSE_OK;
}

inline short parse_target_line(const char *line, Rule *rule) {
   char *colon = strchr(line, ':');

   if (!colon) {
      fprintf(stderr, "Parse error: no colon in rule: %s\n", line);
      return PARSE_SYNTAX_ERROR;
   }

   // ---- Parse target ----
   size_t target_len = (size_t)(colon - line);
   rule->target = malloc(target_len + 1);
   if (rule->target == NULL) {
      fprintf(stderr, "Error with memory allocation for target\n");
      return PARSE_NOMEM_ERROR;
   }
   memcpy(rule->target, line, target_len);
   rule->target[target_len] = '\0';

   // ---- Parse deps ----
   const char *p = colon + 1;

   SKIP_SPACES(p)

   rule->deps = NULL;
   rule->deps_count = 0;

   while (*p) {
      // Find end of dep
      const char *start = p;

      while (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\0')
         p++;

      size_t dep_len = (size_t)(p - start);
      if (dep_len > 0) {
         char **new_deps = realloc(
            rule->deps, 
            sizeof(char *) * (size_t)(rule->deps_count + 1)
         );
         
         if (new_deps == NULL) {
            fprintf(stderr, "Error realloc for deps\n");
            return PARSE_NOMEM_ERROR;
         }
         rule->deps = new_deps;
         rule->deps[rule->deps_count] = malloc(dep_len + 1);
         if (rule->deps[rule->deps_count] == NULL) {
            fprintf(stderr, "Error malloc for deps\n");
            return PARSE_NOMEM_ERROR;
         }
         memcpy(rule->deps[rule->deps_count], start, dep_len);
         rule->deps[rule->deps_count][dep_len] = '\0';
         rule->deps_count++;
      }
		// End of line
      if (*p == '\n') break;

      SKIP_SPACES(p)
   }

   return PARSE_OK;
}

short parse_file(const char *filename, Rule *rules) {
   FILE *file = fopen(filename, "r");

   if (file == NULL) {
      perror("Error opening file");
      return PARSE_FILE_ERROR;
   }

   DString line;
   ds_init(&line);
   rules->cmd_count = 0;

   while (ds_readline(&line, file)) {
      if (line.data[0] == '\0' || line.data[0] == '\n') continue;

      if (line.data[0] != ' ' && line.data[0] != '\t') {
         // For target lines
         if (parse_target_line(line.data, rules) != PARSE_OK) {
            ds_free(&line);
            fclose(file);
            return PARSE_TARGET_ERROR;
         }
      }
      else {
			// For command lines
         if (parse_command_line(line.data + 1, rules) != PARSE_OK) {
            ds_free(&line);
            fclose(file);
            return PARSE_COMMAND_ERROR;
         }
      }
   }

   ds_free(&line);
   fclose(file);
   return PARSE_OK;
}