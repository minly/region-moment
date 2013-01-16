#include "Descriptors.h"


/**calculate the feature descriptors**/
CvMat** descriptors(IplImage* hsv_frame)
{
	CvMat** feature_set;
	CvMoments* moments;
	CvHuMoments* hu;
	int i, w, h, area;	
    unsigned long long int area2, area3, area4, area5, area7, area10;
	float xd, yd, rm1, tt;
	float area2_5, bbb=231.232;
	CvMat** descriptors, *YCM, *YCMI, *YRM;

	moments = (CvMoments*)malloc(sizeof(CvMoments));//store moments and central moments 
	hu = (CvHuMoments*)malloc(sizeof(CvHuMoments));
	descriptors = (CvMat**)malloc(N_DESCRIPTORS * sizeof(CvMat*));//wait release 1
    YCM = cvCreateMat(7, N_FEATURES, CV_32FC1);
	YCMI = cvCreateMat(7, N_FEATURES, CV_32FC1);
	YRM = cvCreateMat(1, N_FEATURES, CV_32FC1);
	w = hsv_frame->width;
	h = hsv_frame->height;				
	xd = -w/2;
	yd = -h/2;
	area = w * h;
	area2 = area*area;
	area2_5 = area2 * sqrt((float)area);
	area3 = area * area2;
	area4 = area2 * area2;
	area5 = area * area4;
	area7 = area2 * area5;
	area10 = area5 * area5;
//	printf("area=%llu, area2=%llu, area3=%llu, area4=%llu\n area5=%llu, area7=%llu, area10=%llu, area2_5=%lf, bbb=%f", area,area2,area3,area4,area5,area7,area10,area2_5, bbb);
	feature_set = calc_feature_set(hsv_frame);//get all the feature mats
	for(i=0; i<N_FEATURES; i++)
	{
		cvMoments(feature_set[i], moments, 0);
		cvGetHuMoments(moments, hu);
		cvmSet( YCM, 0, i, moments->mu02/(area2) );
		cvmSet( YCM, 1, i, moments->mu11/(area2) );
		cvmSet( YCM, 2, i, moments->mu20/(area2) );
		cvmSet( YCM, 3, i, moments->mu03/(area2_5) );
		cvmSet( YCM, 4, i, moments->mu12/(area2_5) );
		cvmSet( YCM, 5, i, moments->mu21/(area2_5) );
		cvmSet( YCM, 6, i, moments->mu30/(area2_5) );
		cvmSet( YCMI, 0, i, hu->hu1/area2 );
		cvmSet( YCMI, 1, i, hu->hu2/area4 );
		cvmSet( YCMI, 2, i, hu->hu3/area5 );
		cvmSet( YCMI, 3, i, hu->hu4/area5 );
		cvmSet( YCMI, 4, i, hu->hu5/area10 );
		cvmSet( YCMI, 5, i, hu->hu6/area7 );
		cvmSet( YCMI, 6, i, hu->hu7/area10 );
		rm1 = xd * xd * moments->m00 + xd * moments->m10 + moments->m20;
		// rm2 = pow( xd, 4 ) * moments->m00 + pow( xd, 3 ) * moments->m10 + pow( xd, 2 ) * moments->m20 + xd * moments->m30 +moments->m40 
		     // + xd * xd *yd *yd *moments->m00 + xd * xd * yd * moments->m01 + xd * xd *moments->m02 + xd * yd * yd * moments->m10 + xd * yd * moments->m11
			// + xd * moments->m12 + yd * yd * moments->m20 + yd * moments->m21 + moments->m22;						  
		cvmSet( YRM, 0, i, rm1/area2 );
		//cvmSet( YRM, 1, k, rm2/area3)
	 }
	descriptors[0] = YCM;
	descriptors[1] = YCMI;
	descriptors[2] = YRM;
	YCM = NULL;
	YCMI = NULL;
	YRM = NULL;
//	Prt_Mat_Data( descriptors[0], "ycm.txt" );
//	Prt_Mat_Data( descriptors[1], "ycmi.txt" );
//	Prt_Mat_Data( descriptors[2], "yrm.txt" );
	for(i=0; i<N_FEATURES; i++)
	{
		 cvReleaseMat(&feature_set[i]);
	}				   
	free(feature_set);
	free(moments);
	free(hu);
	return descriptors;

}

