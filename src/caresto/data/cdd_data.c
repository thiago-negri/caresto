#include <caresto/data/cdd_data.h>
#include <engine/et_test.h>

ET_TEST(cdd_sizes) {
    // Make sure it fits on persistent storage
    ET_ASSERT(sizeof(struct cdd_data) < 10 * 1024 * 1024);
    ET_DONE;
}
