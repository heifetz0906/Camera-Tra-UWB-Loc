#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <time.h>
#include <list>
#include <map>
#include <math.h>
#include <cmath>
#include "qcvTypes.h"
#include "t2t_exec_C.h"
#include "fastcv.h"

//#include <opencv\cv.h>
//#include <opencv\highgui.h>

//using namespace cv;
using namespace qcv::qot;
using namespace std;

unsigned char* yuvCurr;
//#include <sys/timeb.h>

#define  LOG_TAG    "BabyFaceDetJNI"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static JavaVM *sVm;
static const char* const kClassPathName = "qct/mmrd/babyfacedetdmo/BFDMainActivity";


//#include "utils/fastcvHeapStats.cpp"


static unsigned int params_flags = 0;
static int FRAME_WIDTH = FRAME_WIDTH_QVGA;
static int FRAME_HEIGHT = FRAME_HEIGHT_QVGA;
static int FRAME_NUMBER = FRAME_NUMBER_CONSTANT;

static char vidFileName[256];

static uint32_t VIDEO_START_FRAME = 0;

static FILE *output_file_fp = NULL;
static char currImageName[256] = "";

//file directories for input image files
typedef struct input_dir
{
    char fileDir[256];
    char img_prefix[256];
    char img_ext[10];
    int start_frame_num;
    int end_frame_num;
    int frame_name_length;
}input_dir;
static input_dir *img_dir=NULL;

//number of frames to wait for initilizing object bb
#define NUM_OF_FRAMES_WAIT_INIT_BB 100
static int numOfFrameWaitInitBB = 0;

static int totalTime = 0;

/*===========================================================================
Data Types
============================================================================*/

typedef struct _vc_tracker_internal_t
{
    //scveTracker *t2t;
    scveQOTHandle t2t;
    scveQOTObjectHandle t2tObjects[MAX_TRACK_OBJECTS];
    //unsigned char* srcImageLeft; //store converted previous frames
    unsigned char* srcImageRight; //store converted current frames
    int  selection[MAX_TRACK_OBJECTS][4];
    qcv::Point2D<uint32_t> *leftTop;
    qcv::Point2D<uint32_t> *rightBottom;
    uint32_t *trackConf;
    int trackObject;
    int numROIs;
    bool trackerInit;
    //fcvRectangleInt *trackingResult;
    uint32_t trackingResultX[MAX_TRACK_OBJECTS];
    uint32_t trackingResultY[MAX_TRACK_OBJECTS];
    uint32_t trackingResultWidth[MAX_TRACK_OBJECTS];
    uint32_t trackingResultHeight[MAX_TRACK_OBJECTS];
    //uint32_t trackingResultConfidence[MAX_TRACK_OBJECTS];
    bool *trackingValidity;
}vc_tracker_internal_t;

/*===========================================================================
Global Variables for PC test
============================================================================*/
//#ifndef ANDROID

static float32_t vizRatio = 1;
static uint32_t DISPLAY_HEIGHT = 0;
static uint32_t DISPLAY_WIDTH = 0;
//#endif

int selectObject = 0;
int trackObject = 0;
int numROIs = 1; //The number of objects to be tracked
int selectNum = 0;

//#ifndef ANDROID

int selection[MAX_TRACK_OBJECTS][4];
int leftTop[MAX_TRACK_OBJECTS][2], rightBottom[MAX_TRACK_OBJECTS][2];
//#endif

static int numFrames = FRAME_NUMBER;
static int numFrameSkip = 0;
static int numHandDetected = 0;
static int numFramesNotTracking = 0;
static float fps_average = 0;

//for NCC verification maps
uint32_t numNCCPosTemplates = 0;
uint32_t numNCCNegTemplates = 0;

uint8_t nccPosImgSmall[320*240];
uint8_t nccNegImgSmall[320*240];
uint32_t nccImgWidth = 320, nccImgHeight = 240, nccImgStride = 320;


//input bb size in camera mode
static int CAMERA_BB_W = 0;
static int CAMERA_BB_H = 0;

//input bb size in file mode
static int FILE_BB_X = 0;
static int FILE_BB_Y = 0;
static int FILE_BB_W = 0;
static int FILE_BB_H = 0;

char writeFileName[100];//eg: "..//T2T_720p_1.bin";

char writeImageDir[256];

/*===========================================================================
Function Prototypes
============================================================================*/

