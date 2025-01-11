#include <caresto/cd_data.h>
#include <engine/et_test.h>

ET_TEST(cd_state) {
    // Make sure it fits on persistent storage
    ET_ASSERT(sizeof(struct cd_state) < 10 * 1024 * 1024);
    ET_DONE;
}
