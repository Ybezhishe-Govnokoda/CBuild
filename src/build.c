#include "../include/build.h"

bool is_outdated(Vertex *v) {
	// If already checked, return cached result
   if (v->checked)
      return v->outdated;

	v->checked = true; // Mark as checked

	// If target file doesn't exist - it's outdated
   if (!file_exists(v->name)) {
      v->outdated = true;
      return true;
   }

   time_t target_mtime = get_mtime(v->name);

	// Check dependencies
   for (int i = 0; i < v->dep_count; i++) {
      Vertex *dep = v->deps[i];

		// If dependency is outdated - the target is outdated
      if (is_outdated(dep)) {
         v->outdated = true;
         return true;
      }

		// If dependency file exists and is newer than target
      if (file_exists(dep->name)) {
         time_t dep_mtime = get_mtime(dep->name);
         if (dep_mtime > target_mtime) {
            v->outdated = true;
            return true;
         }
      }
   }

   v->outdated = false;
   return false;
}
