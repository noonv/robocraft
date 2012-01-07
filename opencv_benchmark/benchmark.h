//
// OpenCV benchmark functions
//
//
// http://robocraft.ru
//

#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

#define DEFAULT_COUNT 100

#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#include <cv.h>
#include <highgui.h>
#include <cvaux.h> // FAST

float benchmark( void (*function)(), int count = DEFAULT_COUNT );
float benchmark_cv( void (*function)(IplImage*), IplImage* src, int count = DEFAULT_COUNT );
float benchmark_cv( void (*function)(IplImage*, IplImage*), IplImage* src, IplImage* dst, int count = DEFAULT_COUNT );
float benchmark_cv( void (*function)(IplImage*, IplImage*, IplImage*), IplImage* src, IplImage* dst, IplImage* dst2, int count = DEFAULT_COUNT );
float benchmark_cv( void (*function)(IplImage*, CvMemStorage*), IplImage* src, CvMemStorage* storage, int count = DEFAULT_COUNT );

//
// wrappers for OpenCV functions
//
void bench_cvConvertImage_BGR2GRAY(IplImage*, IplImage*);
void bench_cvCanny(IplImage*, IplImage*);
void bench_cvSobel(IplImage*, IplImage*);
void bench_cvLaplace(IplImage*, IplImage*);
void bench_cvIntegral(IplImage*, IplImage*);
void bench_cvExtractSURF(IplImage* src, CvMemStorage* storage);

void bench_cvSmooth(IplImage*, IplImage*);
void bench_FAST(IplImage*);
void bench_cvGoodFeaturesToTrack(IplImage*, IplImage*, IplImage*);

#endif //#ifndef _BENCHMARK_H_
