/*
 * test tinycv
 */

#include <stdio.h>
#include <stdlib.h>

#include "tinycv.h"

#define PRINT_PERFORMANCE(str, t2, t1) ( printf("[t] %s:\t %u s %u us\n", (str), ((t2).tv_sec-(t1).tv_sec), ((t2).tv_usec-(t1).tv_usec)) )

struct timeval t0, t1;

int main(int argc, char* argv[])
{
    printf("[i] Start...\n");
	
    image* img = image_create(320, 240, 3, CV_DEPTH_8U);
    image* img_gray = image_create(320, 240, 1, CV_DEPTH_8U);

    if(!img) {
        printf("[!] Error: image_create()\n");
    }

    int res = 0;
    videocapture_dev dev;

    videocapture_init("/dev/video0", &dev);

    res = videocapture_open(&dev);

    if(res != 0)
        return -1;

    int counter=0;
    while(1) {
        gettimeofday(&t0, NULL);
        image* frame = videocapture_update(&dev);
        gettimeofday(&t1, NULL);
        image_copy(frame, img);
        image_save(img, "frame.bmp");
        PRINT_PERFORMANCE("videocapture_update", t1, t0);

        image_convert_color(img, img_gray, CV_RGB2GRAY);
        image_save(img_gray, "frame_gray.bmp");
        if(counter++>5) {
            break;
        }
    }

    videocapture_close(&dev);

    image_delete(&img);
    image_delete(&img_gray);

    printf("[i] End.\n");
    return 0;
}
