#include <stdint.h>
#include "color.h"

/**
 * Converts a HSV color to a RGB one.
 */
color_t HSVtoRGB(color_t hsv) {
    if (hsv.s) {
        // s != 0
        uint8_t region, remainder, p, q, t;
        region = hsv.h / 43;
        remainder = (hsv.h - (region * 43)) * 6;

        p = (hsv.v * (255 - hsv.s)) >> 8;
        q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
        t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

        color_t rgb = {0};
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
        color_t rgb = {(uint8_t) hsv.v, (uint8_t) hsv.v, (uint8_t) hsv.v};
        return rgb;
    }
}