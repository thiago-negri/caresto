#pragma once

#include <stdio.h>

#define ET_TEST(x) void et_##x##_(const char *et_name, int *et_done_called)

#define ET_DONE                                                                \
    {                                                                          \
        *et_done_called = 1;                                                   \
        et_done(et_name);                                                      \
    _et_done:                                                                  \
    }

#define ET_ASSERT(x)                                                           \
    {                                                                          \
        fprintf(stdout, #x);                                                   \
        if (!et_assert(et_name, #x, (x))) {                                    \
            *et_done_called = 1;                                               \
            goto _et_done;                                                     \
        }                                                                      \
        fprintf(stdout, " OK\n");                                              \
    }

#define ET_ASSERT_FMT(x, fmt, ...)                                             \
    {                                                                          \
        if (!et_assert(et_name, #x, (x))) {                                    \
            fprintf(stderr, fmt, __VA_ARGS__);                                 \
            *et_done_called = 1;                                               \
            goto _et_done;                                                     \
        }                                                                      \
    }

bool et_assert(const char *et_name, const char *message, bool assertion);

void et_done(const char *et_name);
