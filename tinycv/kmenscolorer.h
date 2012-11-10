/*
 * TinyCV
 */

#ifndef KMEANSCOLORER_H
#define KMEANSCOLORER_H

#include "tinycvtypes.h"
#include "image.h"
#include "improc.h"

/**
const cv_scalar BLACK = CV_RGB(0, 0, 0);
const cv_scalar WHITE = CV_RGB(255, 255, 255);

const cv_scalar RED = CV_RGB(255, 0, 0);
const cv_scalar ORANGE = CV_RGB(255, 100, 0);
const cv_scalar YELLOW = CV_RGB(255, 255, 0);
const cv_scalar GREEN = CV_RGB(0, 255, 0);
const cv_scalar LIGHTBLUE = CV_RGB(60, 170, 255);
const cv_scalar BLUE = CV_RGB(0, 0, 255);
const cv_scalar VIOLET = CV_RGB(194, 0, 255);

const cv_scalar GINGER = CV_RGB(215, 125, 49);
const cv_scalar PINK = CV_RGB(255, 192, 203);
const cv_scalar LIGHTGREEN = CV_RGB(153, 255, 153);
const cv_scalar BROWN = CV_RGB(150, 75, 0);
/**/

typedef struct cv_color_cluster {
    int id;
    cv_scalar color;
    cv_scalar new_color;
    int count;

} cv_color_cluster;

CV_INLINE cv_color_cluster _cv_color_cluster()
{
    cv_color_cluster cc;
    cc.id = 0;
    cc.color = _cv_scalar_all(0);
    cc.new_color = _cv_scalar_all(0);
    cc.count = 0;
    return cc;
}

float rgb_euclidean(cv_scalar p1, cv_scalar p2);

// define dominate colors on image by k-meanes algorithm
//
// src - [in]- source image
// dst - [out] - image in dominate colors
// cluster_indexes - [out] - 1-column image with cluster numbers
// clusters -[out] - pointer to array of cv_color_cluster
// cluster_count - [in] - number elements of "clusters"
//
// RETURN: number of dominate colors (number elements of "clusters" with count != 0)
//
int image_kmeans_colorer(image* src, image* dst, image* cluster_indexes, cv_color_cluster* clusters, int cluster_count);

//--------

// Determine what type of color the HSV pixel is. Returns the colorType between 0 and NUM_COLOR_TYPES.
int hsv_get_pixel_color_type(int H, int S, int V);

// define dominate colors on image by HSV-colors
int image_hsv_colorer(image* src, image* dst, image* cluster_indexes, cv_color_cluster* clusters, int cluster_count);

#endif /* #ifndef KMEANSCOLORER_H */
