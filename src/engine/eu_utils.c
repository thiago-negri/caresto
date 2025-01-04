#include <stdio.h>
#include <sys/stat.h>

#include <engine/el_log.h>
#include <engine/eu_utils.h>

int eu_copy_file(const char *from, const char *to, struct em_arena *arena) {
    int rc = 0;
    FILE *fd = NULL;

    size_t arena_offset = em_arena_save_offset(arena);

    errno_t err = fopen_s(&fd, from, "rb");
    if (err != 0) {
        el_critical("U: Could not open for read: %s.\n", from);
        rc = -1;
        goto _err;
    }
    fseek(fd, 0, SEEK_END);
    long file_size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // +1 for NULL terminator
    char *buffer = (char *)em_arena_alloc(arena, file_size + 1);
    if (buffer == NULL) {
        el_critical("WIN: OOM: Could not allocate to read file %s.\n", from);
        rc = -1;
        goto _err;
    }
    memset(buffer, 0, file_size + 1);
    size_t read_count = fread(buffer, sizeof(char), file_size, fd);
    if (read_count != file_size) {
        el_critical("U: Something went wrong while reading %s.\n", from);
        rc = -1;
        goto _err;
    }

    fclose(fd);
    fd = 0;

    err = fopen_s(&fd, to, "wb");
    if (err != 0) {
        el_critical("U: Could not open for write: %s.\n", to);
        rc = -1;
        goto _err;
    }

    size_t write_count = fwrite(buffer, sizeof(char), file_size, fd);
    if (write_count != file_size) {
        el_critical("WIN: Something went wrong while writing %s.\n", to);
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
    em_arena_restore_offset(arena, arena_offset);
    return rc;
}

long long eu_file_timestamp(const char *path) {
    struct stat file_stat = {0};
    stat(path, &file_stat);
    long long timestamp = file_stat.st_mtime;
    return timestamp;
}
