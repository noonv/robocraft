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
    img->step = width*img->n_channels;
    switch(type) {
    case CV_DEPTH_8S:
        img->size *= sizeof(char);
        img->step *= sizeof(char);
        break;
    case CV_DEPTH_16S:
        img->size *= sizeof(short);
        img->step *= sizeof(short);
        break;
    case CV_DEPTH_32F:
        img->size *= sizeof(float);
        img->step *= sizeof(float);
        break;
    case CV_DEPTH_8U:
    default:
        img->size *= sizeof(unsigned char);
        img->step *= sizeof(unsigned char);
        break;
    }
    img->data = malloc(img->size);
    if(!img->data) {
        fprintf(stderr, "[!] Error: allocate memory!\n");
        free(img);
        return NULL;
    }
    memset(img->data, 0, img->size);
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

int image_rgb2gray(image* _src, image* _dst)
{
    if(!_src || !_dst) {
        return -1;
    }

    if(_src->n_channels != 3 && _dst->n_channels != 1) {
        fprintf(stderr, "[!][image_rgb2gray] Error: bad images!\n");
        return -1;
    }

    if(_src->width != _dst->width ||
       _src->height != _dst->height) {
        return -1;
    }

    int i;
    int counter;
    unsigned char* src = (unsigned char*)_src->data;
    unsigned char* dst = (unsigned char*)_dst->data;

    // GRAY = 0.299*R + 0.587*G + 0.114*B
    counter=0;
    for(i=0; i<(_src->size-2); i+=3) {
        //float temp = 0.2989f * src[i] + 0.5870f * src[i + 1] + 0.1140f * src[i + 2]; //RGB
        float temp = 0.2989f * src[i+2] + 0.5870f * src[i + 1] + 0.1140f * src[i]; //BGR
        dst[counter++] = temp;
    }

    return 0;
}

int image_gray2rgb(image* _src, image* _dst)
{
    if(!_src || !_dst) {
        return -1;
    }

    if(_src->n_channels != 1 && _dst->n_channels != 3) {
        fprintf(stderr, "[!][image_gray2rgb] Error: bad images!\n");
        return -1;
    }

    if(_src->width != _dst->width ||
       _src->height != _dst->height) {
        return -1;
    }

    int i;
    int counter;
    unsigned char* src = (unsigned char*)_src->data;
    unsigned char* dst = (unsigned char*)_dst->data;

    for(i=0; i<_src->size; i++) {
        dst[counter] = src[i];
        dst[counter+1] = src[i];
        dst[counter+2] = src[i];
        counter += 3;
    }

    return 0;
}

int image_rgb2hsv(image* _src, image* _dst)
{
    if(!_src || !_dst) {
        return -1;
    }

    if(_src->n_channels != 3 || _dst->n_channels != 3) {
        fprintf(stderr, "[!][image_rgb2hsv] Error: bad images!\n");
        return -1;
    }

    if(_src->width != _dst->width ||
       _src->height != _dst->height) {
        return -1;
    }

    int i;
    unsigned char* src = (unsigned char*)_src->data;
    unsigned char* dst = (unsigned char*)_dst->data;

    float H, S, V, min, max, delta;
    float R, G, B;
    for(i=0; i<(_src->size-2); i+=3) {
        // BGR
        B = src[i]/255.0;
        G = src[i+1]/255.0;
        R = src[i+2]/255.0;

        //------------------
        min = R < G ? R : G;
        min = min  < B ? min  : B;

        max = R > G ? R : G;
        max = max  > B ? max  : B;

        V = max;

        delta = max - min;

        if( max > 0.0 ) {
            S = (delta / max);
        }
        else {
            S = 0.0;
            H = 0;  // its now undefined
            dst[i] = dst[i+1] = dst[i+2] = 0;
            return 0;
        }

        if( R >= max )
            H = (G - B) / delta;        // between yellow & magenta
        else if( G >= max )
            H = 2.0 + (B - R) / delta;  // between cyan & yellow
        else
            H = 4.0 + (R - G) / delta;  // between magenta & cyan

        H *= 60.0;  // degrees

        if( H < 0.0 )
            H += 360.0;

        dst[i] = (unsigned char)(H/2.0);
        dst[i+1] = (unsigned char)(S*UCHAR_MAX);
        dst[i+2] = (unsigned char)(V*UCHAR_MAX);
        //------------------
    }

    return 0;
}

int image_hsv2rgb(image* _src, image* _dst)
{
    if(!_src || !_dst) {
        return -1;
    }

    if(_src->n_channels != 3 || _dst->n_channels != 3) {
        fprintf(stderr, "[!][image_hsv2rgb] Error: bad images!\n");
        return -1;
    }

    if(_src->width != _dst->width ||
       _src->height != _dst->height) {
        return -1;
    }

    int i;
    unsigned char* src = (unsigned char*)_src->data;
    unsigned char* dst = (unsigned char*)_dst->data;

    float H, S, V;
    float R, G, B;

    float hh, p, q, t, ff;
    long ii;

    for(i=0; i<(_src->size-2); i+=3) {
        H = src[i]*2;
        S = src[i+1]/255.0;
        V = src[i+2]/255.0;

//        if(S <= 0.0) {
//            dst[i] = dst[i+1] = dst[i+2] = (unsigned char)(V*UCHAR_MAX);
//            return 0;
//        }

        hh = H;
        if(hh >= 360.0)
            hh = 0.0;
        hh /= 60.0;
        ii = (long)hh;
        ff = hh - ii;
        p = V * (1.0 - S);
        q = V * (1.0 - (S * ff));
        t = V * (1.0 - (S * (1.0 - ff)));

        switch(ii) {
        case 0:
            R = V;
            G = t;
            B = p;
            break;
        case 1:
            R = q;
            G = V;
            B = p;
            break;
        case 2:
            R = p;
            G = V;
            B = t;
            break;
        case 3:
            R = p;
            G = q;
            B = V;
            break;
        case 4:
            R = t;
            G = p;
            B = V;
            break;
        case 5:
        default:
            R = V;
            G = p;
            B = q;
            break;
        }

        dst[i] = (unsigned char)(B*UCHAR_MAX);
        dst[i+1] = (unsigned char)(G*UCHAR_MAX);
        dst[i+2] = (unsigned char)(R*UCHAR_MAX);
    }

    return 0;
}

int image_convert_color(image* _src, image* _dst, int type)
{
    if(!_src || !_dst) {
        fprintf(stderr, "[!] Error: bad param!\n");
        return -1;
    }

    switch (type) {
    case CV_RGB2GRAY:
        // RGB to Gray
        return image_rgb2gray(_src, _dst);
        break;
    default:
        break;
    }

    return 0;
}

image* image_clone(image* src)
{
    if(!src)
        return NULL;

    image* img = image_create(src->width, src->height, src->n_channels, src->type);
    image_copy(src, img);
    return img;
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
