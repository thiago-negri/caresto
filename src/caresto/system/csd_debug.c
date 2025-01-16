#include <caresto/system/csd_debug.h>
#include <engine/el_log.h>

void csd_quad(struct cds_systems *systems, int x, int y, int w, int h) {
    el_assert(systems->debug.debug_count < CDS_DEBUG_MAX);
    struct em_color red = {1, 0, 0, 0.5};
    systems->debug.debug_gpu[systems->debug.debug_count++] =
        (struct coo_debug){
            .vertex =
                {
                    [0] = {.position = {.x = x, .y = y}, .color = red},
                    [1] = {.position = {.x = x, .y = y + h}, .color = red},
                    [2] = {.position = {.x = x + w, .y = y}, .color = red},
                    [3] = {.position = {.x = x + w, .y = y}, .color = red},
                    [4] = {.position = {.x = x, .y = y + h}, .color = red},
                    [5] = {.position = {.x = x + w, .y = y + h}, .color = red},
                },
        };
}
