/*
 * TinyCV
 */

#include "improc.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// set image to zero
int image_zero(image* src)
{
    if(!src) {
        return -1;
    }

    if(src->data) {
        memset(src->data, 0, src->size);
    }

    return 0;
}

// set image to value
int image_set(image* src, cv_scalar value)
{
    if(!src) {
        return -1;
    }

    int x,y;
    for(y=0; y<src->height; y++) {
        for(x=0; x<src->width; x++) {
            src->data[y*src->step + x] = value.val[0];
            if(src->n_channels == 3) {
                src->data[y*src->step + x+1] = value.val[1];
                src->data[y*src->step + x+2] = value.val[2];
            }
        }
    }

    return 0;
}

// convert image from float to uchar
int image_float2uchar(image* src, image** dst, float min, float max)
{
    if(!src || !dst) {
        return -1;
    }

    if( src->type != CV_DEPTH_32F || max < min || fabs(max - min)<0.001f) {
        fprintf(stderr, "[!][image_float2uchar] Error: bad param!\n");
        return -1;
    }

    printf("[i][image_float2uchar] %d %d %d (%d)\n", src->width, src->height, src->n_channels, src->step);

    *dst = image_create(src->width, src->height, src->n_channels, CV_DEPTH_8U);

    if(!*dst) {
        fprintf(stderr, "[!] Error: allocate memory!\n");
        return -1;
    }

    printf("[i][image_float2uchar] %d %d %d (%d)\n", (*dst)->width, (*dst)->height, (*dst)->n_channels, (*dst)->step);

    float scale = UCHAR_MAX / (max - min);

    unsigned char val;
    float fval;

    int x, y;
//    int i, j;
//    for( j=1, y=0; j<src->height-1; j++, y++) {
//        for( i=1, x=0; i<src->width-1; i++, x++) {
//            fval = CV_PIXEL(CVTYPE_DEPTH_32F, src, i, j)[0];
    for (y = 0; y < src->height; y++) {
        for (x = 0; x < src->width; x++) {
            fval = CV_PIXEL(CVTYPE_DEPTH_32F, src, x, y)[0];
            val = (unsigned char)((fval - min) * scale);
            CV_PIXEL(CVTYPE_DEPTH_8U, *dst, x, y)[0] = val;

            if(src->n_channels == 3) {
                fval = CV_PIXEL(CVTYPE_DEPTH_32F, src, x, y)[1];
                val = (unsigned char)((fval - min) * scale);
                CV_PIXEL(CVTYPE_DEPTH_8U, *dst, x, y)[1] = val;

                fval = CV_PIXEL(CVTYPE_DEPTH_32F, src, x, y)[2];
                val = (unsigned char)((fval - min) * scale);
                CV_PIXEL(CVTYPE_DEPTH_8U, *dst, x, y)[2] = val;
            }
        }
    }

    return 0;
}

// make image border  (cvCopyMakeBorder)
int image_border(image* src, image* dst, cv_point offset, int border_type, cv_scalar value)
{
    if(!src || !dst) {
        return -1;
    }

    if(dst->width-src->width < offset.x || dst->height-src->height < offset.y) {
        fprintf( stderr, "[!] Error: bad image size or offset!\n");
        return -1;
    }

    if( (src->type != CV_DEPTH_8S && src->type != CV_DEPTH_8U) ||
            (dst->type != CV_DEPTH_8S && dst->type != CV_DEPTH_8U) ) {
        fprintf( stderr, "[!][image_border] Error: bad image format!\n");
        return -1;
    }


    int x, y, i, j;

    if(border_type == CV_BORDER_CONSTANT) {
        image_set(dst, value);
    }
    else if(border_type == CV_BORDER_REPLICATE) {
        ///@TODO
    }

    for(y=0, j=offset.y; y<src->height; j++, y++) {
        for(x=0, i=offset.x; x<src->width; i++, x++) {
            ((CVTYPE_DEPTH_8U*)dst->data)[j*dst->step + i] = ((CVTYPE_DEPTH_8U*)src->data)[y*src->step + x];
        }
    }

    return 0;
}

