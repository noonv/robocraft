/*
 * TinyCV
 */

#include "videocapture.h"

static void errno_exit(const char* s)
{
    fprintf (stderr, "[!] Error: %s errno: %d, %s\n",
             s, errno, strerror (errno));

    exit (EXIT_FAILURE);
}

static int xioctl(int fd, int request, void* arg)
{
    int r;

    do r = ioctl (fd, request, arg);
    while (-1 == r && EINTR == errno);

    return r;
}

static void init_read(videocapture_dev* dev, unsigned int buffer_size)
{
    if(!dev) return;

    printf("[i] init read...\n");
    dev->buffers = (struct buffer *) calloc (1, sizeof(*(dev->buffers)));

    if (!dev->buffers) {
        fprintf (stderr, "Out of memory\n");
        exit (EXIT_FAILURE);
    }

    dev->buffers[0].length = buffer_size;
    dev->buffers[0].start = malloc (buffer_size);

    if (!dev->buffers[0].start) {
        fprintf (stderr, "Out of memory\n");
        exit (EXIT_FAILURE);
    }
}

static void init_mmap(videocapture_dev* dev)
{
    if(!dev) return;

    struct v4l2_requestbuffers req;

    CLEAR (req);

    req.count               = 4;
    req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory              = V4L2_MEMORY_MMAP;

    if (-1 == xioctl (dev->fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf (stderr, "%s does not support "
                     "memory mapping\n", dev->dev_name);
            exit (EXIT_FAILURE);
        } else {
            errno_exit ("VIDIOC_REQBUFS");
        }
    }

    if (req.count < 2) {
        fprintf (stderr, "Insufficient buffer memory on %s\n",
                 dev->dev_name);
        exit (EXIT_FAILURE);
    }

    dev->buffers = (struct buffer *) calloc (req.count, sizeof (*(dev->buffers)));

    if (!dev->buffers) {
        fprintf (stderr, "Out of memory\n");
        exit (EXIT_FAILURE);
    }

    for (dev->n_buffers = 0; dev->n_buffers < req.count; ++dev->n_buffers) {
        struct v4l2_buffer buf;

        CLEAR (buf);

        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = dev->n_buffers;

        if (-1 == xioctl (dev->fd, VIDIOC_QUERYBUF, &buf))
            errno_exit ("VIDIOC_QUERYBUF");

        dev->buffers[dev->n_buffers].length = buf.length;
        dev->buffers[dev->n_buffers].start =
                mmap (NULL /* start anywhere */,
                      buf.length,
                      PROT_READ | PROT_WRITE /* required */,
                      MAP_SHARED /* recommended */,
                      dev->fd, buf.m.offset);

        if (MAP_FAILED == dev->buffers[dev->n_buffers].start)
            errno_exit ("mmap");
    }
}

void videocapture_init(const char* name, videocapture_dev* dev)
{
    if(!name || !dev)
        return;

    dev->dev_name = name;
    dev->io = IO_METHOD_MMAP;
    dev->fd = -1;
    dev->buffers = NULL;
    dev->n_buffers = 0;

    if(!dev->width)
        dev->width = 320;
    if(!dev->height)
        dev->height = 240;
    dev->fps = 10;
    dev->pixelformat = V4L2_PIX_FMT_YUYV;
    //dev->pixelformat = V4L2_PIX_FMT_BGR24;

    dev->img = image_create(dev->width, dev->height, 3, CV_DEPTH_8U);
    dev->frame_counter = 0;
}

void videocapture_uninit(videocapture_dev* dev)
{
    if(!dev) return;

    image_delete(&(dev->img));
}

