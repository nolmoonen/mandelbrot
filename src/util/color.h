#ifndef MANDELBROT_COLOR_H
#define MANDELBROT_COLOR_H

typedef struct {
    union {
        struct {
            uint8_t r, g, b;
        };
        struct {
            uint8_t h, s, v;
        };
    };
} color_t;

/** Converts a HSV color to a RGB one. */
color_t HSVtoRGB(color_t hsv);

#endif //MANDELBROT_COLOR_H
