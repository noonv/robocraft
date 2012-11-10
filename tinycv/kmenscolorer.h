/*
 * TinyCV
 */

#ifndef KMEANSCOLORER_H
#define KMEANSCOLORER_H

#include "tinycvtypes.h"
#include "image.h"
#include "improc.h"

#define CV_RGB( r, g, b )  _cv_scalar( (b), (g), (r), 0 )

/**
cv_scalar BLACK = CV_RGB(0, 0, 0);
cv_scalar WHITE = CV_RGB(255, 255, 255);

cv_scalar RED = CV_RGB(255, 0, 0);
cv_scalar ORANGE = CV_RGB(255, 100, 0);
cv_scalar YELLOW = CV_RGB(255, 255, 0);
cv_scalar GREEN = CV_RGB(0, 255, 0);
cv_scalar LIGHTBLUE = CV_RGB(60, 170, 255);
cv_scalar BLUE = CV_RGB(0, 0, 255);
cv_scalar VIOLET = CV_RGB(194, 0, 255);

cv_scalar GINGER = CV_RGB(215, 125, 49);
cv_scalar PINK = CV_RGB(255, 192, 203);
cv_scalar LIGHTGREEN = CV_RGB(153, 255, 153);
cv_scalar BROWN = CV_RGB(150, 75, 0);
/**/

typedef struct cv_color_cluster {
    cv_scalar color;
    cv_scalar new_color;
    int count;

} cv_color_cluster;

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

#endif /* #ifndef KMEANSCOLORER_H */