/**
calculate the Anisotropic features or Isotropic features, which will be used to calculate the feature descriptors
frame---hsv color space
**/
CvMat** calc_feature_set(IplImage* frame)
{
	int r, c, i, j, h, w, n, width_step;
	IplImage* gray_img, *x_deriv_img, *y_deriv_img, *xx_deriv_img, *yy_deriv_img, *xy_deriv_img, *h_, *s_, *v_;
	CvMat** feature_set;
	float k1, k2;
	CvMat* hession, *evects, *evals;

	/*here to calculate the gradient of the ROI, we can also calculate the whole gradient in the main function*/
	h = frame->height;
	w = frame->width;
	n = w * h;
	feature_set = (CvMat**)malloc(N_FEATURES* sizeof(CvMat*));
	for(i=0; i<N_FEATURES; i++)
	{
		feature_set[i] = cvCreateMat(h, w, CV_32FC1);
	}
	
	gray_img = cvCreateImage(cvGetSize(frame), IPL_DEPTH_32F, 1);	
	//Prt_Image_Data(frame, "frame.txt", 'f');
	//Prt_Image_Data(gray_img, "gray_img.txt", 'f');
	h_ = cvCreateImage( cvGetSize(frame), IPL_DEPTH_32F, 1 );//y
    s_ = cvCreateImage( cvGetSize(frame), IPL_DEPTH_32F, 1 );
    v_ = cvCreateImage( cvGetSize(frame), IPL_DEPTH_32F, 1 );
	
	x_deriv_img = cvCreateImage(cvGetSize(frame), IPL_DEPTH_32F, 1);
	y_deriv_img = cvCreateImage(cvGetSize(frame), IPL_DEPTH_32F, 1);
	xx_deriv_img = cvCreateImage(cvGetSize(frame), IPL_DEPTH_32F, 1);
	yy_deriv_img = cvCreateImage(cvGetSize(frame), IPL_DEPTH_32F, 1);
	xy_deriv_img = cvCreateImage(cvGetSize(frame), IPL_DEPTH_32F, 1);
	/*get each channel of frame*/
	cvCvtPixToPlane( frame, h_, s_, v_, NULL );
	Img2Mat(h_, feature_set[0]);
	Img2Mat(s_, feature_set[1]);
	Img2Mat(v_, feature_set[2]);
	/*convert to gray*/
	cvCvtColor(frame, gray_img, CV_RGB2GRAY);
	
#ifdef ANISOTROPIC /*features: R(x; y); G(x; y); B(x; y) ; |Ix(x; y)|; |Iy(x; y)|; |Ixx(x; y)|; |Iyy(x; y)|*/
	/*calc the first and second order derivations*/
	cvSobel(gray_img, x_deriv_img, 1, 0, 1);
	cvSobel(gray_img, y_deriv_img, 0, 1, 1);
	cvSobel(x_deriv_img, xx_deriv_img, 1, 0, 1);
	cvSobel(y_deriv_img, yy_deriv_img, 0, 1, 1);
	cvSobel(x_deriv_img, xy_deriv_img, 0, 1, 1);

	cvAbs(x_deriv_img, x_deriv_img);
	cvAbs(y_deriv_img, y_deriv_img);
	cvAbs(xx_deriv_img, xx_deriv_img);
	cvAbs(yy_deriv_img, yy_deriv_img);

	Img2Mat(x_deriv_img, feature_set[3]);
	Img2Mat(y_deriv_img, feature_set[4]);
	Img2Mat(xx_deriv_img, feature_set[5]);
	Img2Mat(yy_deriv_img, feature_set[6]);	

//	Prt_Image_Data(x_deriv_img, "x_deriv_img.txt", 'f');
//	Prt_Image_Data(y_deriv_img, "y_deriv_img.txt", 'f');
//	Prt_Image_Data(xy_deriv_img, "xy_deriv_img.txt", 'f');
//	Prt_Image_Data(xx_deriv_img, "xx_deriv_img.txt", 'f');
//	Prt_Image_Data(yy_deriv_img, "yy_deriv_img.txt", 'f');

	/*cvNamedWindow("frame");
	cvNamedWindow("gray_img");
	cvNamedWindow("x_deriv_img");
	cvNamedWindow("y_deriv_img");
	cvShowImage("frame", frame);
	cvShowImage("gray_img", gray_img);
	cvShowImage("x_deriv_img", x_deriv_img);
	cvShowImage("y_deriv_img", y_deriv_img);
	cvWaitKey(0);
	cvDestroyWindow("frame");
	cvDestroyWindow("gray_img");
	cvDestroyWindow("x_deriv_img");
	cvDestroyWindow("y_deriv_img");*/
#endif
#ifdef ISOTROPIC /*features: R(x; y); G(x; y); B(x; y); |deltaI(x; y)|; |kp(x; y)|; |ktheta(x; y)|; |s(x; y)|*/
	/*calc the mat features in the feature_set*/
	hession = cvCreateMat(2, 2, CV_32FC1);
	evects = cvCreateMat(2, 2, CV_32FC1);
	evals = cvCreateMat(2, 1, CV_32FC1);
	for(r=0; r<h; r++)
		for(c=0; c<w; c++)
		{
			width_step = gray_img->widthStep;
			cvmSet( feature_set[3], r, c, sqrt(((float*)(x_deriv_img->imageData + r*width_step))[c] * ((float*)(x_deriv_img->imageData + r*width_step))[c] +
				((float*)(y_deriv_img->imageData + r*width_step))[c] * ((float*)(y_deriv_img->imageData + r*width_step))[c]) );
			cvmSet( hession, 0, 0, ((float*)(xx_deriv_img->imageData + r*width_step))[c] );
			cvmSet( hession, 0, 1, ((float*)(xy_deriv_img->imageData + r*width_step))[c] );
			cvmSet( hession, 1, 0, ((float*)(xy_deriv_img->imageData + r*width_step))[c] );
			cvmSet( hession, 1, 1, ((float*)(yy_deriv_img->imageData + r*width_step))[c] );
//		    Prt_Mat_Data(hession, "hession1.txt");
			cvEigenVV( hession, evects, evals );
//			Prt_Mat_Data(hession, "hession2.txt");
//			Prt_Mat_Data(evals, "evals.txt");
			k1 = cvmGet( evals, 0, 0 );
			k2 = cvmGet(evals, 1, 0 );	
			cvmSet( feature_set[4], r, c, sqrt( k1 * k1 +k2 * k2 ) );
			cvmSet( feature_set[5], r, c, cvFastArctan( k2, k1 ) );
			cvmSet( feature_set[6], r, c, cvFastArctan( (k2 + k1), (k2 - k1) ) / 180 );
		}

		cvReleaseMat(&hession);
		cvReleaseMat(&evects);
		cvReleaseMat(&evals);

#endif
		
//		Prt_Mat_Data(feature_set[7], "mat7.txt");
//		Prt_Mat_Data(feature_set[8], "mat8.txt");
//		Prt_Mat_Data(feature_set[9], "mat9.txt");
//		Prt_Mat_Data(feature_set[10], "mat10.txt");
		cvReleaseImage(&gray_img);
		cvReleaseImage(&x_deriv_img);
		cvReleaseImage(&y_deriv_img);
		cvReleaseImage(&xy_deriv_img);
		cvReleaseImage(&xx_deriv_img);
		cvReleaseImage(&yy_deriv_img);
		cvReleaseImage(&h_);
		cvReleaseImage(&s_);
		cvReleaseImage(&v_);
		
		return feature_set;
}
/**cvt img to mat
   img-----single channel,32F **/
