#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <stdio.h>
#include "../include/utils.h"

typedef enum {
	PARSE_OK = 0,
	PARSE_NOMEM_ERROR = -1,
	PARSE_SYNTAX_ERROR = -2,
	PARSE_FILE_ERROR = -3,
	PARSE_COMMAND_ERROR = -4,
	PARSE_TARGET_ERROR = -5
} parse_status;

#ifdef DEBUG
inline short parse_command_line(const char *line, Rule *rule);
inline short parse_target_line(const char *line, Rule *rule);
#endif

short parse_file(const char *filename, Rule *rules);

#endif // FILE_PARSER_H