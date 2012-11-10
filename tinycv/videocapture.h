/*
 * TinyCV
 */

#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include "image.h"

/*
*  based on V4L2 video capture example
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

typedef enum {
    IO_METHOD_READ,
    IO_METHOD_MMAP
} io_method;

struct buffer {
    void *                  start;
    size_t                  length;
};

typedef struct videocapture_dev {
    char *          dev_name;
    io_method       io;
    int             fd;
    struct buffer * buffers;
    unsigned int    n_buffers;

    int width;
    int height;
    int fps;
    int pixelformat;

    image* img;
    int frame_counter;

} videocapture_dev;

// init params for video capture
void videocapture_init(const char* name, videocapture_dev* dev);
// open video device and start capture
int videocapture_open(videocapture_dev* dev);
// stop capture and close video device
int videocapture_close(videocapture_dev* dev);
// grab next frame
image* videocapture_update(videocapture_dev* dev);

//
// internal functions
//
void videocapture_uninit(videocapture_dev* dev);

int videocapture_open_device (videocapture_dev* dev);
void videocapture_close_device(videocapture_dev* dev);

void videocapture_init_device(videocapture_dev* dev);
void videocapture_uninit_device(videocapture_dev* dev);

void videocapture_start_capturing(videocapture_dev* dev);
void videocapture_stop_capturing(videocapture_dev* dev);

void yuyv_to_image(unsigned char* data, image *img);

#endif /* #ifndef VIDEOCAPTURE_H */
