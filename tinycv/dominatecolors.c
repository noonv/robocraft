/*
 * TinyCV
 */

#include "dominatecolors.h"
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
        clusters[k].id = k;
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
#if 0
            printf("[i] color: %d (R: %03d G: %03d B: %03d) - %d\n", clusters[k].id,
                   (int)clusters[k].color.val[2],
                   (int)clusters[k].color.val[1],
                   (int)clusters[k].color.val[0],
                   clusters[k].count);
#endif
            colors_count++;
        }
    }

    return colors_count;
}

//-------------

// Various color types
//	    0			1	  2		 3		4		 5		  6		7		8		9			10
//enum {cBLACK=0, cWHITE, cGREY, cRED, cORANGE, cYELLOW, cGREEN, cAQUA, cBLUE, cPURPLE, NUM_COLOR_TYPES};
char*         sCTypes[NUM_COLOR_TYPES] = {"black","white","grey","red","orange","yellow","green","aqua","blue","purple"};
unsigned char cCTHue[NUM_COLOR_TYPES]  = {0,       0,      0,     0,     20,      30,      60,    85,   120,    138  };
unsigned char cCTSat[NUM_COLOR_TYPES]  = {0,       0,      0,    255,   255,     255,     255,   255,   255,    255  };
unsigned char cCTVal[NUM_COLOR_TYPES]  = {0,      255,    120,   255,   255,     255,     255,   255,   255,    255  };

// Determine what type of color the HSV pixel is. Returns the colorType between 0 and NUM_COLOR_TYPES.
int hsv_get_pixel_color_type(int H, int S, int V)
{
    int color = cBLACK;

#if 1
    if (V < 75)
        color = cBLACK;
    else if (V > 190 && S < 27)
        color = cWHITE;
    else if (S < 53 && V < 185)
        color = cGREY;
    else
#endif
    {
        if (H < 7)
            color = cRED;
        else if (H < 25)
            color = cORANGE;
        else if (H < 34)
            color = cYELLOW;
        else if (H < 73)
            color = cGREEN;
        else if (H < 102)
            color = cAQUA;
        else if (H < 140)
            color = cBLUE;
        else if (H < 170)
            color = cPURPLE;
        else	// full circle
            color = cRED;	// back to Red
    }
    return color;
}

