/*
 * TinyCV
 */

#include "bmpload.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef MIPS
#define to_mips2(x) ( ((x) >> 8) | (((x) & 0xFF) << 8) )
#define to_mips4(x) ( ((x) >> 24) | ((((x)>>16) & 0xFF) << 8) | ((((x)>>8) & 0xFF) << 16) | (((x) & 0xFF)<<24) )
#endif

int _bmp_load(const char* filename, image* img)
{
    if(!filename || !img) {
        fprintf(stderr, "[!][_bmp_load] Error: bad param!\n");
        return -1;
    }

    FILE *fs;
    fs = fopen(filename, "rb");
    if(!fs) {
        fprintf(stderr, "[!][_bmp_load] Error: cant open file: %s!\n", filename);
        return -1;
    }

    BmpHeader header;
    BmpImageInfo info;

    if( fread(&header, sizeof(BmpHeader), 1, fs) != 1 ) {
        fprintf(stderr, "[!][_bmp_load] Error: reading bmp header!\n");
        return -1;
    }

    if( fread(&info, sizeof(BmpImageInfo), 1, fs) != 1 ) {
        fprintf(stderr, "[!][_bmp_load] Error: reading image info!\n");
        return -1;
    }

#ifdef MIPS
    header.fileSize = to_mips4(header.fileSize);
    header.offset = to_mips4(header.offset);
    info.headerSize = to_mips4(info.headerSize);
    info.width = to_mips4(info.width);
    info.height = to_mips4(info.height);
    info.planeCount = to_mips2(info.planeCount);
    info.bitDepth = to_mips2(info.bitDepth);
    info.compression = to_mips4(info.compression);
    info.compressedImageSize = to_mips4(info.compressedImageSize);
    info.horizontalResolution = to_mips4(info.horizontalResolution);
    info.verticalResolution = to_mips4(info.verticalResolution);
    info.numColors = to_mips4(info.numColors);
    info.importantColors = to_mips4(info.importantColors);
#endif //#ifdef MIPS

#if 1
    printf("[i] === BMP load %s ===\n", filename);
    printf("[i] BmpHeader: %d BmpImageInfo: %d\n", sizeof(BmpHeader), sizeof(BmpImageInfo));
    printf("[i] BMP header:\n" );
    printf("[i] signature: %c%c\n", header.signature[0],header.signature[1]);
    printf("[i] fileSize: %d\n", header.fileSize );
    printf("[i] reserved: %d offset: %d\n", header.reserved, header.offset );

    printf("[i] BMP Info:\n" );
    printf("[i] headerSize: %d\n", info.headerSize);
    printf("[i] size: %dx%d\n", info.width, info.height );
    printf("[i] planeCount: %d\n", info.planeCount );
    printf("[i] bitDepth: %d\n", info.bitDepth );
    printf("[i] compression: %d\n", info.compression );
    printf("[i] compressedImageSize: %d\n", info.compressedImageSize );
    printf("[i] horizontalResolution: %d\n", info.horizontalResolution );
    printf("[i] verticalResolution: %d\n", info.verticalResolution );
    printf("[i] numColors: %d\n", info.numColors );
    printf("[i] importantColors: %d\n", info.importantColors );
#endif

    if( info.numColors > 0 ) {
        fprintf(stderr, "[!][_bmp_load] Error: wrong pallete: %d!\n", info.numColors);
        return -1;
    }

    img->width = info.width;
    img->height = info.height;
    img->n_channels = info.bitDepth/8;
    img->type = CV_DEPTH_8U;

    img->size = img->width*img->height*img->n_channels*sizeof(unsigned char);
    img->step = img->width*img->n_channels*sizeof(unsigned char);

    if(img->data) {
        free(img->data);
    }
    img->data = malloc(img->size);
    if(!img->data) {
        fprintf(stderr, "[!][_bmp_load] Error: allocate memory!\n");
        return -1;
    }

    fseek(fs, header.offset, SEEK_SET);

    if( fread(img->data, sizeof(char), img->size, fs) != img->size ) {
        fprintf(stderr, "[!][_bmp_load] Error: reading image data!\n");
        return -1;
    }

    fclose(fs);

    return 0;
}

