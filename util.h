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

#endif //MANDELBROT_UTIL_H
