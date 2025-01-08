#ifndef CC_CAMERA
#define CC_CAMERA

struct cc_camera {
    float x, y, w, h;
};

struct cc_bounds {
    float left, right, top, bottom;
};

void cc_bounds(struct cc_bounds *bounds, struct cc_camera *cam);

#endif // CC_CAMERA
