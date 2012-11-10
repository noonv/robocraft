/*
 * test tinycv
 */

#include <stdio.h>
#include <stdlib.h>

#include "tinycv.h"

#define PRINT_PERFORMANCE(str, t2, t1) ( printf("[t] %s:\t %u s %d us\n", (str), ((t2).tv_sec-(t1).tv_sec), ((t2).tv_usec-(t1).tv_usec)) )

#define COLORS_COUNT 10

struct timeval t0, t1, t2, t3;

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
	
    image* img = image_create(320, 240, 3, CV_DEPTH_8U);
    image* img_gray = image_create(320, 240, 1, CV_DEPTH_8U);
    image* img_color = image_create(320, 240, 3, CV_DEPTH_8U);
    image* img_color_idx = image_create(320, 240, 1, CV_DEPTH_8U);

    if(!img) {
        printf("[!] Error: image_create()\n");
    }

    int res = 0;
    videocapture_dev dev;

    videocapture_init("/dev/video0", &dev);

    res = videocapture_open(&dev);

    if(res != 0)
        return -1;

    cv_color_cluster clusters2[COLORS_COUNT];
    int colors_count;

    int counter=0;
    while(1) {
        gettimeofday(&t0, NULL);
        image* frame = videocapture_update(&dev);
        gettimeofday(&t1, NULL);
        image_copy(frame, img);
        image_save(img, "frame.bmp");
        print_performance("videocapture_update", t1, t0);

        image_convert_color(img, img_gray, CV_RGB2GRAY);
        image_save(img_gray, "frame_gray.bmp");

        gettimeofday(&t2, NULL);
        colors_count = image_hsv_colorer(img, img_color, img_color_idx, clusters2, COLORS_COUNT);
        gettimeofday(&t3, NULL);
        print_performance("image_hsv_colorer", t3, t2);

        printf("[i] colors count: %d\n", colors_count);
        image_save(img_color, "frame_hsvcolorer.bmp");

        if(counter++>10) {
            break;
        }
    }

    videocapture_close(&dev);

    image_delete(&img);
    image_delete(&img_gray);
    image_delete(&img_color);
    image_delete(&img_color_idx);

    printf("[i] End.\n");
    return 0;
}
