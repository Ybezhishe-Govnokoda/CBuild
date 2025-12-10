#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdbool.h>
#include <time.h>

#ifdef _WIN32 // Windows
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#define STAT _stat

#else // POSIX
#include <sys/stat.h>
#define STAT stat

#endif

bool file_exists(const char *path) {
   struct STAT st;
   return STAT(path, &st) == 0;
}

// Get modification time of file, or 0 if file doesn't exist
time_t get_mtime(const char *path) {
   struct STAT st;
   if (STAT(path, &st) != 0)
      return 0;
   return st.st_mtime;
}

#endif // FILESYSTEM_H