/*
 * TinyCV
 */

#ifndef IMPROC_H
#define IMPROC_H

#include "tinycvtypes.h"
#include "image.h"

/* image routines */

// set image to zero
int image_zero(image* src);

// set image to value
int image_set(image* src, cv_scalar value);

// convert image from float to uchar
int image_float2uchar(image* src, image** dst, float min, float max);

enum {CV_BORDER_CONSTANT=0, CV_BORDER_REPLICATE};

// make image border  (cvCopyMakeBorder)
int image_border(image* src, image* dst, cv_point offset, int border_type, cv_scalar value);

// calc image gradient (cvSobel)
int image_gradient(image* src, image** magnitude, image** orientation);

// calc thin borders (cvCanny)
int image_thin_borders(image* src, image** dst);

void image_min_max_loc(image* src, double* minVal, double* maxVal, cv_point* minLoc, cv_point* maxLoc);

void image_threshold(image* src, image* dst, double threshold);

void image_resize(image* src, image* dst);

#endif /* #ifndef IMPROC_H */
