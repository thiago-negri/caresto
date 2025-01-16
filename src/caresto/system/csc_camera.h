#pragma once

#include <caresto/data/cds_systems.h>

struct csc_bounds {
    float left, right, top, bottom;
};

void csc_bounds(struct csc_bounds *bounds, struct cds_camera *cam);
