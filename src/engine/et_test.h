#ifndef ET_TEST_H
#define ET_TEST_H

#include <stdbool.h>

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

bool et_assert(const char *et_name, const char *message, bool assertion);
void et_done(const char *et_name);

#endif // ET_TEST_H