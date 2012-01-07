//
// OpenCV benchmark
//
//
// Author: 	Vladimir  aka noonv
// email: 	noonv13@gmail.com
//
// http://robocraft.ru
//

#include <stdio.h>
#include <stdlib.h>

#include <cv.h>
#include <highgui.h>

#ifdef _MSC_VER
# if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION == 0
// OpenCV 2.0
#  pragma comment(lib, "cxcore200.lib")
#  pragma comment(lib, "cv200.lib")
#  pragma comment(lib, "highgui200.lib")
#  pragma comment(lib, "ml200.lib")

#  pragma comment(lib, "cvaux200.lib")

# elif CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION == 1
// OpenCV 2.1
#  ifdef _DEBUG
// отладочная версия библиотек
#   pragma comment(lib, "cxcore210d.lib")
#   pragma comment(lib, "cv210d.lib")
#   pragma comment(lib, "highgui210d.lib")
#   pragma comment(lib, "ml210d.lib")

#   pragma comment(lib, "cvaux210d.lib")
#  else
#   pragma comment(lib, "cxcore210.lib")
#   pragma comment(lib, "cv210.lib")
#   pragma comment(lib, "highgui210.lib")
#   pragma comment(lib, "ml210.lib")

//#   pragma comment(lib, "opencv_ffmpeg210.lib")

#   pragma comment(lib, "cvaux210.lib")
#  endif

# elif CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION == 2
// OpenCV 2.2
#  pragma comment(lib, "opencv_core220.lib")
#  pragma comment(lib, "opencv_highgui220.lib")
#  pragma comment(lib, "opencv_imgproc220.lib")
#  pragma comment(lib, "opencv_video220.lib")
#  pragma comment(lib, "opencv_ml220.lib")

#  pragma comment(lib, "opencv_objdetect220.lib")
#  pragma comment(lib, "opencv_features2d220.lib")
#  pragma comment(lib, "opencv_contrib220.lib")
#  pragma comment(lib, "opencv_ts220.lib")
#  pragma comment(lib, "opencv_legacy220.lib")
#  pragma comment(lib, "opencv_flann220.lib")
#  pragma comment(lib, "opencv_ffmpeg220.lib")
#  pragma comment(lib, "opencv_gpu220.lib")

# endif
#endif //#ifdef _MSC_VER

#define SHOW_IMAGE 0

#include "benchmark.h"

int main(int argc, char* argv[])
{
	IplImage* image = 0, *src = 0, *dst = 0;
	IplImage *gray = 0, *bin = 0, *img_16s = 0, *img_64f, *img_64f_1 = 0;
	IplImage *img_32f1 = 0, *img_32f1__2 = 0;
	CvMemStorage* storage = 0;

	// default image-name for testing
	char file_name[] = "image.jpg";

	// set default image or first param
	char* filename = argc == 2 ? argv[1] : file_name;

	// get picture
	image = cvLoadImage(filename);

	if(!image){
		fprintf(stderr, "[!][benchmark_cv] Error: cant load image: %s !\n", filename);
		return -1;
	}

	src = cvCloneImage(image);
	dst = cvCloneImage(image);
	gray = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	bin = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	img_16s = cvCreateImage(cvGetSize(image), IPL_DEPTH_16S, 3);
	img_64f = cvCreateImage(cvGetSize(image), IPL_DEPTH_64F, 3);
	img_64f_1 = cvCreateImage( cvSize(image->width+1, image->height+1), IPL_DEPTH_64F, 1);
	img_32f1 = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1);
	img_32f1__2 = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1);

	// convert to gray
	cvConvertImage(image, gray, CV_BGR2GRAY);
	storage = cvCreateMemStorage(0);
	
#if 0
	printf("[i] file name: %s\n", filename);

	// image info
	printf( "[i] channels:  %d\n",        image->nChannels );
	printf( "[i] pixel depth: %d bits\n",   image->depth );
	printf( "[i] width:       %d pixels\n", image->width );
	printf( "[i] height:      %d pixels\n", image->height );
	printf( "[i] image size:  %d bytes\n",  image->imageSize );
	printf( "[i] width step:  %d bytes\n",  image->widthStep );
#endif

#if SHOW_IMAGE
	char window[] = "image";
	cvNamedWindow(window);
#endif

	fprintf(stdout, "cvConvertImage \t\t%f\n", benchmark_cv(&bench_cvConvertImage_BGR2GRAY, image, bin));
#if SHOW_IMAGE
	cvShowImage(window, bin);
	cvWaitKey(0);
#endif
	fprintf(stdout, "cvCanny \t\t%f\n", benchmark_cv(&bench_cvCanny, gray, bin));
#if SHOW_IMAGE
	cvShowImage(window, bin);
	cvWaitKey(0);
#endif
	fprintf(stdout, "cvSobel \t\t%f\n", benchmark_cv(&bench_cvSobel, image, img_16s));
#if SHOW_IMAGE
	cvConvertScale(img_16s, dst);
	cvShowImage(window, dst);
	cvWaitKey(0);
#endif	
	fprintf(stdout, "cvLaplace \t\t%f\n", benchmark_cv(&bench_cvLaplace, image, img_16s));
#if SHOW_IMAGE
	cvConvertScale(img_16s, dst);
	cvShowImage(window, dst);
	cvWaitKey(0);
#endif	
	fprintf(stdout, "cvIntegral \t\t%f\n", benchmark_cv(&bench_cvIntegral, gray, img_64f_1));
#if SHOW_IMAGE
	cvShowImage(window, img_64f_1);
	cvWaitKey(0);
#endif	
	fprintf(stdout, "cvExtractSURF \t\t%f\n", benchmark_cv(&bench_cvExtractSURF, gray, storage));

	fprintf(stdout, "cvSmooth \t\t%f\n", benchmark_cv(&bench_cvSmooth, image, dst));
#if SHOW_IMAGE
	cvShowImage(window, dst);
	cvWaitKey(0);
#endif
	fprintf(stdout, "cv::FAST \t\t%f\n", benchmark_cv(&bench_FAST, gray));
	fprintf(stdout, "cvGoodFeaturesToTrack \t\t%f\n", benchmark_cv(&bench_cvGoodFeaturesToTrack, gray, img_32f1, img_32f1__2));
	
	
	cvReleaseImage(&image);
	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	cvReleaseImage(&gray);
	cvReleaseImage(&bin);
	cvReleaseImage(&img_16s);
	cvReleaseImage(&img_64f);
	cvReleaseImage(&img_64f_1);
	cvReleaseImage(&img_32f1);
	cvReleaseImage(&img_32f1__2);
	if(storage){
		cvReleaseMemStorage(&storage);
	}
	
	return 0;
}
