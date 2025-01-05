#ifndef EU_UTILS_H
#define EU_UTILS_H

#include <engine/em_memory.h>

int eu_copy_file(const char *from, const char *to, struct em_arena *arena);
long long eu_file_timestamp(const char *path);
long long eu_max(long long a, long long b, long long c);
float eu_lerp(float start, float end, float d);

// TODO(tnegri): Create assertions
#define eu_assert(X)

#endif // EU_UTILS_H
