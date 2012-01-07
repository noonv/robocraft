//
// OpenCV benchmark functions
//
//
// http://robocraft.ru
//

#include "benchmark.h"

float benchmark( void (*function)(), int count )
{
	float res = 0;

	if(!function || count<=0){
		fprintf(stderr, "[!][benchmark] Error: bad param!\n");
		return res;
	}

	double begin = 0;
	double end = 0;
	double summ = 0;

	for(int i=0; i<count; i++){
		begin = (double)cvGetTickCount();
		(*function)();
		end = (double)cvGetTickCount();
		summ += (end - begin);
	}

	summ /= count;
	res = (float)(summ/(cvGetTickFrequency()*1000.0));

	return res;
}

float benchmark_cv( void (*function)(IplImage*), IplImage* src, int count )
{
	float res = 0;

	if(!function || !src || count<=0){
		fprintf(stderr, "[!][benchmark_cv] Error: bad param!\n");
		return res;
	}

	double begin = 0;
	double end = 0;
	double summ = 0;

	for(int i=0; i<count; i++){
		begin = (double)cvGetTickCount();
		(*function)(src);
		end = (double)cvGetTickCount();
		summ += (end - begin);
	}

	summ /= count;
	res = (float)(summ/(cvGetTickFrequency()*1000.0));

	return res;
}

float benchmark_cv( void (*function)(IplImage*, IplImage*), IplImage* src, IplImage* dst, int count )
{
	float res = 0;

	if(!function || !src || !dst || count<=0){
		fprintf(stderr, "[!][benchmark_cv] Error: bad param!\n");
		return res;
	}

	double begin = 0;
	double end = 0;
	double summ = 0;

	for(int i=0; i<count; i++){
		begin = (double)cvGetTickCount();
		(*function)(src, dst);
		end = (double)cvGetTickCount();
		summ += (end - begin);
	}

	summ /= count;
	res = (float)(summ/(cvGetTickFrequency()*1000.0));

	return res;
}

float benchmark_cv( void (*function)(IplImage*, IplImage*, IplImage*), IplImage* src, IplImage* dst, IplImage* dst2, int count )
{
	float res = 0;

	if(!function || !src || !dst || !dst2 || count<=0){
		fprintf(stderr, "[!][benchmark_cv] Error: bad param!\n");
		return res;
	}

	double begin = 0;
	double end = 0;
	double summ = 0;

	for(int i=0; i<count; i++){
		begin = (double)cvGetTickCount();
		(*function)(src, dst, dst2);
		end = (double)cvGetTickCount();
		summ += (end - begin);
	}

	summ /= count;
	res = (float)(summ/(cvGetTickFrequency()*1000.0));

	return res;
}

float benchmark_cv( void (*function)(IplImage*, CvMemStorage*), IplImage* src, CvMemStorage* storage, int count )
{
		float res = 0;

	if(!function || !src || !storage || count<=0){
		fprintf(stderr, "[!][benchmark_cv] Error: bad param!\n");
		return res;
	}

	double begin = 0;
	double end = 0;
	double summ = 0;

	for(int i=0; i<count; i++){
		begin = (double)cvGetTickCount();
		(*function)(src, storage);
		end = (double)cvGetTickCount();
		summ += (end - begin);
	}

	summ /= count;
	res = (float)(summ/(cvGetTickFrequency()*1000.0));

	return res;
}

//
// wrappers for OpenCV functions
//
void bench_cvConvertImage_BGR2GRAY(IplImage* src, IplImage* dst)
{
	cvConvertImage(src, dst, CV_BGR2GRAY);
}

void bench_cvCanny(IplImage* src, IplImage* dst)
{
	cvCanny(src, dst, 50, 200);
}

void bench_cvSobel(IplImage* src, IplImage* dst)
{
	cvSobel(src, dst, 1, 1);
}

void bench_cvLaplace(IplImage* src, IplImage* dst)
{
	cvLaplace(src, dst);
}

void bench_cvIntegral(IplImage* src, IplImage* dst)
{
	cvIntegral(src, dst);
}

CvSURFParams surf_params = cvSURFParams(500, 1);

void bench_cvExtractSURF(IplImage* src, CvMemStorage* storage)
{
	CvSeq *keypoints = 0;
	CvSeq *descriptors = 0;
	cvExtractSURF( src, 0, &keypoints, &descriptors, storage, surf_params );
}

void bench_cvSmooth(IplImage* src, IplImage* dst)
{
	cvSmooth(src, dst, CV_GAUSSIAN, 3, 3);
}

void bench_FAST(IplImage* gray)
{
	std::vector< cv::KeyPoint > keypoints;  

	// детектирование углов с использованием алгоритма FAST
	cv::FAST(gray, keypoints, 30);
}

// максимальное число особых (угловых) точек для отслеживания
#define MAX_CORNERS 100
// массив для хранения точек
CvPoint2D32f cornersA[MAX_CORNERS];

void bench_cvGoodFeaturesToTrack(IplImage* src, IplImage* eig_image, IplImage* tmp_image)
{
	int corner_count = MAX_CORNERS;
	cvGoodFeaturesToTrack(
		src,
		eig_image,
		tmp_image,
		cornersA,
		&corner_count,
		0.01,
		5.0,
		0,
		3,
		0,
		0.04
		);
}