static void process_frame_t2t_tracker(vc_tracker_info_t* pTrackerInfo, vc_frame_desc_t *pFrame, double *x, double *y, unsigned int *confidence);
static void convert_to_tracker_frame(vc_tracker_info_t* pTrackerInfo);

void parse_parameters();

//unsigned int vclib_get_elapsed_milliseconds(void);

/*===========================================================================
Function Definitions (Intra Module)
============================================================================*/

//NV21 to RGB convertion
#define T2TBOUND(l,v,h)		( ((v)>=(l)) ? ( ((v)<=(h)) ? (v) : (h) ) : (l) )



unsigned int vc_tracker_init_t2t_tracker(vc_tracker_info_t* pTrackerInfo, unsigned int frameWidth, unsigned int frameHeight)
{
    vc_tracker_internal_t *trackerInternal = (vc_tracker_internal_t*)malloc(sizeof(vc_tracker_internal_t));

    //trackerInternal->t2t = new T2T();
    //if(params_flags & T2T_ENABLE_COLOR)
    //    //trackerInternal->t2t = scveInitObjectTracker(frameWidth, frameHeight, frameWidth, true, true);
    //    scveQOTInitC(frameWidth, frameHeight, frameWidth, TrackerParamModes::HIQUALITY, QOTColorFormat::YUV420, &(trackerInternal->t2t));
    //else
        scveQOTInitC(frameWidth, frameHeight, frameWidth, TrackerParamModes::HIQUALITY, QOTColorFormat::GRAY, &(trackerInternal->t2t));
    for (int i=0;i<MAX_TRACK_OBJECTS;i++)
    {
        trackerInternal->t2tObjects[i] = NULL;
    }


    trackerInternal->leftTop = (qcv::Point2D<uint32_t> *)malloc(sizeof(qcv::Point2D<uint32_t>) * MAX_TRACK_OBJECTS);
    trackerInternal->rightBottom = (qcv::Point2D<uint32_t> *)malloc(sizeof(qcv::Point2D<uint32_t>) * MAX_TRACK_OBJECTS);
    trackerInternal->trackConf = (uint32_t*)malloc(sizeof(uint32_t) * MAX_TRACK_OBJECTS);
    //trackerInternal->trackingResult = (fcvRectangleInt*)malloc(sizeof(fcvRectangleInt) * MAX_TRACK_OBJECTS);
    trackerInternal->trackingValidity = (bool *)malloc(sizeof(bool) * MAX_TRACK_OBJECTS);
    trackerInternal->trackerInit = false;

    //trackerInternal->srcImageLeft = (unsigned char*)malloc(frameWidth * frameHeight * 3);
    trackerInternal->srcImageRight = (unsigned char*)malloc(frameWidth * frameHeight * 1);

    trackerInternal->trackObject = 0;
    trackerInternal->numROIs = 0;

    if(params_flags & T2T_FILE_INPUT_BB_SIZE)
    {
        trackerInternal->selection[0][0] = FILE_BB_X;
        trackerInternal->selection[0][1] = FILE_BB_Y;
        trackerInternal->selection[0][2] = FILE_BB_W;
        trackerInternal->selection[0][3] = FILE_BB_H;
    }

    pTrackerInfo->trackerInternal = (void*)trackerInternal;


    return 0;
}

unsigned int vc_tracker_stop_t2t_tracker(vc_tracker_info_t* pTrackerInfo)
{
    vc_tracker_internal_t* trackerInternal = (vc_tracker_internal_t*)pTrackerInfo->trackerInternal;

    //scveReleaseObjectTracker(trackerInternal->t2t);

    //first unregister all object
    for (int i=0;i<MAX_TRACK_OBJECTS;i++)
    {
        if (trackerInternal->t2tObjects[i])
        {
            scveQOTUnregisterObjectC(trackerInternal->t2t, trackerInternal->t2tObjects[i]);
            trackerInternal->t2tObjects[i] = NULL;
        }
    }
    scveQOTReleaseC(trackerInternal->t2t);


    free(trackerInternal->leftTop);
    free(trackerInternal->rightBottom);
    free(trackerInternal->trackConf);
    //free(trackerInternal->srcImageLeft);
    free(trackerInternal->srcImageRight);
    //free(trackerInternal->trackingResult);
    free(trackerInternal->trackingValidity);
    trackerInternal->trackerInit = false;
    free(trackerInternal);


    return 0;
}