int videocapture_open_device (videocapture_dev* dev)
{
    if(!dev)
        return -1;

    if(!dev->dev_name){
        fprintf(stderr, "[!] Error: bad dev name!\n");
        return -1;
    }

    struct stat st;

    if (-1 == stat (dev->dev_name, &st)) {
        fprintf (stderr, "[!] Error: Cannot identify '%s': %d, %s\n",
                 dev->dev_name, errno, strerror (errno));
        return -1;
    }

    if (!S_ISCHR (st.st_mode)) {
        fprintf (stderr, "[!] Error: %s is no device\n", dev->dev_name);
        return -1;
    }

    dev->fd = open (dev->dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

    if (-1 == dev->fd) {
        fprintf (stderr, "[!] Error: Cannot open '%s': %d, %s\n",
                 dev->dev_name, errno, strerror (errno));
        return -1;
    }

    return 0;
}

void videocapture_close_device(videocapture_dev* dev)
{
    if(!dev) return;
    if(dev->fd<0) return;

    if (-1 == close (dev->fd))
        errno_exit ("close");

    dev->fd = -1;
}

void videocapture_init_device(videocapture_dev* dev)
{
    if(!dev) return;

    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    unsigned int min;

    int fd = dev->fd;

    if (-1 == xioctl (fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf (stderr, "%s is no V4L2 device\n",
                     dev->dev_name);
            return;
        } else {
            errno_exit ("VIDIOC_QUERYCAP");
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf (stderr, "%s is no video capture device\n",
                 dev->dev_name);
        return;
    }

    switch (dev->io)
    {
    case IO_METHOD_READ:
        if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
            fprintf (stderr, "%s does not support read i/o\n",
                     dev->dev_name);
            exit (EXIT_FAILURE);
        }

        break;

    case IO_METHOD_MMAP:
        if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
            fprintf (stderr, "%s does not support streaming i/o\n",
                     dev->dev_name);
            exit (EXIT_FAILURE);
        }

        break;
    }


    /* Select video input, video standard and tune here. */


    CLEAR (cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl (fd, VIDIOC_CROPCAP, &cropcap)) {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        if (-1 == xioctl (fd, VIDIOC_S_CROP, &crop)) {
            switch (errno)
            {
            case EINVAL:
                /* Cropping not supported. */
                break;
            default:
                /* Errors ignored. */
                break;
            }
        }
    } else {
        /* Errors ignored. */
    }


    CLEAR (fmt);

    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = dev->width;
    fmt.fmt.pix.height      = dev->height;
    fmt.fmt.pix.pixelformat = dev->pixelformat; // V4L2_PIX_FMT_BGR24 V4L2_PIX_FMT_YUYV
    fmt.fmt.pix.field       = V4L2_FIELD_ANY; //_INTERLACED;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

    if (-1 == xioctl (fd, VIDIOC_S_FMT, &fmt))
        errno_exit ("VIDIOC_S_FMT");

    /**/
#if 0
    // get available FPS
    struct v4l2_frmivalenum frm;
    CLEAR(frm);
    frm.index=0;
    frm.pixel_format = fmt.fmt.pix.pixelformat;
    frm.width = fmt.fmt.pix.width;
    frm.height = fmt.fmt.pix.height;
    if(-1 != xioctl (fd, VIDIOC_ENUM_FRAMEINTERVALS, &frm)){
        printf("[i] Frame interval (s): type=%d discrete: %d - %d\n",
               frm.type, frm.discrete.numerator, frm.discrete.denominator);
        printf("[i] Frame interval (s): min: %d - %d max: %d - %d step %d - %d\n",
               frm.stepwise.min.numerator, frm.stepwise.min.denominator,
               frm.stepwise.max.numerator, frm.stepwise.max.denominator,
               frm.stepwise.step.numerator, frm.stepwise.step.denominator);
    }
    else{
        printf("[!] Error: VIDIOC_ENUM_FRAMEINTERVALS\n");
    }
#endif

    // set FPS
    struct v4l2_streamparm setfps;
    CLEAR(setfps);
    setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    setfps.parm.capture.timeperframe.numerator=1;
    setfps.parm.capture.timeperframe.denominator=dev->fps;
//    setfps.parm.output.timeperframe.numerator=1;
//    setfps.parm.output.timeperframe.denominator=dev->fps;
    xioctl(fd, VIDIOC_S_PARM, &setfps);
    /**/

#if 0
    //standard
    v4l2_std_id std_id = 0;
    xioctl(fd, VIDIOC_G_STD, &std_id);
    printf("std_id=%d\n", std_id);
    /**/
    struct v4l2_input inp;
    CLEAR(inp);
    inp.index=1;
    inp.type = V4L2_INPUT_TYPE_CAMERA;
    inp.std = (std_id & V4L2_STD_PAL);
    if(-1==xioctl(fd, VIDIOC_ENUMINPUT, &inp)){
        errno_exit("VIDIOC_ENUMINPUT");
    }
#endif

#if 0
    //controls
    struct v4l2_control control;
    CLEAR(control);
    control.id = V4L2_CID_EXPOSURE_AUTO;
    control.value=0;
    if(-1==xioctl(fd, VIDIOC_S_CTRL, &control)){
        errno_exit("VIDIOC_S_CTL");
    }
#endif
    char fourcc[5] = {0, 0, 0, 0, 0};
    memmove(fourcc, &fmt.fmt.pix.pixelformat, 4);
    printf("capture:\n size: %d x %d\n format: %4s\n framesize: %d\n",
           fmt.fmt.pix.width,
           fmt.fmt.pix.height,
           fourcc, //fmt.fmt.pix.pixelformat,
           fmt.fmt.pix.sizeimage);
    /**/

    /* Note VIDIOC_S_FMT may change width and height. */

    /* Buggy driver paranoia. */
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min)
        fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
        fmt.fmt.pix.sizeimage = min;

    switch (dev->io)
    {
    case IO_METHOD_READ:
        init_read (dev, fmt.fmt.pix.sizeimage);
        break;

    case IO_METHOD_MMAP:
        init_mmap (dev);
        break;

    }
}

