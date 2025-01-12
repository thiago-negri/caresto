#include <engine/ef_file.h>
#include <engine/el_log.h>
#include <stdio.h>
#include <sys/stat.h>

int ef_copy(const char *to, const char *from, struct ea_arena *arena) {
    int rc = 0;
    FILE *fd = NULL;

    size_t arena_offset = ea_arena_save_offset(arena);

    errno_t err = fopen_s(&fd, from, "rb");
    if (err != 0) {
        el_critical_fmt("U: Could not open for read: %s.\n", from);
        rc = -1;
        goto _err;
    }
    fseek(fd, 0, SEEK_END);
    long file_size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // +1 for NULL terminator
    char *buffer = (char *)ea_arena_alloc(arena, file_size + 1);
    if (buffer == NULL) {
        el_critical_fmt("WIN: OOM: Could not allocate to read file %s.\n", from);
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
    fd = 0;

    err = fopen_s(&fd, to, "wb");
    if (err != 0) {
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

long long ef_timestamp(const char *path) {
    struct stat file_stat = {0};
    stat(path, &file_stat);
    long long timestamp = file_stat.st_mtime;
    return timestamp;
}
