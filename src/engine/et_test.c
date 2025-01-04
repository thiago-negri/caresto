#include <stdio.h>

#include <engine/et_test.h>

bool et_assert(const char *et_name, const char *message, bool assertion) {
    if (!assertion) {
        fprintf(stderr, "FAIL: %s - %s\n", et_name, message);
    }
    return assertion;
}

void et_done(const char *et_name) { fprintf(stderr, "OK: %s\n", et_name); }