void videocapture_uninit_device(videocapture_dev* dev)
{
    if(!dev) return;

    unsigned int i;

    switch (dev->io)
    {
    case IO_METHOD_READ:
        free (dev->buffers[0].start);
        break;

    case IO_METHOD_MMAP:
        for (i = 0; i < dev->n_buffers; ++i)
            if (-1 == munmap (dev->buffers[i].start, dev->buffers[i].length))
                errno_exit ("munmap");
        break;
    }

    free (dev->buffers);
}

void videocapture_start_capturing(videocapture_dev* dev)
{
    if(!dev) return;

    unsigned int i;
    enum v4l2_buf_type type;

    switch (dev->io)
    {
    case IO_METHOD_READ:
        /* Nothing to do. */
        break;

    case IO_METHOD_MMAP:
        for (i = 0; i < dev->n_buffers; ++i) {
            struct v4l2_buffer buf;

            CLEAR (buf);

            buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory      = V4L2_MEMORY_MMAP;
            buf.index       = i;

            if (-1 == xioctl (dev->fd, VIDIOC_QBUF, &buf))
                errno_exit ("VIDIOC_QBUF");
        }

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (-1 == xioctl (dev->fd, VIDIOC_STREAMON, &type))
            errno_exit ("VIDIOC_STREAMON");

        break;
    }
}

void videocapture_stop_capturing(videocapture_dev* dev)
{
    if(!dev) return;
    if(dev->fd<0) return;

    enum v4l2_buf_type type;

    switch (dev->io)
    {
    case IO_METHOD_READ:
        /* Nothing to do. */
        break;

    case IO_METHOD_MMAP:
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (-1 == xioctl (dev->fd, VIDIOC_STREAMOFF, &type))
            errno_exit ("VIDIOC_STREAMOFF");
        break;
    }
}

void process_image(videocapture_dev* dev, const void* p)
{
    if(!dev || !p)
        return;

//    fputc ('.', stdout);
//    fflush (stdout);
#if 1
    if(dev->pixelformat == V4L2_PIX_FMT_BGR24) {
        memcpy(dev->img->data, p, dev->img->size);
    }
    else if(dev->pixelformat == V4L2_PIX_FMT_YUYV) {
        yuyv_to_image((unsigned char*)p, dev->img);
        image_reflect_vertical(dev->img);
    }

    dev->frame_counter++;
#endif
}