void Img2Mat(IplImage* img, CvMat* mat)
{
	mat = cvCreateMat(img->height, img->width,  CV_32FC1);
	memcpy( mat->data.fl, img->imageData, img->imageSize);
}

IplImage* bgr2hsv( IplImage* bgr )
{
  IplImage* bgr32f, * hsv;

  bgr32f = cvCreateImage( cvGetSize(bgr), IPL_DEPTH_32F, 3 );
  hsv = cvCreateImage( cvGetSize(bgr), IPL_DEPTH_32F, 3 );
  cvConvertScale( bgr, bgr32f, 1.0 / 255.0, 0 );//nomarlized the value of r,g and b within [0,1]
  cvCvtColor( bgr32f, hsv, CV_BGR2HSV );
  cvReleaseImage( &bgr32f );
  return hsv;
}

int cmp_dist(const void* p1, const void* p2)
{
	distance* _p1 = (distance*)p1;
	distance* _p2 = (distance*)p2;
	if( _p1->dist> _p2->dist )
        return 1;
     if( _p1->dist < _p2->dist )
        return -1;
    return 0;
}
double calc_distance( CvMat** descriptor1,  CvMat** descriptor2)
{

	int i, r, c, rn, cn;
	double sum = 0;	
	//for(i=0; i<N_DESCRIPTORS; i++)
	//{
		cvSub(descriptor2[0], descriptor1[0], descriptor2[0]);
//		Prt_Mat_Data( descriptor2[0], "sub.txt" );
		cvPow(descriptor2[0], descriptor2[0], 2);
//		Prt_Mat_Data(descriptor2[0], "pow.txt" );
		sum += cvSum(descriptor2[0]).val[0];
	//}
	sum = sqrt(sum);
	return sum;
}



void Prt_Image_Data(IplImage* img, char* filename, char format)
{
	FILE *fp;
	int r, c;
	fp = fopen(filename, "w");
	if(!fp) printf("error in opening gradient.txt");
	for(r=0; r<img->height; r++)
	{
		for(c=0; c<img->width; c++)
		{
			if(format == 'f')
				fprintf(fp, "%f ", ((float*)(img->imageData+img->widthStep*r))[c]);
			if(format == 'u')
				fprintf(fp, "%d ", ((uchar*)(img->imageData+img->widthStep*r))[c]);
		}
		fprintf(fp, "\n");
	}
		
	fclose(fp);			
}



void Prt_Mat_Data(CvMat *mat, char* filename)
{
	int r, c;
	FILE *fp;
	fp = fopen(filename, "a");
	if(!fp) printf("error in opening gradient.txt");
	for(r=0; r<mat->rows; r++)
	{
		for(c=0; c<mat->cols; c++)
			fprintf(fp, "%f ", cvmGet(mat, r, c));
		fprintf(fp, "\n");
	}
	fprintf(fp, "************************************************\n");
	fclose(fp);
		
}