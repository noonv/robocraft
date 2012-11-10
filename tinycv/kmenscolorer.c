/*
 * TinyCV
 */

#include "kmenscolorer.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

float rgb_euclidean(cv_scalar p1, cv_scalar p2)
{
    float val = sqrtf( (p1.val[0]-p2.val[0])*(p1.val[0]-p2.val[0]) +
                       (p1.val[1]-p2.val[1])*(p1.val[1]-p2.val[1]) +
                       (p1.val[2]-p2.val[2])*(p1.val[2]-p2.val[2]) +
                       (p1.val[3]-p2.val[3])*(p1.val[3]-p2.val[3]));

    return val;
}

// define dominate colors on image by k-meanes algorithm
int image_kmeans_colorer(image* src, image* dst, image* cluster_indexes, cv_color_cluster* clusters, int cluster_count)
{
    if(!src || !dst || !cluster_indexes || !clusters || cluster_count<=0) {
        return -1;
    }

    if(src->n_channels != 3 || src->type != CV_DEPTH_8U ||
            dst->n_channels != 3 || dst->type != CV_DEPTH_8U ||
            src->width != dst->width || src->height != dst->height) {
        fprintf(stderr, "[!][kmeans_colorer] Error: bad param!\n");
        return -1;
    }


    int i=0, j=0, k=0, x=0, y=0;

    time_t t;
    srandom(time(&t));

    // first cluster colors
    for(k=0; k<cluster_count; k++) {
        clusters[k].new_color = CV_RGB(rand()%255, rand()%255, rand()%255);
    }

    // k-means
    float min_rgb_euclidean = 0, old_rgb_euclidean=0;

    while(1) {
        for(k=0; k<cluster_count; k++) {
            clusters[k].count = 0;
            clusters[k].color = clusters[k].new_color;
            clusters[k].new_color = _cv_scalar_all(0);
        }

        for (y=0; y<src->height; y++) {
            for (x=0; x<src->width; x++) {
                // get RGB
                CVTYPE_DEPTH_8U B = CV_PIXEL(CVTYPE_DEPTH_8U, src, x, y)[0];	// B
                CVTYPE_DEPTH_8U G = CV_PIXEL(CVTYPE_DEPTH_8U, src, x, y)[1];	// G
                CVTYPE_DEPTH_8U R = CV_PIXEL(CVTYPE_DEPTH_8U, src, x, y)[2];	// R

                min_rgb_euclidean = 255*255*255;
                int cluster_index = -1;
                for(k=0; k<cluster_count; k++) {
                    float euclid = rgb_euclidean(_cv_scalar(B, G, R, 0), clusters[k].color);
                    if(  euclid < min_rgb_euclidean ) {
                        min_rgb_euclidean = euclid;
                        cluster_index = k;
                    }
                }
                // set cluster index
                CV_PIXEL(CVTYPE_DEPTH_8U, cluster_indexes, x, y)[0] = cluster_index;

                clusters[cluster_index].count++;
                clusters[cluster_index].new_color.val[0] += B;
                clusters[cluster_index].new_color.val[1] += G;
                clusters[cluster_index].new_color.val[2] += R;
            }
        }

        min_rgb_euclidean = 0;
        for(k=0; k<cluster_count; k++) {
            // new color
            clusters[k].new_color.val[0] /= clusters[k].count;
            clusters[k].new_color.val[1] /= clusters[k].count;
            clusters[k].new_color.val[2] /= clusters[k].count;
            float ecli = rgb_euclidean(clusters[k].new_color, clusters[k].color);
            if(ecli > min_rgb_euclidean)
                min_rgb_euclidean = ecli;
        }

        //printf("[i][kmeans_colorer] %f\n", min_rgb_euclidean);
        if( fabs(min_rgb_euclidean - old_rgb_euclidean)<1 )
            break;

        old_rgb_euclidean = min_rgb_euclidean;
    }
    //-----------------------------------------------------

    // create picture in cluster colors
    for (y=0; y<dst->height; y++) {
        for (x=0; x<dst->width; x++) {
            int cluster_index = CV_PIXEL(CVTYPE_DEPTH_8U, cluster_indexes, x, y)[0];

            CV_PIXEL(CVTYPE_DEPTH_8U, dst, x, y)[0] = clusters[cluster_index].color.val[0];
            CV_PIXEL(CVTYPE_DEPTH_8U, dst, x, y)[1] = clusters[cluster_index].color.val[1];
            CV_PIXEL(CVTYPE_DEPTH_8U, dst, x, y)[2] = clusters[cluster_index].color.val[2];
        }
    }

    int colors_count = 0;
    // show colors
    for(k=0; k<cluster_count; k++) {
        if(clusters[k].count != 0) {
            printf("[i] Color %d: R: %03d G: %03d B: %03d (%d)\n", k, (int)clusters[k].color.val[2],
                   (int)clusters[k].color.val[1],
                   (int)clusters[k].color.val[0],
                   clusters[k].count);
            colors_count++;
        }
    }

    return colors_count;
}
