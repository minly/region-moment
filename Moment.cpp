#include "Moment.h"

/*define anisotropic or isotropic features to use    ISOTROPIC/ANISOTROPIC*/
#define ISOTROPIC

/***************************** Function Prototypes ***************************/
CvMat** target_descriptors(char* dirname, char* dirscan);

/********************************** Globals **********************************/
//const char* vid_file = "G:\\video\\video_redteam.avi";
char* filter = "filtered_target.txt";

/********************************** Main **********************************/
void main()
{
	IplImage* frame, *hsv_frame; 
	CvMat** target_des, **candidate_des;
	int i = 0;
	distance dist[CANDIDATE];
    char* pname = 0; /* File name buffer */
    WIN32_FIND_DATAA info;
	char* filename;
	FILE* fp;
    HANDLE h = FindFirstFileA(DIRSCAN_CANDIDATE,&info);
	/**calc the average descriptors of the targets**/
	target_des = target_descriptors(DIRNAME_TARGET, DIRSCAN_TARGET);
//	Prt_Mat_Data( target_des[0], "average_of_target_cm.txt" );
//	Prt_Mat_Data( target_des[1], "average_of_target_cmi.txt" );
//	Prt_Mat_Data( target_des[2], "average_of_target_rm.txt" );
    if (h == INVALID_HANDLE_VALUE)
    {
        /* print message?.. */
        return;
    }
    pname = (char*)malloc(MAX_PATH); /* (char*) for C++ only */
	/**for each candidate img, calc the distance with the target descriptors**/
    do
    {
        if ((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            continue; /* skip directories */
        strcat(strcpy( pname,DIRNAME_CANDIDATE),info.cFileName );
		printf("%s", pname);
		frame = cvLoadImage( pname,  CV_LOAD_IMAGE_UNCHANGED );
		hsv_frame = bgr2hsv( frame );
        candidate_des = descriptors( hsv_frame );
		
//		Prt_Mat_Data(target_des[0], "des2.txt" );
//		Prt_Mat_Data( candidate_des[0], "des1.txt" );
		dist[i].file = (char*)malloc(strlen(pname)*sizeof(char));
		dist[i].dist = calc_distance( target_des, candidate_des );
		strcpy( dist[i].file, pname );
		i++;
        /* Now we have the next file name...    */
        /* Open, process then close the file ...*/
    } while (FindNextFileA(h,&info));
    FindClose(h); /* Close dir scan */
	cvReleaseImage(&hsv_frame);
    free(pname);	
	/**sort the distance descendantly**/
	fp = fopen("namelist.txt", "w");
	if(!fp) exit(0);
	for(i=0; i<CANDIDATE; i++)
		fprintf(fp, "%s\n", dist[i].file);
	fclose(fp);

	qsort(dist, CANDIDATE, sizeof(distance), &cmp_dist);
	fp = fopen(filter, "w");
	if(!fp) exit(0);
	for(i=0; i<CANDIDATE; i++)
		fprintf(fp, "%s\n", dist[i].file);
	fclose(fp);
	//here to write to get the imgs similarity in the former

	for(i=0; i<N_DESCRIPTORS; i++)
	{
		cvReleaseMat(&target_des[i]);
		cvReleaseMat(&candidate_des[i]);
	}
	
}


/*get the target average descriptors*/
CvMat** target_descriptors(char* dirname, char* dirscan)
{
	IplImage* frame, *hsv_frame;
	CvMat** each_tar_descri, **sum;
	int i, n = 0, r, c, r0, c0;

	char* pname = 0; /* File name buffer */
    WIN32_FIND_DATAA info;
	char* filename;
	sum = (CvMat**)malloc(sizeof(CvMat**));
    HANDLE h = FindFirstFileA(dirscan,&info);

    if (h == INVALID_HANDLE_VALUE)
    {
        /* print message?.. */
        return NULL;
    }	
    pname = (char*)malloc(MAX_PATH); /* (char*) for C++ only */
    do
    {
        if ((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            continue; /* skip directories */
        strcat(strcpy(pname,dirname),info.cFileName);
		frame = cvLoadImage(pname, 1);
		hsv_frame = bgr2hsv( frame );	
		each_tar_descri = descriptors(hsv_frame);
//		Prt_Mat_Data( each_tar_descri[0], "each_tar_descri_cm.txt" );
//		Prt_Mat_Data( each_tar_descri[1], "each_tar_descri_cmi.txt" );
//		Prt_Mat_Data( each_tar_descri[2], "each_tar_descri_rm.txt" );
		if(n == 0)
			for( i = 0; i<N_DESCRIPTORS; i++ )
	        {
				sum[i] = cvCreateMat(each_tar_descri[i]->rows, each_tar_descri[i]->cols, CV_32FC1); 
				for( r = 0; r<sum[i]->rows; r++)
					for( c = 0; c<sum[i]->cols; c++)
						cvmSet(sum[i], r, c, 0);
	        }
		for( i = 0; i<N_DESCRIPTORS; i++ )
		{
			//cvAdd( each_tar_descri[i], sum[i], sum[i] );
			for(r0 = 0; r0<sum[i]->rows; r0++)
				for(c0 = 0; c0<sum[i]->cols; c0++)
					cvmSet(sum[i], r0, c0, cvmGet(sum[i], r0, c0)+cvmGet(each_tar_descri[i], r0, c0));
		}
		n++;
        /* Now we have the next file name...    */
        /* Open, process then close the file ...*/
    } while (FindNextFileA(h,&info));
    FindClose(h); /* Close dir scan */
    free(pname);
	cvReleaseImage(&hsv_frame);
	/*get the average value of all the targets' descriptors*/
//	Prt_Mat_Data( sum[0], "sum0.txt" );
//	Prt_Mat_Data( sum[1], "sum1.txt" );
//	Prt_Mat_Data( sum[2], "sum2.txt" );
	for( i = 0; i<N_DESCRIPTORS; i++ )
		{
			for( r = 0; r<sum[i]->rows; r++)
				for( c = 0; c<sum[i]->cols; c++)
					cvmSet(sum[i], r, c, cvmGet(sum[i], r, c)/n);
		} 
	return sum;

}