// calc image gradient (cvSobel)
int image_gradient(image* src, image** magnitude, image** orientation)
{
    if(!src) {
        return -1;
    }

    if(src->n_channels != 1) {
        fprintf(stderr, "[!][image_gradient] Error: convert to grayscale first!\n");
        return -1;
    }

    image* image = image_create(src->width+2, src->height+2, src->n_channels, src->type);
    *magnitude = image_create( src->width+2, src->height+2, src->n_channels, CV_DEPTH_32F);
    *orientation = image_create( src->width+2, src->height+2, src->n_channels, CV_DEPTH_32F);

    if(!image || !*magnitude || !*orientation) {
        fprintf(stderr, "[!] Error: allocate memory!\n");
        return -1;
    }

    image_zero(*magnitude);
    image_zero(*orientation);

    // make borders
    image_border(src, image, _cv_point(1, 1), CV_BORDER_CONSTANT, _cv_scalar_all(0));

#if 0
    // debug
    image_save(image, "test_borders.bmp");
#endif

    int i, j;
    float dx, dy;

    for(j=1; j<image->height-1; j++) {
        for(i=1; i<image->width-1; i++) {
            // calc grad
            dx = (float)( CV_PIXEL(CVTYPE_DEPTH_8U, image, i+1, j)[0] - CV_PIXEL(CVTYPE_DEPTH_8U, image, i-1, j)[0] );
            dy = (float)( CV_PIXEL(CVTYPE_DEPTH_8U, image, i, j+1)[0] - CV_PIXEL(CVTYPE_DEPTH_8U, image, i, j-1)[0] );

            CV_PIXEL(CVTYPE_DEPTH_32F, *magnitude, i, j)[0] = sqrtf(dx*dx + dy*dy);
            CV_PIXEL(CVTYPE_DEPTH_32F, *orientation, i, j)[0] = atan2f(dy, dx);
        }
    }

    image_delete(&image);

    return 0;

}

// (for image_thin_borders) define for what one of 4 types relate this angle
static int _what_angle(float val)
{
    float grad = RAD_TO_DEG(val);

    grad /= 2;

    if( (grad<=22.5 && grad> 337.5) /*|| (grad>157.5 && grad<=202.5)*/ ) {
        return 0;
    }
    else if( (grad>22.5 && grad<=67.5) /*|| (grad>202.5 && grad<=247.5)*/ ) {
        return 45;
    }
    else if( (grad>67.5 && grad<=112.5) /*|| (grad>247.5 && grad<=292.5)*/ ) {
        return 90;
    }
    else {
        return 135;
    }

    return 0;
}

// calc thin borders (cvCanny)
int image_thin_borders(image* src, image** dst)
{
    if(!src) {
        return -1;
    }

    if(src->n_channels!=1) {
        fprintf(stderr, "[!] convert to grayscale first!\n");
        return -1;
    }

    image* magnitude = NULL;
    image* orientation = NULL;

    image_gradient(src, &magnitude, &orientation);

    if(!magnitude || !orientation){
        fprintf( stderr, "[!] Error: image_gradient()!\n");
        return -1;
    }

#if 0
    // for debug purposes
    // save magnitude and orientation
    image* _mg = NULL;
    image* _or = NULL;
    image_float2uchar(magnitude, &_mg, 0, 255);
    image_float2uchar(orientation, &_or, 0, 255);
    if(_mg && _or) {
        image_save(_mg, "test_magnitude.bmp");
        image_save(_or, "test_orientation.bmp");
        image_delete(&_mg);
        image_delete(&_or);
    }
#endif

    *dst = image_create(src->width, src->height, 1, CV_DEPTH_8U);
    if(!*dst) {
        fprintf( stderr, "[!] Error: allocate memory!\n");
        return -1;
    }

    image_zero(*dst);

    int i, j, x, y;

    for( j=1, y=0; j<magnitude->height-1; j++, y++) {
        for( i=1, x=0; i<magnitude->width-1; i++, x++) {
            float m = CV_PIXEL(CVTYPE_DEPTH_32F, magnitude, i, j)[0];
            float ori = CV_PIXEL(CVTYPE_DEPTH_32F, orientation, i, j)[0];
            float m1=0, m2=0;

            unsigned char val = 0;

            // depends on gradient angle
            // check corresponding neighbours
            switch( _what_angle(ori) ) {
            case 0:
                m1 = CV_PIXEL(CVTYPE_DEPTH_32F, magnitude, i, j-1)[0];
                m2 = CV_PIXEL(CVTYPE_DEPTH_32F, magnitude, i, j+1)[0];
                if(m>m1 && m>m2){
                    val=255;
                }
                break;
            case 45:
                m1 = CV_PIXEL(CVTYPE_DEPTH_32F, magnitude, i+1, j-1)[0];
                m2 = CV_PIXEL(CVTYPE_DEPTH_32F, magnitude, i-1, j+1)[0];
                if(m>m1 && m>m2){
                    val=255;
                }
                break;
            case 90:
                m1 = CV_PIXEL(CVTYPE_DEPTH_32F, magnitude, i+1, j)[0];
                m2 = CV_PIXEL(CVTYPE_DEPTH_32F, magnitude, i-1, j)[0];
                if(m>m1 && m>m2){
                    val=255;
                }
                break;
            case 135:
                m1 = CV_PIXEL(CVTYPE_DEPTH_32F, magnitude, i-1, j-1)[0];
                m2 = CV_PIXEL(CVTYPE_DEPTH_32F, magnitude, i+1, j+1)[0];
                if(m>m1 && m>m2){
                    val=255;
                }
                break;
            default:
                break;
            }

            CV_PIXEL(CVTYPE_DEPTH_8U, *dst, x, y)[0] = val;
        }
    }

    image_delete(&magnitude);
    image_delete(&orientation);

    return 0;
}

