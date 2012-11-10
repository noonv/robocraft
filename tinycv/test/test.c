/*
 * test tinycv
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> // gettimeofday()

#include "tinycv.h"

#define PRINT_PERFORMANCE(str, t2, t1) ( printf("[t] %s:\t %u s %d us\n", (str), ((t2).tv_sec-(t1).tv_sec), ((t2).tv_usec-(t1).tv_usec)) )

struct timeval t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;

void print_performance(const char* str, struct timeval t2, struct timeval t1)
{
    if(!str)
        return;

    double sec1 = t1.tv_sec*1000000.0 + t1.tv_usec;
    double sec2 = t2.tv_sec*1000000.0 + t2.tv_usec;

    double delta = (sec2-sec1)/1000000.0;

//    PRINT_PERFORMANCE(str, t2, t1);
    printf("[t] %s:\t %02.6f s\n", str, delta);
}

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
    image_save(img2, "test2_load_save.bmp");

    printf("[i] == Tests == \n");
#if 1
    // test convert image to grayscale
    printf("[i] image_convert_color \n");
    image* img_gray = image_create(320, 240, 1, CV_DEPTH_8U);
    gettimeofday(&t0, NULL);
    image_convert_color(img2, img_gray, CV_RGB2GRAY);
    gettimeofday(&t1, NULL);
    image_save(img_gray, "test3_gray.bmp");

    // test borders detection
    printf("[i] image_thin_borders \n");
    image* img_borders = NULL;
    gettimeofday(&t2, NULL);
    image_thin_borders(img_gray, &img_borders);
    gettimeofday(&t3, NULL);
    image_save(img_borders, "test4_thin_borders.bmp");

    // min-max-loc
    printf("[i] image_min_max_loc \n");
    double _min, _max;
    gettimeofday(&t4, NULL);
    image_min_max_loc(img_gray, &_min, &_max, NULL, NULL);
    gettimeofday(&t5, NULL);
    printf("[i] min=%0.2f max=%0.2f\n", _min, _max);

    // threshold
    printf("[i] image_threshold \n");
    image* img_thr = image_create(320, 240, 1, CV_DEPTH_8U);
    gettimeofday(&t6, NULL);
    image_threshold(img_gray, img_thr, 60);
    gettimeofday(&t7, NULL);
    image_save(img_thr, "test5_threshold.bmp");
#endif

#if 1
    // rotate180
    printf("[i] image_rotate180 \n");
    image_rotate180(img2);
    image_save(img2, "test6_rotate180.bmp");
    image_rotate180(img2);
    // reflect vertical
    printf("[i] image_reflect_vertical \n");
    image_reflect_vertical(img2);
    image_save(img2, "test7_reflect_vertical.bmp");
    image_reflect_vertical(img2);
#endif // rotate180

    int colors_count;

#if 1
    // simple resize
    printf("[i] image_resize \n");
    image *img_small = image_create(160, 120, 3, CV_DEPTH_8U);
    gettimeofday(&t8, NULL);
    image_resize(img2, img_small);
    gettimeofday(&t9, NULL);
    image_save(img_small, "test8_resize.bmp");

 //   image* img_small_gray = image_create(80, 60, 1, CV_DEPTH_8U);
//    image_convert_color(img_small, img_small_gray, CV_RGB2GRAY);

//    image* img_small_borders = NULL;
//    image_thin_borders(img_small_gray, &img_small_borders);
//    image_save(img_small_borders, "test_resize_thin_borders.bmp");

#if 1
    // k-meanes colorer
    printf("[i] image_kmeans_colorer \n");
    image* img_kmeanes = image_create(160, 120, 3, CV_DEPTH_8U);
    image* img_kmeanes_idx = image_create(160, 120, 1, CV_DEPTH_8U);

#define CLUSTER_COUNT 10
    int cluster_count = CLUSTER_COUNT;
    cv_color_cluster clusters[CLUSTER_COUNT];

    gettimeofday(&t10, NULL);
    colors_count = image_kmeans_colorer(img_small, img_kmeanes, img_kmeanes_idx, clusters, cluster_count);
    gettimeofday(&t11, NULL);
    printf("[i] colors count: %d\n", colors_count);

    image_save(img_kmeanes, "test_kmeanscolorer.bmp");

    image_delete(&img_kmeanes);
    image_delete(&img_kmeanes_idx);
#endif // k-meanes colorer

    image_delete(&img_small);
//    image_delete(&img_small_gray);
//    image_delete(&img_small_borders);
#endif // simple resize

#if 1
    // HSV
    printf("[i] image_hsv2rgb \n");
    image* img_hsv = image_create(320, 240, 3, CV_DEPTH_8U);
    image* img_bgr = image_create(320, 240, 3, CV_DEPTH_8U);

    gettimeofday(&t12, NULL);
    image_rgb2hsv(img2, img_hsv);
    gettimeofday(&t13, NULL);

    image_hsv2rgb(img_hsv, img_bgr);

    image_save(img_hsv, "test9_rgb2hsv.bmp");
    image_save(img_bgr, "test9_hsv2rgb.bmp");

    image_delete(&img_hsv);
    image_delete(&img_bgr);
#endif // HSV

#if 1
    // hsv colorer
    printf("[i] image_hsv_colorer \n");
    image* img_hsv_col = image_create(320, 240, 3, CV_DEPTH_8U);
    image* img_hsv_idx = image_create(320, 240, 1, CV_DEPTH_8U);

#define COLORS_COUNT 10
    cv_color_cluster clusters2[COLORS_COUNT];

    gettimeofday(&t14, NULL);
    colors_count = image_hsv_colorer(img2, img_hsv_col, img_hsv_idx, clusters2, COLORS_COUNT);
    gettimeofday(&t15, NULL);

    printf("[i] colors count: %d\n", colors_count);
    image_save(img_hsv_col, "test_hsvcolorer.bmp");

    image_delete(&img_hsv_col);
    image_delete(&img_hsv_idx);
#endif // k-meanes colorer

    printf("[i] == Performance == \n");
    print_performance("image_convert_color", t1, t0);
    print_performance("image_thin_borders", t3, t2);
    print_performance("image_min_max_loc", t5, t4);
    print_performance("image_threshold", t7, t6);
    print_performance("image_resize", t9, t8);
    print_performance("image_kmeans_colorer", t11, t10);
    print_performance("image_rgb2hsv", t13, t12);
    print_performance("image_hsv_colorer", t15, t14);

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
