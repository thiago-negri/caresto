#ifndef _T_TEST_H
#define _T_TEST_H

#include <stdbool.h>

#define T_TEST(x)                                                              \
    void t_##x##_(int *t_done_called) {                                        \
        const char *t_name = #x;                                               \
        if (1)

#define T_DONE                                                                 \
    {                                                                          \
        *t_done_called = 1;                                                    \
        t_done(t_name);                                                        \
    }                                                                          \
    }

#define T_ASSERT(x)                                                            \
    {                                                                          \
        if (!t_assert(t_name, #x, (x))) {                                      \
            *t_done_called = 1;                                                \
            return;                                                            \
        }                                                                      \
    }

bool t_assert(const char *test_name, const char *message, bool assertion);
void t_done(const char *test_name);

#endif // _T_TEST_H
