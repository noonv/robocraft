/*
 * TinyCV
 */

#include "image.h"
#include "bmpload.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

image* image_create(int width, int height, int n_channels, int type)
{
    image* img = malloc(sizeof(image));
    if(!img) {
        fprintf(stderr, "[!] Error: allocate memory!\n");
        return NULL;
    }

    img->width = width;
    img->height = height;
    img->n_channels = n_channels;
    img->type = type;
    img->size = width*height*n_channels;
    switch(type)
    {
    case CV_DEPTH_8S:
        img->size *= sizeof(char);
        img->step = width*img->n_channels*sizeof(char);
        break;
    case CV_DEPTH_16S:
        img->size *= sizeof(short);
        img->step = width*img->n_channels*sizeof(short);
        break;
    case CV_DEPTH_32F:
        img->size *= sizeof(float);
        img->step = width*img->n_channels*sizeof(float);
        break;
    case CV_DEPTH_8U:
    default:
        img->size *= sizeof(unsigned char);
        img->step = width*img->n_channels*sizeof(unsigned char);
        break;
    }
    img->data = malloc(img->size);
    if(!img->data) {
        fprintf(stderr, "[!] Error: allocate memory!\n");
        free(img);
        return NULL;
    }
    return img;
}

int image_delete(image** img)
{
    if(*img) {
        if( (*img)->data ) {
            free((*img)->data);
            (*img)->data = NULL;
        }
        free(*img);
        (*img) = NULL;
    }
    return 0;
}

image* image_load(const char* filename)
{
    if(!filename) {
        fprintf(stderr, "[!] Error: bad file name!\n");
        return NULL;
    }

    image* img = malloc(sizeof(image));
    if(!img) {
        fprintf(stderr, "[!] Error: allocate memory!\n");
        return NULL;
    }

    int res = -1;

    char* ext = strchr(filename, '.');
    if(ext) {
        ext++;

        if( !strncmp(ext, "bmp", 3) ) {
            res = _bmp_load(filename, img);
        }
    }

    if( res == -1 ) {
        fprintf(stderr, "[!] Error: load image!\n");
        free(img);
        return NULL;
    }

    return img;
}

int image_save(image* img, const char* filename)
{
    if(!img || !filename) {
        fprintf(stderr, "[!] Error: bad param!\n");
        return -1;
    }

    int res = -1;

    char* ext = strchr(filename, '.');
    if(ext) {
        ext++;

        if( !strncmp(ext, "bmp", 3) ) {
            res = _bmp_save(filename, img);
        }
    }

    if( res == -1 ) {
        fprintf(stderr, "[!] Error: save image!\n");
        return -1;
    }

    return 0;
}

int image_convert_color(image* _src, image* _dst, int type)
{
    if(!_src || !_dst) {
        fprintf(stderr, "[!] Error: bad param!\n");
        return -1;
    }

    int i,j, counter;
    unsigned char* src = (unsigned char*)_src->data;
    unsigned char* dst = (unsigned char*)_dst->data;

    switch (type) {
    case CV_RGB2GRAY:
        // RGB to Gray

        if(_src->n_channels != 3 && _dst->n_channels != 1) {
            fprintf(stderr, "[!][image_convert_color] Error: bad images for CV_RGB2GRAY!\n");
            return -1;
        }

        // GRAY = 0.299*R + 0.587*G + 0.114*B
        counter=0;
        for(i=0; i<(_src->size-2); i+=3) {
            //float temp = 0.2989f * src[i] + 0.5870f * src[i + 1] + 0.1140f * src[i + 2]; //RGB
            float temp = 0.2989f * src[i+2] + 0.5870f * src[i + 1] + 0.1140f * src[i]; //BGR
            dst[counter++] = temp;
        }

        break;
    default:
        break;
    }

    return 0;
}

int image_copy(image* src, image* dst)
{
    if(!src || !dst) {
        fprintf(stderr, "[!] Error: bad param!\n");
        return -1;
    }

    if(src->width == dst->width &&
       src->height == dst->height &&
       src->type == dst->type &&
       src->size == dst->size &&
       src->data && dst->data) {
        memcpy(dst->data, src->data, src->size);
    }
    else {
        fprintf(stderr, "[!][image_copy] Error: wrong images!\n");
        return -1;
    }

    return 0;
}

void image_rotate180(image *_src)
{
    if(!_src)
        return;

    int x, y, i, j;
    CVTYPE_DEPTH_8U tmp;
    int h2 = _src->height/2;
    for (y = 0, j=_src->height-1; y < h2; y++, j--) {
        for (x = 0, i=_src->width-1; x < _src->width; x++, i--) {
            tmp = CV_PIXEL(CVTYPE_DEPTH_8U,_src,x,y)[0];
            CV_PIXEL(CVTYPE_DEPTH_8U,_src,x,y)[0] = CV_PIXEL(CVTYPE_DEPTH_8U,_src,i,j)[0];
            CV_PIXEL(CVTYPE_DEPTH_8U,_src,i,j)[0] = tmp;
            if(_src->n_channels == 3) {
                tmp = CV_PIXEL(CVTYPE_DEPTH_8U,_src,x,y)[1];
                CV_PIXEL(CVTYPE_DEPTH_8U,_src,x,y)[1] = CV_PIXEL(CVTYPE_DEPTH_8U,_src,i,j)[1];
                CV_PIXEL(CVTYPE_DEPTH_8U,_src,i,j)[1] = tmp;

                tmp = CV_PIXEL(CVTYPE_DEPTH_8U,_src,x,y)[2];
                CV_PIXEL(CVTYPE_DEPTH_8U,_src,x,y)[2] = CV_PIXEL(CVTYPE_DEPTH_8U,_src,i,j)[2];
                CV_PIXEL(CVTYPE_DEPTH_8U,_src,i,j)[2] = tmp;
            }
        }
    }
}

void image_reflect_vertical(image *_src)
{
    if(!_src)
        return;

    int x, y, i, j;
    CVTYPE_DEPTH_8U tmp;
    int h2 = _src->height/2;
    for (y = 0, j=_src->height-1; y < h2; y++, j--) {
        for (x = 0, i=0; x < _src->width; x++, i++) {
            tmp = CV_PIXEL(CVTYPE_DEPTH_8U,_src,x,y)[0];
            CV_PIXEL(CVTYPE_DEPTH_8U,_src,x,y)[0] = CV_PIXEL(CVTYPE_DEPTH_8U,_src,i,j)[0];
            CV_PIXEL(CVTYPE_DEPTH_8U,_src,i,j)[0] = tmp;
            if(_src->n_channels == 3) {
                tmp = CV_PIXEL(CVTYPE_DEPTH_8U,_src,x,y)[1];
                CV_PIXEL(CVTYPE_DEPTH_8U,_src,x,y)[1] = CV_PIXEL(CVTYPE_DEPTH_8U,_src,i,j)[1];
                CV_PIXEL(CVTYPE_DEPTH_8U,_src,i,j)[1] = tmp;

                tmp = CV_PIXEL(CVTYPE_DEPTH_8U,_src,x,y)[2];
                CV_PIXEL(CVTYPE_DEPTH_8U,_src,x,y)[2] = CV_PIXEL(CVTYPE_DEPTH_8U,_src,i,j)[2];
                CV_PIXEL(CVTYPE_DEPTH_8U,_src,i,j)[2] = tmp;
            }
        }
    }
}
