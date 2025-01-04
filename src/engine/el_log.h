#ifndef EL_LOG_H
#define EL_LOG_H

#include <SDL3/SDL.h>

#define l_debug(fmt, ...)                                                      \
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, fmt, __VA_ARGS__)
#define l_info(fmt, ...)                                                       \
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, fmt, __VA_ARGS__)
#define l_error(fmt, ...)                                                      \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, fmt, __VA_ARGS__)
#define l_critical(fmt, ...)                                                   \
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, fmt, __VA_ARGS__)

#endif // EL_LOG_H
