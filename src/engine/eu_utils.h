#ifndef EU_UTILS_H
#define EU_UTILS_H

#include <engine/em_memory.h>

int eu_copy_file(const char *from, const char *to, struct em_arena *arena);
long long eu_file_timestamp(const char *path);
long long eu_max(long long a, long long b, long long c);

#endif // EU_UTILS_H
