
#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H
#include "opencv/cv.h"
#include "opencv/highgui.h"


#define N_FEATURES 7//anisotropic or isotropic features
#define N_DESCRIPTORS 3


typedef struct distance{
	char* file;
	double dist;
} distance;


IplImage* bgr2hsv( IplImage* bgr );
void Prt_Image_Data(IplImage* img, char* filename, char format);
CvMat** descriptors(IplImage* hsv_frame);
CvMat** calc_feature_set(IplImage* img);
void Prt_Mat_Data(CvMat *mat, char* filename);
void Img2Mat(IplImage* img, CvMat* mat);
int cmp_dist(const void* p1, const void* p2);
double calc_distance( CvMat** descriptor1,  CvMat** descriptor2);



#endif