#include <caresto/data/cds_state.h>
#include <engine/et_test.h>

ET_TEST(cds_state) {
    // Make sure it fits on persistent storage
    ET_ASSERT(sizeof(struct cds_state) < 10 * 1024 * 1024);
    ET_DONE;
}
