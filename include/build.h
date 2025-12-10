#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "graph.h"
#include "filesystem.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum {
	 BUILD_OK,
	 BUILD_ERR_CMD,
	 BUILD_ERR_MISSING,     // missing target file and no rule
	 BUILD_ERR_OTHER
} build_status;

	// If dep is a file and target is older than dep
	// and the target id older than any of its dependencies, return true
	bool is_outdated(Vertex *v);

	/* execute build for a target vertex (DFS) */
	int build_vertex(Graph *g, Vertex *v);

	/* Reset cached flags (checked/outdated/built) for full rebuild or before calling build */
	void build_reset_flags(Graph *g);

	/* Execute a single shell command. Returns 0 on success, non-zero otherwise. */
	int exec_command(const char *cmd);

	/* convenience: build from a vertex name (find via hash table) */
	int build_by_name(Graph *g, const char *target_name);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FILESYSTEM_H