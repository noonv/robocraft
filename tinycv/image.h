/*
 * TinyCV
 */

#ifndef IMAGE_H
#define IMAGE_H

/* image types */
//enum {CV_8SC1=0, CV_8SC3, CV_8UC1, CV_8UC3, CV_16SC1, CV_16SC3, CV_16UC1, CV_16UC3, CV_32FC1, CV_32FC3};
enum {CV_DEPTH_8S=0, CV_DEPTH_8U, CV_DEPTH_16S, CV_DEPTH_16U, CV_DEPTH_32F};

/* simple image */

typedef struct image {
	int width;
	int height;
	int type;
	int n_channels;
	int size;
	int step;
	
	char* data;
} image;

typedef char			CVTYPE_DEPTH_8S;
typedef unsigned char	CVTYPE_DEPTH_8U;
typedef short			CVTYPE_DEPTH_16S;
typedef unsigned short	CVTYPE_DEPTH_16U;
typedef float			CVTYPE_DEPTH_32F;

#ifndef UCHAR_MAX
# define UCHAR_MAX 0xFF
#endif

#define CV_PIXEL(type,img,x,y) ( ( (type*)(((img)->data)+(y)*(img)->step) ) + (x)*(img)->n_channels )

image* image_create(int width, int height, int n_channels, int type);

int image_delete(image** img);

image* image_load(const char* filename);

int image_save(image* img, const char* filename);

int image_rgb2gray(image* src, image* dst);
int image_gray2rgb(image* src, image* dst);
int image_rgb2hsv(image* src, image* dst);
int image_hsv2rgb(image* src, image* dst);

enum {CV_RGB2GRAY=0};

int image_convert_color(image* src, image* dst, int type);

image* image_clone(image* src);
int image_copy(image* src, image* dst);

void image_rotate180(image *src);
void image_reflect_vertical(image *src);

#endif /* #ifndef IMAGE_H */