void image_min_max_loc(image* _src, double* minVal, double* maxVal, cv_point* minLoc, cv_point* maxLoc)
{
    if(!_src || !minVal || !maxVal) {
        return;
    }

    if(_src->n_channels != 1)
        return;

    unsigned char* src = (unsigned char*)_src->data;

    double min, max;
    int x, y;
    for (y = 0; y < _src->height; y++) {
        for (x = 0; x < _src->width; x++) {
            double val = src[y*_src->step + x];
            if (min > val) {
                min = val;
                if(minLoc) {
                    minLoc->x = x;
                    minLoc->y = y;
                }
            }
            if (max < val) {
                max = val;
                min = val;
                if(maxLoc) {
                    maxLoc->x = x;
                    maxLoc->y = y;
                }
            }
        }
    }

    *minVal = min;
    *maxVal = max;
}

void image_threshold(image* _src, image* _dst, double threshold)
{
    if(!_src || !_dst) {
        return;
    }

    if(_src->width != _dst->width || _src->height != _dst->height) {
        fprintf(stderr, "[!][image_threshold] Error: different size!\n");
        return;
    }

    unsigned char* src = (unsigned char*)_src->data;
    unsigned char* dst = (unsigned char*)_dst->data;

    int x, y;
    for (y = 0; y < _src->height; y++) {
        for (x = 0; x < _src->width; x++) {
            dst[y*_dst->step + x] = (src[y*_src->step + x] > threshold )?src[y*_src->step + x]:0;
        }
    }
}

void image_resize(image* _src, image* _dst)
{
    if(!_src || !_dst) {
        return;
    }

    if(_src->n_channels != _dst->n_channels || _src->type != CV_DEPTH_8U) {
        fprintf(stderr, "[!][image_resize] Error: bad param!\n");
        return;
    }

    if(_src->width < _dst->width || _src->height < _dst->height) {
        fprintf(stderr, "[!][image_resize] Error: not implemented!\n");
        return;
    }

    image* img_tmp = image_create(_dst->width, _dst->height, _dst->n_channels, CV_DEPTH_32F);
    if(!img_tmp)
        return;

    int x, y, i, j;
    int step_x = _src->width / _dst->width;
    int step_y = _src->height / _dst->height;
    int step_val = step_x*step_y;
    //printf("[i] [image_resize] %d %d %d\n", step_x, step_y, step_val);
    for (y = 0, j=0; y < _src->height; y++) {
        for (x = 0, i=0; x < _src->width; x++) {
            //printf("[i] %d %d %d %d\n", x, y, i, j);
            CV_PIXEL(CVTYPE_DEPTH_32F, img_tmp, i, j)[0] += CV_PIXEL(CVTYPE_DEPTH_8U, _src, x, y)[0];

            if(_src->n_channels == 3) {
                CV_PIXEL(CVTYPE_DEPTH_32F, img_tmp, i, j)[1] += CV_PIXEL(CVTYPE_DEPTH_8U, _src, x, y)[1];
                CV_PIXEL(CVTYPE_DEPTH_32F, img_tmp, i, j)[2] += CV_PIXEL(CVTYPE_DEPTH_8U, _src, x, y)[2];
            }
            if(x>0 && x%step_x==0) i++;
        }
        if(y>0 && y%step_y==0) j++;
    }

    for (y = 0; y < _dst->height; y++) {
        for (x = 0; x < _dst->width; x++) {
            CV_PIXEL(CVTYPE_DEPTH_8U, _dst, x, y)[0]  = (CVTYPE_DEPTH_8U)(CV_PIXEL(CVTYPE_DEPTH_32F, img_tmp, x, y)[0]/step_val);

            if(_dst->n_channels == 3) {
                CV_PIXEL(CVTYPE_DEPTH_8U, _dst, x, y)[1] = (CVTYPE_DEPTH_8U)(CV_PIXEL(CVTYPE_DEPTH_32F, img_tmp, x, y)[1]/step_val);
                CV_PIXEL(CVTYPE_DEPTH_8U, _dst, x, y)[2] = (CVTYPE_DEPTH_8U)(CV_PIXEL(CVTYPE_DEPTH_32F, img_tmp, x, y)[2]/step_val);
            }
        }
    }

    image_delete(&img_tmp);
}
