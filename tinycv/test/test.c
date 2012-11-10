/*
 * test tinycv
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> // gettimeofday()

#include "tinycv.h"

#define PRINT_PERFORMANCE(str, t2, t1) ( printf("[t] %s:\t %u s %d us\n", (str), ((t2).tv_sec-(t1).tv_sec), ((t2).tv_usec-(t1).tv_usec)) )

struct timeval t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11;

int main(int argc, char* argv[])
{
    printf("[i] Start...\n");

    char file_name[] = "test.bmp";
    char* filename=file_name;

    if(argc >= 2) {
        filename = argv[1];
    }

    printf("[i] file: %s\n", filename);
	
    image* img = image_create(320, 240, 3, CV_DEPTH_8U);

    if(!img) {
        printf("[!] Error: image_create()\n");
    }

    printf("[i] image size: %dx%dx%d (%d)\n", img->width, img->height, img->n_channels, img->size);

    // test image loading
    image* img2 = image_load(filename);
    image_save(img2, "test2.bmp");

    printf("[i] == Tests == \n");
#if 1
    // test convert image to grayscale
    image* img_gray = image_create(320, 240, 1, CV_DEPTH_8U);
    gettimeofday(&t0, NULL);
    image_convert_color(img2, img_gray, CV_RGB2GRAY);
    gettimeofday(&t1, NULL);
    image_save(img_gray, "test3.bmp");

    // test borders detection
    image* img_borders = NULL;
    gettimeofday(&t2, NULL);
    image_thin_borders(img_gray, &img_borders);
    gettimeofday(&t3, NULL);
    image_save(img_borders, "test4.bmp");

    // min-max-loc
    double _min, _max;
    gettimeofday(&t4, NULL);
    image_min_max_loc(img_gray, &_min, &_max, NULL, NULL);
    gettimeofday(&t5, NULL);
    printf("[i] min=%0.2f max=%0.2f\n", _min, _max);

    // threshold
    image* img_thr = image_create(320, 240, 1, CV_DEPTH_8U);
    gettimeofday(&t6, NULL);
    image_threshold(img_gray, img_thr, 60);
    gettimeofday(&t7, NULL);
    image_save(img_thr, "test5.bmp");
#endif

#if 1
    // rotate180
    image_rotate180(img2);
    image_save(img2, "test_rotate180.bmp");
    image_rotate180(img2);
    // reflect vertical
    image_reflect_vertical(img2);
    image_save(img2, "test_reflect_vertical.bmp");
    image_reflect_vertical(img2);
#endif

#if 1
    // simple resize
    image *img_small = image_create(160, 120, 3, CV_DEPTH_8U);
    gettimeofday(&t8, NULL);
    image_resize(img2, img_small);
    gettimeofday(&t9, NULL);
    image_save(img_small, "test_resize.bmp");

 //   image* img_small_gray = image_create(80, 60, 1, CV_DEPTH_8U);
//    image_convert_color(img_small, img_small_gray, CV_RGB2GRAY);

//    image* img_small_borders = NULL;
//    image_thin_borders(img_small_gray, &img_small_borders);
//    image_save(img_small_borders, "test_resize_thin_borders.bmp");

#if 1
    // k-meanes colorer
    image* img_kmeanes = image_create(160, 120, 3, CV_DEPTH_8U);
    image* img_kmeanes_idx = image_create(160, 120, 1, CV_DEPTH_8U);

#define CLUSTER_COUNT 10
    int cluster_count = CLUSTER_COUNT;
    cv_color_cluster clusters[CLUSTER_COUNT];

    gettimeofday(&t10, NULL);
    int colors_count = image_kmeans_colorer(img_small, img_kmeanes, img_kmeanes_idx, clusters, cluster_count);
    gettimeofday(&t11, NULL);
    printf("[i] colors count: %d\n", colors_count);

    image_save(img_kmeanes, "test_kmeanscolorer.bmp");

    image_delete(&img_kmeanes);
    image_delete(&img_kmeanes_idx);
#endif

    image_delete(&img_small);
//    image_delete(&img_small_gray);
//    image_delete(&img_small_borders);
#endif

    printf("[i] == Performance == \n");
    PRINT_PERFORMANCE("image_convert_color", t1, t0);
    PRINT_PERFORMANCE("image_thin_borders", t3, t2);
    PRINT_PERFORMANCE("image_min_max_loc", t5, t4);
    PRINT_PERFORMANCE("image_threshold", t7, t6);
    PRINT_PERFORMANCE("image_resize", t9, t8);
    PRINT_PERFORMANCE("image_kmeans_colorer", t11, t10);

    image_delete(&img);    
    image_delete(&img2);
#if 1
    image_delete(&img_gray);
    image_delete(&img_borders);
    image_delete(&img_thr);
#endif

    printf("[i] End.\n");
    return 0;
}