/*===============================================================================
Function Definitions (File Scope)
================================================================================*/

static void process_frame_t2t_tracker(vc_tracker_info_t* pTrackerInfo, vc_frame_desc_t *pFrame, double *x, double *y, unsigned int *confidence)
{
    vc_tracker_internal_t* trackerInternal = (vc_tracker_internal_t*)pTrackerInfo->trackerInternal;

    trackerInternal->trackObject = trackObject;

    //copy the selected area
    for (int i=0;i<MAX_TRACK_OBJECTS;i++)
		    	for (int j=0;j<4;j++)
		    		trackerInternal->selection[i][j] = selection[i][j];

    //as long as it is not no tracking
    if(trackerInternal->trackObject)
    {

        //convert image to tracker required format
        convert_to_tracker_frame(pTrackerInfo);

        //trackObject == -1 for setting ROIs
        if(trackerInternal->trackObject < 0)
        {
            trackerInternal->numROIs = numROIs;

            if(params_flags & T2T_FILE_INPUT_BB_SIZE)
            {
                trackerInternal->numROIs = 1;
                trackerInternal->selection[0][0] = FILE_BB_X;
                trackerInternal->selection[0][1] = FILE_BB_Y;
                trackerInternal->selection[0][2] = FILE_BB_W;
                trackerInternal->selection[0][3] = FILE_BB_H;
            }


            for (int i=0;i<MAX_TRACK_OBJECTS;i++)
            {
                if (trackerInternal->t2tObjects[i])
                {
                    scveQOTUnregisterObjectC(trackerInternal->t2t, trackerInternal->t2tObjects[i]);
                    trackerInternal->t2tObjects[i] = NULL;
                }
            }

            trackerInternal->trackerInit  = true;
            for(int i = 0; i < trackerInternal->numROIs; i++)
            {


				/* //VOT related stuff
                uint8_t* tempP = trackerInternal->srcImageRight;
                char csvA[255]; FILE *caHandle;
                sprintf_s(csvA,"C:\\Temp\\vot\\qotInput.txt");
                fopen_s(&caHandle, csvA, "w+");
                for(uint32_t i=0; i < pFrame->height * pFrame->width; i++)
                {
                    fprintf(caHandle, "%d ", tempP[i]);
                }
                fclose(caHandle);
				*/
				printf( "tttttttttttttt  position %d %d %d %d\n", trackerInternal->leftTop[0].x, trackerInternal->leftTop[0].y,trackerInternal->rightBottom[0].x,trackerInternal->rightBottom[0].y);

                //register each object
                scveQOTRegisterObjectC(trackerInternal->t2t, trackerInternal->srcImageRight, trackerInternal->selection[i][0], trackerInternal->selection[i][1], trackerInternal->selection[i][2], trackerInternal->selection[i][3], &(trackerInternal->t2tObjects[i]));


                if (trackerInternal->t2tObjects[i]==NULL)
                {
                    trackerInternal->trackerInit  = false;
                    params_flags &= ~T2T_FILE_INPUT_BB_SIZE;
                }
            }


            //Call T2T Initialization
            //trackerInternal->trackerInit = scveSelectObjectToTrack(trackerInternal->t2t, trackerInternal->srcImageRight, trackerInternal->selection, trackerInternal->numROIs);

            if (!trackerInternal->trackerInit)
            {
                trackObject = 0;

                selectNum = 0;
                selectObject = 0;

                for(int i = selectNum; i>=0; i--)
                {
                    selection[i][0] = 0;
                    selection[i][1] = 0;
                    selection[i][2] = 0;
                    selection[i][3] = 0;
                }
            }
            else
            {
                trackerInternal->trackObject = 1;
                trackObject = 1;
            }

            for (int i=0; i< trackerInternal->numROIs; i++)
            {   //set results for output
                trackerInternal->leftTop[i].x = leftTop[i][0] = trackerInternal->selection[i][0];
                trackerInternal->leftTop[i].y = leftTop[i][1] = trackerInternal->selection[i][1];
                trackerInternal->rightBottom[i].x = rightBottom[i][0] = trackerInternal->selection[i][0]+trackerInternal->selection[i][2] -1;
                trackerInternal->rightBottom[i].y = rightBottom[i][1] = trackerInternal->selection[i][1]+trackerInternal->selection[i][3];
                trackerInternal->trackConf[i] = 100;
            }
        }
        else
        {
            //otherwise, track the object
            //scveTrackObject(trackerInternal->t2t, trackerInternal->srcImageRight, trackerInternal->trackingResult,
            //            trackerInternal->trackingValidity, trackerInternal->trackConf);
            scveQOTTrackObjectsC(trackerInternal->t2t, trackerInternal->srcImageRight, trackerInternal->t2tObjects,
                trackerInternal->trackingResultX, trackerInternal->trackingResultY, trackerInternal->trackingResultWidth,
                trackerInternal->trackingResultHeight, trackerInternal->trackConf, trackerInternal->numROIs);

            //Return the tracking results
            qcv::Size2D<uint32_t> boxSize;
            if (trackerInternal->trackerInit)
            {
                for(int i = 0; i < trackerInternal->numROIs; i++)
                {
                    if(trackerInternal->trackingResultWidth[i] > 0)
                    {
                        trackerInternal->leftTop[i].x = trackerInternal->trackingResultX[i];
                        trackerInternal->leftTop[i].y = trackerInternal->trackingResultY[i];
                        trackerInternal->rightBottom[i].x = trackerInternal->trackingResultX[i] + trackerInternal->trackingResultWidth[i]-1;
                        trackerInternal->rightBottom[i].y = trackerInternal->trackingResultY[i] + trackerInternal->trackingResultHeight[i]-1;

                        leftTop[i][0] = trackerInternal->leftTop[i].x;
                        leftTop[i][1] = trackerInternal->leftTop[i].y;
                        rightBottom[i][0] = trackerInternal->rightBottom[i].x;
                        rightBottom[i][1] = trackerInternal->rightBottom[i].y;

                        boxSize.w = trackerInternal->rightBottom[i].x - trackerInternal->leftTop[i].x;
                        boxSize.h = trackerInternal->rightBottom[i].y - trackerInternal->leftTop[i].y;

                        *x = (double) trackerInternal->leftTop[i].x + boxSize.w / 2;
                        *y = (double) trackerInternal->rightBottom[i].y + boxSize.h / 2;
                        *confidence = (unsigned int) (trackerInternal->trackConf[i]);
                        //printf("[X,Y,W,H,Conf]=[%f, %f, %d, %d, %d]\n", *x, *y, boxSize.w, boxSize.h, *confidence);
                    }
                    else
                    {
                        leftTop[i][0] = trackerInternal->leftTop[i].x = 0;
                        leftTop[i][1] = trackerInternal->leftTop[i].y = 0;
                        rightBottom[i][0] = trackerInternal->rightBottom[i].x = 0;
                        rightBottom[i][1] = trackerInternal->rightBottom[i].y = 0;

                        boxSize.w = trackerInternal->rightBottom[i].x - trackerInternal->leftTop[i].x;
                        boxSize.h = trackerInternal->rightBottom[i].y - trackerInternal->leftTop[i].y;

                        *x = (double) trackerInternal->leftTop[i].x + boxSize.w / 2;
                        *y = (double) trackerInternal->rightBottom[i].y + boxSize.h / 2;
                        *confidence = (unsigned int) 0;
                        numFramesNotTracking++;
                    }
                }
            }
        }

        //switch left and right srcimage pointer
        //unsigned char *tmp;
        //tmp = trackerInternal->srcImageRight;
        //trackerInternal->srcImageRight = trackerInternal->srcImageLeft;
        //trackerInternal->srcImageLeft = tmp;
    }

    return;
}


