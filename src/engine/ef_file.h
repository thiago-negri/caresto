#pragma once

#include <engine/ea_allocator.h>

int ef_copy(const char *to, const char *from, struct ea_arena *arena);

long long ef_timestamp(const char *path);
