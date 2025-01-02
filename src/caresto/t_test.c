#include <stdio.h>

#include <caresto/t_test.h>

bool t_assert(const char *test_name, const char *message, bool assertion) {
    if (!assertion) {
        fprintf(stderr, "FAIL: %s - %s\n", test_name, message);
    }
    return assertion;
}

void t_done(const char *test_name) { fprintf(stderr, "OK: %s\n", test_name); }
