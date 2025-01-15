#pragma once

#include <caresto/opengl/coo_opengl.h>
#include <engine/em_math.h>

#define CSD_DEBUG_MAX 100

struct csd_debug {
    unsigned int debug_count;
    struct coo_debug debug_gpu[CSD_DEBUG_MAX];
};

void csd_quad(struct csd_debug *debug, int x, int y, int w, int h);
