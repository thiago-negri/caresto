#pragma once

#include <SDL3/SDL.h>

#define el_debug(msg) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, msg)

#define el_debug_fmt(fmt, ...)                                                 \
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, fmt, __VA_ARGS__)

#define el_info(msg) SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, msg)

#define el_info_fmt(fmt, ...)                                                  \
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, fmt, __VA_ARGS__)

#define el_error(msg) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, msg)

#define el_error_fmt(fmt, ...)                                                 \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, fmt, __VA_ARGS__)

#define el_critical(msg) SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, msg)

#define el_critical_fmt(fmt, ...)                                              \
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, fmt, __VA_ARGS__)

#define el_assert(x)                                                           \
    do {                                                                       \
        if (!(x)) {                                                            \
            el_critical_fmt("assertion %s:%d %s\n", __FILE__, __LINE__, #x);   \
            SDL_TriggerBreakpoint();                                           \
        }                                                                      \
    } while (0)

#define el_assert_fmt(x, fmt, ...)                                             \
    do {                                                                       \
        if (!(x)) {                                                            \
            el_critical_fmt(fmt, __VA_ARGS__);                                 \
            el_critical_fmt("assertion %s:%d %s\n", __FILE__, __LINE__, #x);   \
            SDL_TriggerBreakpoint();                                           \
        }                                                                      \
    } while (0)