int read_frame(videocapture_dev* dev)
{
    if(!dev)
        return -1;

    struct v4l2_buffer buf;
    unsigned int i;
    int res = 0;

    switch (dev->io)
    {
    case IO_METHOD_READ:
        res = read (dev->fd, dev->buffers[0].start, dev->buffers[0].length);
        if(-1 == res) {
            switch (errno)
            {
            case EAGAIN:
                return 0;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                errno_exit ("read");
            }
        }
        printf("read %d\n", res);
        process_image (dev, dev->buffers[0].start);

        break;

    case IO_METHOD_MMAP:
        CLEAR (buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl (dev->fd, VIDIOC_DQBUF, &buf)) {
            switch (errno)
            {
            case EAGAIN:
                return 0;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                errno_exit ("VIDIOC_DQBUF");
            }
        }

        assert (buf.index < dev->n_buffers);

        process_image (dev, dev->buffers[buf.index].start);

        if (-1 == xioctl (dev->fd, VIDIOC_QBUF, &buf))
            errno_exit ("VIDIOC_QBUF");

        break;
    }

    return 1;
}

image* videocapture_update(videocapture_dev* dev)
{
    for (;;) {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO (&fds);
        FD_SET (dev->fd, &fds);

        // Timeout.
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        r = select (dev->fd + 1, &fds, NULL, NULL, &tv);
        if (-1 == r) {
            if (EINTR == errno)
                continue;

            errno_exit ("select");
        }

        if (0 == r) {
            fprintf (stderr, "select timeout\n");
            exit (EXIT_FAILURE);
        }

        read_frame(dev);
        return dev->img;

        // EAGAIN - continue select loop.
    }
}

void yuyv_to_image(unsigned char* data, image *img)
{
    if(!data || !img) {
        return;
    }

    unsigned char *l_=(unsigned char *)img->data;

    int w2 = img->width/2;
    int height = img->height;

    int x, y;
    for(x=0; x<w2; x++) {
        for(y=0; y<height; y++) {
            int y0, y1, u, v; //y0 u y1 v

            int i=(y*w2+x)*4;
            y0=data[i];
            u=data[i+1];
            y1=data[i+2];
            v=data[i+3];

            int r, g, b;
            r = y0 + (1.370705 * (v-128));
            g = y0 - (0.698001 * (v-128)) - (0.337633 * (u-128));
            b = y0 + (1.732446 * (u-128));

            if(r > 255) r = 255;
            if(g > 255) g = 255;
            if(b > 255) b = 255;
            if(r < 0) r = 0;
            if(g < 0) g = 0;
            if(b < 0) b = 0;

            i=(y*img->width+2*x)*3;
            l_[i] = (unsigned char)(b); //B
            l_[i+1] = (unsigned char)(g); //G
            l_[i+2] = (unsigned char)(r); //R


            r = y1 + (1.370705 * (v-128));
            g = y1 - (0.698001 * (v-128)) - (0.337633 * (u-128));
            b = y1 + (1.732446 * (u-128));

            if(r > 255) r = 255;
            if(g > 255) g = 255;
            if(b > 255) b = 255;
            if(r < 0) r = 0;
            if(g < 0) g = 0;
            if(b < 0) b = 0;

            l_[i+3] = (unsigned char)(b); //B
            l_[i+4] = (unsigned char)(g); //G
            l_[i+5] = (unsigned char)(r); //R
        }
    }
}

int videocapture_open(videocapture_dev* dev)
{
    if(!dev) {
        return -1;
    }

    int res = -1;

    res = videocapture_open_device(dev);
    if(res != 0) {
        return -1;
    }

    videocapture_init_device(dev);

    videocapture_start_capturing (dev);

    return 0;
}

int videocapture_close(videocapture_dev* dev)
{
    if(!dev) {
        return -1;
    }

    videocapture_stop_capturing(dev);

    videocapture_uninit_device(dev);
    videocapture_close_device(dev);
    videocapture_uninit(dev);

    return 0;
}
