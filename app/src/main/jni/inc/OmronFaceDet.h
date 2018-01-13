#ifndef OMRON_FACE_DET_HH
#define OMRON_FACE_DET_HH

#include "OkaoAPI.h"
#include "OkaoDef.h"
#include "OkaoDtAPI.h"
#include "OkaoPtAPI.h"
#include "OkaoSmAPI.h"
#include "OkaoFrAPI.h"
#include "OkaoGbAPI.h"
#include "OkaoCtAPI.h"
#include "OkaoPcAPI.h"

typedef struct {
    INT32 nMaxFaceNumber;
    INT32 nMaxSwapNumber;
    INT32 nMinSize;
    INT32 nMaxSize;
    INT32 nSearchStep;
    INT32 nMode;
} FdConfig_ST;

int configFaceDet();

int createFaceDetConfig(int maxSize, int minSize);

int releaseAllFDHandles();

int createAllFDHandles();

int faceDetection( unsigned char *pImage,
                   int imWidth, 
                   int imHeight, 
                   int &count, 
                   FACEINFO* pFaceInfo);

int facePartDetection(unsigned char* pImage,
                                 int  imWidth,
                                 int  imHeight, 
                               POINT* topLeft, 
                               POINT* topRight, 
                               POINT* bottomLeft, 
                               POINT* bottomRight,
                               int pose,
                               POINT* location,
                                 int* confidence);

int faceContourDetection( unsigned char* pImage,
                                 int  imWidth,
                                 int  imHeight, 
                               POINT* aptCtPoint);

/* Perform smile degree estimation */
int faceSmileDetection(unsigned char* pImage,
                                 int  imWidth,
                                 int  imHeight,
                                 int* smileDegree);

 /* Perform gaze-blink estimation */
int faceBlinkDetection(unsigned char* pImage,
                                 int  imWidth,
                                 int  imHeight,
                                 int* eyeClosenessLeft,
                                 int* eyeClosenessRight);

#endif