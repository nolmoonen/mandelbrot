//
// Created by Nol on 04/05/2019.
//

#ifndef MANDELBROT_COLOR_H
#define MANDELBROT_COLOR_H

/**
 * Red Green Blue color.
 */
struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

/**
 * Hue Saturation Value color.
 */
struct HSV {
    uint8_t h;
    uint8_t s;
    uint8_t v;
};

/**
 * Converts a HSV color to a RGB one.
 */
struct RGB HSVtoRGB(struct HSV hsv) {
    if (hsv.s) {
        // s != 0
        uint8_t region, remainder, p, q, t;
        region = hsv.h / 43;
        remainder = (hsv.h - (region * 43)) * 6;

        p = (hsv.v * (255 - hsv.s)) >> 8;
        q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
        t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

        struct RGB rgb = {};
        switch (region) {
            case 0:
                rgb.r = (uint8_t) hsv.v;
                rgb.g = t;
                rgb.b = p;
                break;
            case 1:
                rgb.r = q;
                rgb.g = (uint8_t) hsv.v;
                rgb.b = p;
                break;
            case 2:
                rgb.r = p;
                rgb.g = (uint8_t) hsv.v;
                rgb.b = t;
                break;
            case 3:
                rgb.r = p;
                rgb.g = q;
                rgb.b = (uint8_t) hsv.v;
                break;
            case 4:
                rgb.r = t;
                rgb.g = p;
                rgb.b = (uint8_t) hsv.v;
                break;
            default: // case 5:
                rgb.r = (uint8_t) hsv.v;
                rgb.g = p;
                rgb.b = q;
                break;
        }

        return rgb;
    } else {
        // s == 0
        struct RGB rgb = {(uint8_t) hsv.v, (uint8_t) hsv.v, (uint8_t) hsv.v};
        return rgb;
    }
}

#endif //MANDELBROT_COLOR_H
