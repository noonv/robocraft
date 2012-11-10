/*
 * TinyCV
 */

#ifndef BMPLOAD_H
#define BMPLOAD_H

#include "image.h"

#pragma pack(1)

typedef struct
{
    char signature[2];
    unsigned int fileSize;
    unsigned int reserved;
    unsigned int offset;
} BmpHeader;

typedef struct
{
    unsigned int headerSize;
    unsigned int width;
    unsigned int height;
    unsigned short planeCount;
    unsigned short bitDepth;
    unsigned int compression;
    unsigned int compressedImageSize;
    unsigned int horizontalResolution;
    unsigned int verticalResolution;
    unsigned int numColors;
    unsigned int importantColors;

} BmpImageInfo;

typedef struct
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    //unsigned char reserved; Removed for convenience in fread; info.bitDepth/8 doesn't seem to work for some reason
} Rgb;

int _bmp_load(const char* filename, image* img);

int _bmp_save(const char* filename, image* img);

#endif /* #ifndef BMPLOAD_H */