//convert the current frame to track required frame, stored in the current image (srcImageRight)
static void convert_to_tracker_frame(vc_tracker_info_t* pTrackerInfo)
{
    vc_tracker_internal_t* trackerInternal = (vc_tracker_internal_t*)pTrackerInfo->trackerInternal;


        //image read in as RGB format

            //convert to Grey image
            //rgb2grey((unsigned char*)pFrame->virtualAddress, trackerInternal->srcImageRight, pFrame->width, pFrame->height, 2);  //2 0?
           /* uint32_t imgStride = AlignSize(pFrame->width*3, 4);
            fcvColorRGB888ToGrayu8 ((uint8_t *)pFrame->virtualAddress, pFrame->width, pFrame->height, imgStride, trackerInternal->srcImageRight, pFrame->width );*/

			trackerInternal->srcImageRight = yuvCurr;
            /*
            //temp
            CvMat a= cvMat(pFrame->height, pFrame->width,CV_8UC1, trackerInternal->srcImageRight);
            cvSaveImage("c:/temp/ORIGINAL_image_greyimage.bmp", &a);
            */



}

unsigned int confidenceValue = 0;
    double cameraTrackedX = 0;
    double cameraTrackedY = 0;
    vc_context_t *pContext = (vc_context_t*)malloc(sizeof(vc_context_t));

    int runFlag=0;

    int originWidth =1920, originHeight=1080;
