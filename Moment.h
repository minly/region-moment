#include "Descriptors.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>


/******************************** Definitions ********************************/

#define MAX_FRAMES 2048
#define CANDIDATE 60
#define DIRNAME_TARGET  "F:\\target\\"
#define DIRSCAN_TARGET "F:\\target\\*.*"
#define DIRNAME_CANDIDATE "F:\\candidate\\"
#define DIRSCAN_CANDIDATE "F:\\candidate\\*.*"
/********************************* Structures ********************************/

/* maximum number of objects to be tracked */
/*#define MAX_OBJECTS 1

typedef struct params {
  CvPoint loc1[MAX_OBJECTS];
  CvPoint loc2[MAX_OBJECTS];
  IplImage* objects[MAX_OBJECTS];
  char* win_name;
  IplImage* orig_img;
  IplImage* cur_img;
  int n;
} params;
*/

