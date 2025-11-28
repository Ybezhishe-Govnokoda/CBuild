#include "dynamic_structs.h"
#include "file_parser.h"

int main()
{
	Rule rule;
	rule_init(&rule);

	if (parse_file("Buildfile", &rule) != PARSE_OK) {
		fprintf(stderr, "Error parsing file\n");
		return 1;
	}
	else {
		printf_s(".target: %s\n", rule.target);
		for (int i = 0; i < rule.deps_count; i++) {
			printf_s(" .dep: %s\n", rule.deps[i]);
		}
		for (int i = 0; i < rule.cmd_count; i++) {
			printf_s(" .cmd: %s\n", rule.commands[i]);
		}
	}
}