static void BFD_babyFaceDetYUV(JNIEnv *env, jobject obj, jbyteArray y, jint width, jint height,
		jfloatArray boundingBoxes, jbyteArray scores, jintArray faceIds, jintArray numDetFaces,jint isInit)
{
	    jbyte* yChannel = (jbyte*) env->GetPrimitiveArrayCritical(y, 0);

		jbyte* nScores = (jbyte*) env->GetPrimitiveArrayCritical(scores, 0);
		jint *nDetFaces = (jint*) (env->GetPrimitiveArrayCritical(numDetFaces, 0));
		jfloat *nBoundingBoxes = (jfloat*) (env->GetPrimitiveArrayCritical(boundingBoxes, 0));
		jint *nFaceIds = (jint*) (env->GetPrimitiveArrayCritical(faceIds, 0));

		jint frameW = (jint) (width);
		jint frameH = (jint) (height);
		jint initT = (jint) (isInit);

		int frameWidth = (int)frameW;
		int frameHeight = (int)frameH;
		int initFlag = (int)initT;


		yuvCurr = (unsigned char*)yChannel;





		//LOGD("bbbbbbbbbbbbbbbbbbb origin size %d %d %d %d",frameWidth,frameHeight,initFlag,runFlag);


		if (initFlag==1 && runFlag==0)
			{


			            params_flags |= T2T_FILE_INPUT_BB_SIZE;

			            LOGD("ccccccccccc   %d %d %d %d", (int)nBoundingBoxes[4],(int)nBoundingBoxes[5],(int)nBoundingBoxes[6],(int)nBoundingBoxes[7]);

			            FILE_BB_X = ((float)((int)nBoundingBoxes[4]))*(float(640))/(float(originWidth));
						FILE_BB_Y = ((float)((int)nBoundingBoxes[5]))*(float(480))/(float(originHeight));
						FILE_BB_W = ((float)((int)nBoundingBoxes[6]))*(float(640))/(float(originWidth));
						FILE_BB_H =  ((float)((int)nBoundingBoxes[7]))*(float(480))/(float(originHeight));

			            trackObject = -1;

			            FRAME_WIDTH = frameWidth;
			            FRAME_HEIGHT = frameHeight;



				//LOGD("eeeeeeeeeeeeeeeeee inside static %d %d %d %d",FILE_BB_X,FILE_BB_Y,FILE_BB_W,FILE_BB_H);




			                if(pContext)
			                {
			                    //Initialize the internal context
			                    memset(pContext, 0, sizeof(vc_context_t));
			                    pContext->internal_frame.width = FRAME_WIDTH;
			                    pContext->internal_frame.height = FRAME_HEIGHT;

			                    //Initialize the tracker
			                    vc_tracker_init_t2t_tracker(&pContext->tracker_info, pContext->internal_frame.width, pContext->internal_frame.height);
			                }






//					    vc_tracker_stop_t2t_tracker(&pContext->tracker_info);
//					        free(pContext);

			    runFlag=1;
			    //LOGD("eeeeeeeeeeeeeeeeee done");
			}




		if (initFlag==1 && runFlag==1)
					{

			LOGD("ddddddddddddddddd   %d %d", numROIs, trackObject);




			if( trackObject ) //as long as it is not 0
			        {


			            //Call t2t tracker to process each frame
			            process_frame_t2t_tracker(&pContext->tracker_info, &pContext->internal_frame, &cameraTrackedX, &cameraTrackedY, &confidenceValue);




			            //printf("Number Frame = %d\n", numFrames);

			            for(int i = numROIs - 1; i >= 0; i--)
			            {


			                //save output to file, and visualize the bounding box
			                vc_tracker_internal_t* trackerInternal = (vc_tracker_internal_t*)pContext->tracker_info.trackerInternal;
			                if (trackerInternal->trackerInit)
			                {
			                    if (trackObject>0 )
			                    {



									LOGD("ggggggggggggggggggggggggg     %d %d %d %d      %d,%d,%d,%d,%d\n", FILE_BB_X,FILE_BB_Y,FILE_BB_W+FILE_BB_X,FILE_BB_H+FILE_BB_Y,leftTop[i][0], leftTop[i][1], rightBottom[i][0], rightBottom[i][1], trackerInternal->trackConf[i]);

			                        //change size for visualization



			                        //detections (before ncc)
			                        QOT *qot = (QOT *)trackerInternal->t2t;
			                        QOTResult motResult;
			                        QOTResult kcfResult;
			                        vector<qcv::DetectionScaleResult> detResults;
			                        vector<qcv::qot::QOTResult> detResultVerified;
			                        qot->getTrackingResults((uint32_t)trackerInternal->t2tObjects[0], motResult, kcfResult, detResults, detResultVerified);

			                        //get the NCC template images
			                        qot->getVerifierNCCImages((uint32_t)trackerInternal->t2tObjects[0], nccPosImgSmall, nccNegImgSmall, nccImgWidth, nccImgHeight, nccImgStride, numNCCPosTemplates, numNCCNegTemplates);
			                        //upsample the image
			                        //printf("bbbbbbbbbbbbbbbbbbb bb size %d %d %d %d    %d %d %d %d    %d %d %d %d    %d %d %d %d",FILE_BB_X,FILE_BB_Y,FILE_BB_W,FILE_BB_H,trackerInternal->selection[0][0],trackerInternal->selection[0][1] ,trackerInternal->selection[0][2] ,trackerInternal->selection[0][3] ,trackerInternal->leftTop[0].x, trackerInternal->leftTop[0].y,trackerInternal->rightBottom[0].x,trackerInternal->rightBottom[0].y,leftTop[0][0], leftTop[0][1], rightBottom[0][0], rightBottom[0][1] );


			                    }
			                }

			            }







			        }

					}
//	int px ;
//	int py ;
//	int qx ;
//	int qy ;
//	double angle;
//
//	double hypotenuse;
//
//	if (firstFlag==0)
//	{
//        LOGD("1");
//		yuvCurr = (unsigned char*)yuv;
//		yuvPrev = new unsigned char[(int)width*(int)height*sizeof(unsigned char)];
//		firstFlag=1;
//		memcpy( yuvPrev, yuvCurr, (int)width*(int)height*sizeof(unsigned char));
//
//		voting_x = new int [((int)width*2+1)*sizeof(int)];
//		voting_y = new int [((int)height*2+1)*sizeof(int)];
//		voting_angle = new int [(628+1)*sizeof(int)];
//
//		value_x = new int [((int)width*2+1)*sizeof(float)];
//		value_y = new int [((int)height*2+1)*sizeof(float)];
//		value_angle = new int [(628+1)*sizeof(float)];
//
//		LOGD("2");
//
//	}
//
//
//
//
//
//	else if (firstFlag==1)
//	{
//		LOGD("3");
//		int numLevels =3;
//
//		yuvCurr = (unsigned char*)yuv;
//
//
//
//
//		fcvGoodFeatureToTracku8( yuvPrev,
//				(int)width,
//				(int)height,
//				(int)width,
//				10,
//				0,
//				0.01,
//				xy,
//				nFeatures,
//				numcorners);
//
//		int32_t cornerNumber=0;
//		for (int32_t i=0;i<nFeatures;i++)
//		{
//			if (xy[i*2]>0 && xy[i*2]<(int)width && xy[i*2+1]>0 && xy[i*2+1]<(int)height)
//			{
//				//LOGD("first %d width height x y %d %d %d %d",firstFlag, (int)width, (int)height,xy[i*2],xy[i*2+1]);
//				cornerNumber++;
//			}
//			else
//			break;
//		}
//
//		fcvCornerRefineSubPixu8( yuvPrev,
//				(int)width,
//				(int)height,
//				(int)width,
//									3,
//									3,
//									5,
//									3,
//									xy,
//									cornerNumber,
//									xyOut);
//
//		LOGD("4");
//
//		memset(numStatus,0,nFeatures*sizeof(int32_t));
//		fcvPyramidLevel *src1Pyr, *src2Pyr;
//				src1Pyr = new fcvPyramidLevel[numLevels];
//				src2Pyr = new fcvPyramidLevel[numLevels];
//				fcvPyramidAllocate( src1Pyr, (int)width,(int) height, 1, numLevels, 0 );
//				fcvPyramidAllocate( src2Pyr,(int)width, (int)height, 1, numLevels, 0 );
//
//				fcvPyramidCreateu8( yuvPrev,
//						(int)width,
//						(int)height,
//							numLevels,
//							src1Pyr );
//
//				fcvPyramidCreateu8( yuvCurr,
//						(int)width,
//						(int)height,
//							numLevels,
//							src2Pyr );
//				//LOGD("width %d height %d corner num %d",(int)width, (int)height,cornerNumber);
//				fcvTrackLKOpticalFlowu8( yuvPrev,
//											yuvCurr,
//											(int)width,
//											(int)height,
//											src1Pyr,
//											src2Pyr,
//											NULL,
//											NULL,
//											xyOut,
//											xyOutOut,
//											numStatus,
//											cornerNumber,
//											7,
//											7,
//											5,
//											3,
//											0,
//											0,
//											0,
//											0 );
//				LOGD("5");
//				fcvPyramidDelete( src1Pyr,
//								numLevels,
//								1 );
//				fcvPyramidDelete( src2Pyr,
//								numLevels,
//								1 );
//				if (src1Pyr)
//				{
//					delete[] src1Pyr;   src1Pyr = NULL;
//				}
//				if (src2Pyr)
//				{
//					delete[] src2Pyr; src2Pyr = NULL;
//				}
//				int32_t nTrackFeature = 0;
//				for (int32_t i = 0 ; i < cornerNumber ; i++)
//				{
//					if (numStatus[i] == 1)
//					{
//
//						xyOutOut[nTrackFeature*2] = xyOutOut[i*2];
//						xyOutOut[nTrackFeature*2+1] = xyOutOut[i*2+1];
//						xyOut[nTrackFeature*2] = xyOut[i*2];
//						xyOut[nTrackFeature*2+1] = xyOut[i*2+1];
//						nTrackFeature++;
//						//LOGD("first %d width height x y %d %d %f %f",firstFlag, (int)width, (int)height,xyOutOut[i*2],xyOutOut[i*2+1]);
//					}
//
//				}
//
//
//
//				LOGD("6");
//				memset(voting_x,   0,   ((int)width*2+1)*sizeof(int));
//									   memset(voting_y,   0,   ((int)height*2+1)*sizeof(int));
//									   memset(voting_angle,   0,   (628+1)*sizeof(int));
//
//									   memset(value_x,   0,   ((int)width*2+1)*sizeof(float));
//									   memset(value_y,   0,   ((int)height*2+1)*sizeof(float));
//									   memset(value_angle,   0,   (628+1)*sizeof(float));
//
//
//				LOGD("7");
//							for (int i=0;i<nTrackFeature;i++)
//							{
//
//
//								 px = (int) (xyOut[i*2]);
//								 py = (int) (xyOut[i*2+1]);
//								 qx = (int) (xyOutOut[i*2]);
//								 qy = (int) (xyOutOut[i*2+1]);
//								double angle;		angle = std::atan2( (double) py - qy, (double) px - qx );
//
//								int idx_x = px - qx + (int)width +1;
//					   voting_x[idx_x]++;
//					   value_x[idx_x] = ((float)voting_x[idx_x]*value_x[idx_x] + px - qx)/float(voting_x[idx_x]+1);
//
//
//					   int idx_y = py - qy + (int)height +1;
//					   voting_y[idx_y]++;
//					   value_y[idx_y] = ((float)voting_y[idx_y]*value_y[idx_y] + py - qy)/float(voting_y[idx_y]+1);
//
//					   int idx_angle = int(angle*100) + 314 +1;
//					   voting_angle[idx_angle]++;
//					   value_angle[idx_angle] = ((float)voting_angle[idx_angle]*value_angle[idx_angle] + angle)/float(voting_angle[idx_angle]+1);
//
//					   //printf("%f %f %f %f\n",frame1_features[i].x,frame1_features[i].y,frame2_features[i].x,frame2_features[i].y);
//
//
//
//
//
//
//
//							}
//
//							LOGD("8");
//								 int max_val_x=0, max_idx_x=0;
//
//								  for (int pp=0;pp<(int)width*2+1;pp++)
//								  {
//									  if (voting_x[pp]>max_val_x)
//										  {
//											  max_val_x=voting_x[pp];
//											  max_idx_x=pp;
//									  }
//								  }
//
//								  int max_val_y=0, max_idx_y=0;
//
//								  for (int pp=0;pp<(int)height*2+1;pp++)
//								  {
//									  if (voting_y[pp]>max_val_y)
//										  {
//											  max_val_y=voting_y[pp];
//											  max_idx_y=pp;
//									  }
//								  }
//
//								  int max_val_angle=0, max_idx_angle=0;
//
//								  for (int pp=0;pp<(628+1);pp++)
//								  {
//									  if (voting_angle[pp]>max_val_angle)
//										  {
//											  max_val_angle=voting_angle[pp];
//											  max_idx_angle=pp;
//									  }
//								  }
//
//
//
//								  LOGD("9");
//
//								 px = (int)width/2;
//								 py = (int)height/2;
//
//
//					   //printf("%f %f %f %f\n",frame1_features[i].x,frame1_features[i].y,frame2_features[i].x,frame2_features[i].y);
//
//								 angle;		angle = atan2( (double) value_y[max_idx_y], (double) value_x[max_idx_x] );
//
//								 hypotenuse;	hypotenuse = sqrt( (value_y[max_idx_y])*(value_y[max_idx_y]) + (value_x[max_idx_x])*(value_x[max_idx_x]) );
//
//								qx = (int) (px - 3 * hypotenuse * cos(angle));
//								qy = (int) (py - 3 * hypotenuse * sin(angle));
//								LOGD("3");
//
//
//
//               qx=width-qx;
//               qy=height-qy;
//
//
//
//
//
//
//
//
//
//				memcpy( yuvPrev, yuvCurr, (int)width*(int)height*sizeof(unsigned char));
//				LOGD("10");
//	}




	LOGD("11");

	//LOGD("bbbbbbbbbbbbbbbbbbbb  %f %f %f %f",nBoundingBoxes[4],nBoundingBoxes[5],nBoundingBoxes[6],nBoundingBoxes[7]);

    	 nBoundingBoxes[0]  = ((float)leftTop[0][0])*(float(originWidth))/(float(640));
         nBoundingBoxes[1]=((float)leftTop[0][1])*(float(originHeight))/(float(480));
         nBoundingBoxes[2]=((float)rightBottom[0][0])*(float(originWidth))/(float(640));
         nBoundingBoxes[3]=((float)rightBottom[0][1])*(float(originHeight))/(float(480));





         //LOGE("FILTER_DEBUG: Filtered FaceID: %d Score: %d ", nFaceIds[i], (uint8_t)(nScores[i]));

	//releaseAllFDHandles();
	env->ReleasePrimitiveArrayCritical(y, yChannel, 0);

	env->ReleasePrimitiveArrayCritical(scores, nScores, 0);
	env->ReleasePrimitiveArrayCritical(numDetFaces, nDetFaces, 0);
	env->ReleasePrimitiveArrayCritical(boundingBoxes, nBoundingBoxes, 0);
	env->ReleasePrimitiveArrayCritical(faceIds, nFaceIds, 0);
	LOGD("12");

};

