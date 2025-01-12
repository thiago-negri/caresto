#pragma once

#include <engine/el_log.h>

#define CDI_IDS(CONTAINER, PREFIX, NAME, MAX)                                  \
    static size_t PREFIX##_##NAME##_new(struct CONTAINER *state,               \
                                        size_t index) {                        \
        for (size_t i = 0; i < MAX; i++) {                                     \
            if (state->NAME[i] == 0) {                                         \
                state->NAME[i] = index + 1;                                    \
                return i + 1;                                                  \
            }                                                                  \
        }                                                                      \
        el_assert_fmt(false, "out of ids for %s", #NAME);                      \
        return 0;                                                              \
    }                                                                          \
                                                                               \
    static size_t PREFIX##_##NAME##_get(struct CONTAINER *state, size_t id) {  \
        el_assert(id > 0 && id <= MAX);                                        \
        size_t index = state->NAME[id - 1];                                    \
        el_assert(index > 0);                                                  \
        return index - 1;                                                      \
    }                                                                          \
                                                                               \
    static void PREFIX##_##NAME##_move(struct CONTAINER *state,                \
                                       size_t new_index, size_t old_index) {   \
        el_assert(new_index > 0 && new_index <= MAX);                          \
        el_assert(old_index > 0 && old_index <= MAX);                          \
        for (size_t i = 0; i < MAX; i++) {                                     \
            if (state->NAME[i] == old_index + 1) {                             \
                state->NAME[i] = new_index + 1;                                \
                return;                                                        \
            }                                                                  \
        }                                                                      \
        el_assert_fmt(false, "cant find index on %s", #NAME);                  \
    }                                                                          \
                                                                               \
    static size_t PREFIX##_##NAME##_rm(struct CONTAINER *state, size_t id) {   \
        el_assert(id > 0 && id <= MAX);                                        \
        size_t index = state->NAME[id - 1];                                    \
        el_assert(index > 0);                                                  \
        state->NAME[id - 1] = 0;                                               \
        return index - 1;                                                      \
    }
