#pragma once

struct csc_camera {
    float x, y, w, h;
};

struct csc_bounds {
    float left, right, top, bottom;
};

void csc_bounds(struct csc_bounds *bounds, struct csc_camera *cam);