int jniRegisterNativeMethods(JNIEnv* env,
                             const char* className,
                             const JNINativeMethod* gMethods,
                             int numMethods)
{
    jclass clazz;

    LOGD("Registering %s natives", className);
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGE("Native registration unable to find class '%s'", className);
        return -1;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        LOGE("RegisterNatives failed for '%s'", className);
        return -1;
    }
    return 0;
}


static JNINativeMethod gMethods[] = {
//	{ "initFaceDet",       "(IIII)V",             (void*) BFD_initFaceDet       },
//	{ "deInitFaceDet",       "()V",             (void*) BFD_deInitFaceDet       },
	{ "babyFaceDetYUV",       "([BII[F[B[I[II)V",             (void*) BFD_babyFaceDetYUV        },
//	{ "flipVerticalYUV", "([BII[B[B)V", (void*)BFD_FlipVerticalYUV},
};

int register_methods(JNIEnv *env) {

	return jniRegisterNativeMethods(env, kClassPathName, gMethods, sizeof(gMethods)/sizeof(gMethods[0]));
}



jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv* env = NULL;
	jint result = JNI_ERR;
	sVm = vm;

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		LOGE("GetEnv failed!");
		return result;
	}

	LOGD("JNI_OnLoad loading...");
	if(register_methods(env) != JNI_OK) {
		LOGE("Can not load native methods!");
		goto _end;
	}
	LOGD("Loaded");

	result = JNI_VERSION_1_6;

_end:
	return result;
}







