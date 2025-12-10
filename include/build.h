#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "graph.h"
#include "filesystem.h"

// If dep is a file and target is older than dep
// and the target id older than any of its dependencies, return true
bool is_outdated(Vertex *v);

#endif // FILESYSTEM_H