int _bmp_save(const char* filename, image* img)
{
    if(!filename || !img) {
        fprintf(stderr, "[!][_bmp_load] Error: bad param!\n");
        return -1;
    }

    FILE *fs;
    fs = fopen(filename, "w+b");
    if(!fs) {
        fprintf(stderr, "[!][_bmp_save] Error: cant open file: %s!\n", filename);
        return -1;
    }

    BmpHeader header;
    BmpImageInfo info;

    int size = img->size;
    if(img->n_channels == 1) {
        size = img->size * 3;
    }

    header.signature[0] = 'B';
    header.signature[1] = 'M';
    header.fileSize = size + sizeof(BmpHeader) + sizeof(BmpImageInfo);
    header.reserved = 0;
    header.offset = sizeof(BmpHeader) + sizeof(BmpImageInfo);

    info.headerSize = sizeof(BmpImageInfo);
    info.width = img->width;
    info.height = img->height;
    info.planeCount = 1;
    info.bitDepth = 24; //img->n_channels*8;
    info.compression = 0;
    info.compressedImageSize = size;
    info.horizontalResolution = 0;
    info.verticalResolution = 0;
    info.numColors = 0;
    info.importantColors = 0;

#ifdef MIPS
    header.fileSize = to_mips4(header.fileSize);
    header.offset = to_mips4(header.offset);
    info.headerSize = to_mips4(info.headerSize);
    info.width = to_mips4(info.width);
    info.height = to_mips4(info.height);
    info.planeCount = to_mips2(info.planeCount);
    info.bitDepth = to_mips2(info.bitDepth);
    info.compression = to_mips4(info.compression);
    info.compressedImageSize = to_mips4(info.compressedImageSize);
    info.numColors = to_mips4(info.numColors);
#endif //#ifdef MIPS

#if 0
    printf("[i] === save %s ...\n", filename);
    printf("[i] BmpHeader: %d BmpImageInfo: %d\n", sizeof(BmpHeader), sizeof(BmpImageInfo));
    printf("[i] BMP header:\n" );
    printf("[i] signature: %c%c\n", header.signature[0],header.signature[1]);
    printf("[i] fileSize: %d\n", header.fileSize );
    printf("[i] reserved: %d offset: %d\n", header.reserved, header.offset );

    printf("[i] BMP Info:\n" );
    printf("[i] headerSize: %d\n", info.headerSize);
    printf("[i] size: %dx%d\n", info.width, info.height );
    printf("[i] planeCount: %d\n", info.planeCount );
    printf("[i] bitDepth: %d\n", info.bitDepth );
    printf("[i] compression: %d\n", info.compression );
    printf("[i] compressedImageSize: %d\n", info.compressedImageSize );
    printf("[i] horizontalResolution: %d\n", info.horizontalResolution );
    printf("[i] verticalResolution: %d\n", info.verticalResolution );
    printf("[i] numColors: %d\n", info.numColors );
    printf("[i] importantColors: %d\n", info.importantColors );
#endif

    if( fwrite(&header, sizeof(BmpHeader), 1, fs) != 1 ) {
        fprintf(stderr, "[!][_bmp_save] Error: writing bmp header!\n");
        return -1;
    }

    if( fwrite(&info, sizeof(BmpImageInfo), 1, fs) != 1 ) {
        fprintf(stderr, "[!][_bmp_load] Error: writing image info!\n");
        return -1;
    }

    if(img->n_channels == 1) {
        int i,j;
        for(i=0; i<img->size; i++) {
            fwrite(&img->data[i], sizeof(char), 1, fs);
            fwrite(&img->data[i], sizeof(char), 1, fs);
            fwrite(&img->data[i], sizeof(char), 1, fs);
        }
    }
    else {
        if( fwrite(img->data, sizeof(char), img->size, fs) != img->size ) {
            fprintf(stderr, "[!][_bmp_load] Error: writing image data!\n");
            return -1;
        }
    }

    fclose(fs);

    return 0;
}
