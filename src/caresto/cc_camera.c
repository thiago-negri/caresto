#include <caresto/cc_camera.h>
#include <engine/et_test.h>
#include <engine/eu_utils.h>
#include <string.h>

void cc_bounds(struct cc_bounds *bounds, struct cc_camera *cam) {
    eu_assert(bounds != NULL);
    eu_assert(cam != NULL);

    float half_w = cam->w * 0.5f;
    float half_h = cam->h * 0.5f;
    bounds->top = cam->y - half_h;
    bounds->left = cam->x - half_w;
    bounds->right = cam->x + half_w;
    bounds->bottom = cam->y + half_h;
}

ET_TEST(cc_bounds) {
    struct cc_bounds bounds = {0};
    struct cc_camera cam = {.x = 100.0f, .y = 50.0f, .w = 200.0f, .h = 50.0f};

    cc_bounds(&bounds, &cam);

    ET_ASSERT(bounds.left == 0.0f);
    ET_ASSERT(bounds.top == 25.0f);
    ET_ASSERT(bounds.right == 200.0f);
    ET_ASSERT(bounds.bottom == 75.0f);
    ET_DONE;
}
