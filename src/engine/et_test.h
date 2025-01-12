#pragma once

#define ET_TEST(x) void et_##x##_(const char *et_name, int *et_done_called)

#define ET_DONE                                                                \
    {                                                                          \
        *et_done_called = 1;                                                   \
        et_done(et_name);                                                      \
    }

#define ET_ASSERT(x)                                                           \
    {                                                                          \
        if (!et_assert(et_name, #x, (x))) {                                    \
            *et_done_called = 1;                                               \
            return;                                                            \
        }                                                                      \
    }

#define ET_ASSERT_FMT(x, fmt, ...)                                             \
    {                                                                          \
        if (!et_assert(et_name, #x, (x))) {                                    \
            fprintf(stderr, fmt, __VA_ARGS__);                                 \
            *et_done_called = 1;                                               \
            return;                                                            \
        }                                                                      \
    }

bool et_assert(const char *et_name, const char *message, bool assertion);

void et_done(const char *et_name);
