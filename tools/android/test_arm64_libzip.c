#include "zip.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s archive entry output\n", argv[0]);
        return 2;
    }
    int error = 0;
    struct zip *archive = zip_open(argv[1], 0, &error);
    if (!archive) {
        fprintf(stderr, "zip_open failed: %d\n", error);
        return 1;
    }
    int found = 0;
    int count = zip_get_num_files(archive);
    for (int i = 0; i < count; ++i) {
        const char *name = zip_get_name(archive, i, 0);
        if (name && strcmp(name, argv[2]) == 0) { found = 1; break; }
    }
    if (!found) {
        fprintf(stderr, "entry missing among %d names: %s\n", count, argv[2]);
        zip_close(archive);
        return 1;
    }
    struct zip_file *file = zip_fopen(archive, argv[2], 0);
    if (!file) {
        fprintf(stderr, "zip_fopen failed: %s\n", argv[2]);
        zip_close(archive);
        return 1;
    }
    FILE *output = fopen(argv[3], "wb");
    if (!output) {
        perror("fopen output");
        zip_fclose(file);
        zip_close(archive);
        return 1;
    }
    unsigned long long total = 0;
    int failed = 0;
    char buffer[8192];
    for (;;) {
        ssize_t size = zip_fread(file, buffer, sizeof buffer);
        if (size < 0) { failed = 1; break; }
        if (size == 0) { break; }
        if (fwrite(buffer, 1, size, output) != (size_t)size) { failed = 1; break; }
        total += (unsigned long long)size;
    }
    if (fclose(output) != 0) { failed = 1; }
    if (zip_fclose(file) != 0) { failed = 1; }
    if (zip_close(archive) != 0) { failed = 1; }
    if (failed) { fprintf(stderr, "archive read failed\n"); return 1; }
    printf("archive entries: %d; extracted bytes: %llu\n", count, total);
    return 0;
}
