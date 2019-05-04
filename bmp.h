/*
 * Created by Tijn (https://github.com/TijnBertens)
 * With slight C-specific alterations
 */

#ifndef MANDELBROT_BMP_H
#define MANDELBROT_BMP_H

/**
 * It is important that the bitmap header is not padded!
 */
#pragma pack(push, 1)

/**
 * The header of the .bmp file format.
 */
struct BitmapHeader {
    uint16_t fileType;
    uint32_t fileSize;
    uint16_t reserved0;
    uint16_t reserved1;
    uint32_t dataOffset;

    uint32_t headerSize;
    uint32_t width;
    uint32_t height;
    uint16_t numPlanes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t dataSize;
    int32_t horzResolution;
    int32_t vertResolution;
    uint32_t numColorsInPalette;
    uint32_t importantColors;
};
#pragma pack(pop)

#pragma pack(push, 1)
/**
 * ARGB formatted color, components are 8bit unsigned ints.
 */
struct IntColor {
    union {
        uint32_t color;
        struct {
            uint8_t b;
            uint8_t g;
            uint8_t r;
            uint8_t a;
        };
    };
};
#pragma pack(pop)

/**
 * An internal representation of a bitmap file, containing a .bmp header and color data.
 *
 * @note: There is no extra metadata in this struct, since everything can be found in/constructed from data in the header.
 */
struct Bitmap {
    struct BitmapHeader header;
    struct IntColor *data;
};

/**
 * Saves out a bitmap to a specified file path.
 */
uint32_t saveOutBitmap(struct Bitmap bitmap, const char *filePath) {
    FILE *outFile = fopen(filePath, "wb");

    if (outFile) {
        fwrite(&bitmap, sizeof(bitmap.header), 1, outFile);
        fwrite(bitmap.data, bitmap.header.dataSize, 1, outFile);
        fclose(outFile);
        return 0;
    } else {
        printf("Failed to save bitmap file, could not open file.");
        return 1;
    }
}

/**
 * Creates a bitmap with the given data, of the specified width and height. The header will automatically be filled in
 * with the standard values used in this program.
 */
struct Bitmap createBitmap(struct IntColor *data, uint32_t width, uint32_t height) {
    struct Bitmap result = {};

    result.header.fileType = 0x4D42;                            // 'BM'
    result.header.fileSize = sizeof(struct BitmapHeader) + (width * height * sizeof(uint32_t));
    result.header.dataOffset = sizeof(struct BitmapHeader);
    result.header.headerSize =
            sizeof(struct BitmapHeader) - 14; // 14 is the size of the file type/size, offset + reserved bytes
    result.header.width = width;
    result.header.height = height;
    result.header.numPlanes = 1;
    result.header.bitsPerPixel = 32;                            // RGBA format
    result.header.compression = 0;                              // no compression
    result.header.dataSize = (width * height * sizeof(uint32_t));    // again, 32 bits per pixels for RGBA
    result.header.horzResolution = 5000;                        // arbitrary....
    result.header.vertResolution = 5000;                        // arbitrary....
    result.header.numColorsInPalette = 0;                       // we don't use a palette
    result.header.importantColors = 0;

    result.data = data;

    return result;
}

#endif //MANDELBROT_BMP_H
