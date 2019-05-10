//
// Created by Nol on 10/05/2019.
//

#ifndef MANDELBROT_UTIL_H
#define MANDELBROT_UTIL_H

int charArrayCompare(const char *a, const char *b) {
    uint32_t index = 0;
    while (*(a + index) == *(b + index) && *(a + index) != '\0' && *(b + index) != '\0') {
        ++index;
    }

    if (*(a + index) == '\0' && *(b + index) == '\0') {
        fprintf(stdout, "vulkan: found requested validation layer %s\n", a);

        return 0; // succes: strings are the same
    }

    return 1; // not succes: strings are different
}

uint32_t min(uint32_t a, uint32_t b) {
    return a < b ? a : b;
}

uint32_t max(uint32_t a, uint32_t b) {
    return a > b ? a : b;
}

static char *readFile(const char *filename) {
    char *buffer = 0;
    FILE *f = fopen(filename, "rb");

    if (f) {
        fseek(f, 0, SEEK_END);
        long length = ftell(f);
        fseek(f, 0, SEEK_SET);

        buffer = malloc(length + 1);
        if (buffer) {
            fread(buffer, 1, length, f);
            buffer[length] = '\0';
        }
        fclose(f);
    } else {
        fprintf(stderr, "util: could not open file\n");
    }

    return buffer;
}

static uint32_t fileSize(const char *filename) {
    FILE *f = fopen(filename, "rb");

    if (f) {
        fseek(f, 0, SEEK_END);
        return ftell(f);
    } else {
        fprintf(stderr, "util: could not open file\n");
    }

    return 0;
}

#endif //MANDELBROT_UTIL_H