// define dominate colors on image by HSV-colors
int image_hsv_colorer(image* src, image* dst, image* cluster_indexes, cv_color_cluster* clusters, int cluster_count)
{
    if(!src || !dst || !cluster_indexes || !clusters || cluster_count != NUM_COLOR_TYPES) {
        return -1;
    }

    if(src->n_channels != 3 || src->type != CV_DEPTH_8U ||
            dst->n_channels != 3 || dst->type != CV_DEPTH_8U ||
            src->width != dst->width || src->height != dst->height) {
        fprintf(stderr, "[!][image_hsv_colorer] Error: bad param!\n");
        return -1;
    }

    // convert image to HSV
    image* hsv = image_create(src->width, src->height, src->n_channels, src->type);
    if(!hsv)
        return -1;
    image_rgb2hsv(src, hsv);

    int i, x, y;

    for(i=0; i<NUM_COLOR_TYPES; i++) {
        clusters[i].id = i;
        clusters[i].color = _cv_scalar(cCTHue[i], cCTSat[i], cCTVal[i], 0);
        clusters[i].new_color = _cv_scalar_all(0);
        clusters[i].count = 0;
    }

    CVTYPE_DEPTH_8U H, S, V;

    for (y=0; y<hsv->height; y++) {
        for (x=0; x<hsv->width; x++) {

            // get HSV
            H = CV_PIXEL(CVTYPE_DEPTH_8U, hsv, x, y)[0];	// Hue
            S = CV_PIXEL(CVTYPE_DEPTH_8U, hsv, x, y)[1];	// Saturation
            V = CV_PIXEL(CVTYPE_DEPTH_8U, hsv, x, y)[2];	// Value (Brightness)

            // define color
            int ctype = hsv_get_pixel_color_type(H, S, V);

            // устанавливаем этот цвет у отладочной картинки
            CV_PIXEL(CVTYPE_DEPTH_8U, dst, x, y)[0] = cCTHue[ctype];	// Hue
            CV_PIXEL(CVTYPE_DEPTH_8U, dst, x, y)[1] = cCTSat[ctype];	// Saturation
            CV_PIXEL(CVTYPE_DEPTH_8U, dst, x, y)[2] = cCTVal[ctype];	// Value

            // collect RGB-values
            clusters[ctype].new_color.val[0] += CV_PIXEL(CVTYPE_DEPTH_8U, src, x, y)[0]; // B
            clusters[ctype].new_color.val[1] += CV_PIXEL(CVTYPE_DEPTH_8U, src, x, y)[1]; // G
            clusters[ctype].new_color.val[2] += CV_PIXEL(CVTYPE_DEPTH_8U, src, x, y)[2]; // R

            // save
            CV_PIXEL(CVTYPE_DEPTH_8U, cluster_indexes, x, y)[0] = ctype;

            // подсчитываем :)
            clusters[ctype].count++;
        }
    }

    for(i=0; i<NUM_COLOR_TYPES; i++) {
        clusters[i].new_color.val[0] /= clusters[i].count;
        clusters[i].new_color.val[1] /= clusters[i].count;
        clusters[i].new_color.val[2] /= clusters[i].count;
    }

    int colors_count = 0;
    for(i=0; i<NUM_COLOR_TYPES; i++) {
        if(clusters[i].count != 0) {
#if 0
            printf("[i] color: %d %s (R: %03d G: %03d B: %03d) - %d\n", clusters[i].id,
                   sCTypes[i],
                   (int)clusters[i].new_color.val[2],
                   (int)clusters[i].new_color.val[1],
                   (int)clusters[i].new_color.val[0],
                   clusters[i].count );
#endif
            colors_count++;
        }
    }

    // convert to RGB
    image_hsv2rgb(dst, hsv);
    image_copy(hsv, dst);

    image_delete(&hsv);

    return colors_count;
}

// for sort via qsort()
static int sort_cv_color_cluster(const void *p1, const void *p2)
{
    return ( ((cv_color_cluster *)p2)->count - ((cv_color_cluster *)p1)->count );
}

int sort_color_clusters_by_count(cv_color_cluster* clusters, int cluster_count)
{
    if(!clusters || cluster_count <= 0)
        return -1;

    qsort(clusters, cluster_count, sizeof(cv_color_cluster), sort_cv_color_cluster);

    return 0;
}

void print_color_clusters(cv_color_cluster* clusters, int cluster_count)
{
    if(!clusters || cluster_count <= 0)
        return;

    int i;
    for(i=0; i<cluster_count; i++) {
        printf("[i] color: %d (R: %03d G: %03d B: %03d) - %d\n", clusters[i].id,
               (int)clusters[i].new_color.val[2],
               (int)clusters[i].new_color.val[1],
               (int)clusters[i].new_color.val[0],
               clusters[i].count );
    }
}

cv_point get_color_center(int color_index, image* cluster_indexes)
{
    cv_point res = _cv_point(-1, -1);
    if(!cluster_indexes || color_index < 0)
        return res;

    int x,y;
    float xc=0, yc=0;
    int count=0;
    for (y=0; y<cluster_indexes->height; y++) {
        for (x=0; x<cluster_indexes->width; x++) {
            if(CV_PIXEL(CVTYPE_DEPTH_8U, cluster_indexes, x, y)[0] == color_index) {
                xc += x;
                yc += y;
                count++;
            }
        }
    }
    xc /= count;
    yc /= count;

    res.x = (int)xc;
    res.y = (int)yc;

    return res;
}
