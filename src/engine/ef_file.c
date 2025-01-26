#include <engine/ea_allocator.h>
#include <engine/ef_file.h>
#include <engine/el_log.h>
#include <engine/et_test.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>

int ef_copy(const char *to, const char *from, struct ea_arena *arena) {
    int rc = 0;
    FILE *fd = NULL;

    size_t arena_offset = ea_arena_save_offset(arena);

    fd = fopen(from, "r");
    if (errno != 0) {
        el_critical_fmt("U: Could not open for read: %s. %d: %s\n", from, errno,
                        strerror(errno));
        rc = -1;
        goto _err;
    }
    fseek(fd, 0, SEEK_END);
    long file_size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // +1 for NULL terminator
    char *buffer = (char *)ea_arena_alloc(arena, file_size + 1);
    if (buffer == NULL) {
        el_critical_fmt("WIN: OOM: Could not allocate to read file %s.\n",
                        from);
        rc = -1;
        goto _err;
    }
    memset(buffer, 0, file_size + 1);
    size_t read_count = fread(buffer, sizeof(char), file_size, fd);
    if (read_count != (size_t)file_size) {
        el_critical_fmt("U: Something went wrong while reading %s.\n", from);
        rc = -1;
        goto _err;
    }

    fclose(fd);

    fd = fopen(to, "w");
    if (errno != 0) {
        el_critical_fmt("U: Could not open for write: %s.\n", to);
        rc = -1;
        goto _err;
    }

    size_t write_count = fwrite(buffer, sizeof(char), file_size, fd);
    if (write_count != (size_t)file_size) {
        el_critical_fmt("WIN: Something went wrong while writing %s.\n", to);
        rc = -1;
        goto _err;
    }

    fclose(fd);
    goto _done;

_err:
    if (fd != NULL) {
        fclose(fd);
    }

_done:
    ea_arena_restore_offset(arena, arena_offset);
    return rc;
}

ET_TEST(ef_copy) {
    unsigned char buffer[10];
    struct ea_arena arena = {};
    ea_arena_init(&arena, 10, buffer);

    FILE *fd = fopen("ef_copy.tmp", "w");
    fwrite("hello", sizeof(char), 5, fd);
    fclose(fd);

    ef_copy("ef_copy.tmp-to", "ef_copy.tmp", &arena);

    fd = fopen("ef_copy.tmp-to", "r");
    fread(buffer, sizeof(char), 5, fd);
    fclose(fd);

    ET_ASSERT(strcmp((const char *)buffer, "hello") == 0);

    ef_copy("ef_copy.tmp-to2", "ef_copy.tmp", &arena);

    fd = fopen("ef_copy.tmp-to2", "r");
    fread(buffer, sizeof(char), 5, fd);
    fclose(fd);

    ET_ASSERT(strcmp((const char *)buffer, "hello") == 0);

    ET_DONE;
}

long long ef_timestamp(const char *path) {
    struct stat file_stat = {0};
    stat(path, &file_stat);
    long long timestamp = file_stat.st_mtime;
    return timestamp;
}
