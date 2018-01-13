/*========================================================================

*//** @file t2t_exec.h

@par DESCRIPTION:
      

@par EXTERNALIZED FUNCTIONS:
      See below.

    Copyright (c) 2012 QUALCOMM, Incorporated.  All Rights Reserved.
    QUALCOMM Proprietary. Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.

*//*====================================================================== */

#ifndef QT2T_TRACKER_H
#define QT2T_TRACKER_H

/*========================================================================
 Includes
 ========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "fastcv.h"
#include "fastcv_.h"

#include "scveQOTC_api.h"

#include "QOT.h"

/*========================================================================
 Defines
 ========================================================================*/ 
//Macros for run-time parameters
#define T2T_VIDEO_MODE (1)
#define T2T_CAMERA_MODE (1<<2)
#define T2T_DIR_MODE (1<<3)

#define T2T_ENABLE_COLOR (1<<4)

#define T2T_PLAY_FORWARD (1<<12)
#define T2T_PAUSE_EACH_FRAME (1<<13)

#define T2T_WRITE_VIDEO (1<<14)

#define T2T_VERBOSE (1<<15)

#define T2T_NO_WIN_DISPLAY (1<<16)
#define T2T_NO_DOTS (1<<17)

#define T2T_FILE_INPUT_BB_SIZE (1<<19)
#define T2T_WRITE_RAW_IMAGES (1<<20)
#define T2T_WRITE_TRACKED_IMAGES (1<<21)




//Macros for compiled parameters
#define SECOND_TO_MILLISECOND   1000
#define FRAME_WIDTH_QVGA 320
#define FRAME_HEIGHT_QVGA 240

#define FRAME_WIDTH_VGA 640
#define FRAME_HEIGHT_VGA 480

#define FRAME_WIDTH_720P 1280
#define FRAME_HEIGHT_720P 720

#define FRAME_WIDTH_1080P 1920
#define FRAME_HEIGHT_1080P 1080

#define FRAME_NUMBER_CONSTANT 500

#define T2T_COLOR_CHANNEL_NUMBER 0

#define MAX_TRACK_OBJECTS 5

//#define T2T_Display_Detection




/*========================================================================
 Data Types
 ========================================================================*/
typedef struct _vc_tracker_info_t
{
    unsigned int trackedObjectWidth;
    unsigned int trackedObjectHeight;
    int visualDebug;
    void* trackerInternal;
    
} vc_tracker_info_t;



typedef struct vc_frame_desc 
{
    unsigned int width;              /*!< Full width of this entire frame, for a side to side image, this is the stereo width, for top-bottom this is the mono width */
    unsigned int height;             /*!< Full height of this frame, for a side to side image, this is the mono height, for top-bottom this is the mono height*2 */
    unsigned int stride;             /*!< Full stride of this entire frame, should typically be equal to width. For stride that is not equal to width, this is only currently supported for an analysis frame. */
    int fd;                          /*!< File descriptor - applicable in linux */
    void *baseAddress;               /*!< Base address of memory block - applicable in linux */
    void *virtualAddress;            /*!< Virtual address of frame */
    //vc_frame_format_t format;
} vc_frame_desc_t;

typedef struct _vc_context_t
{
    //tracker params
    vc_tracker_info_t tracker_info;

	//general
	vc_frame_desc_t internal_frame;

} vc_context_t;

/*========================================================================
 Entry points
 ========================================================================*/

// ** Tracker control/processing
unsigned int vc_tracker_init_t2t_tracker(vc_tracker_info_t* pTrackerInfo, unsigned int frameWidth, unsigned int frameHeight);
unsigned int vc_tracker_stop_t2t_tracker(vc_tracker_info_t* pTrackerInfo);
void vc_tracker_process_frame_t2t_tracker(vc_tracker_info_t* pTrackerInfo, vc_frame_desc_t *pFrame, double *x, double *y, unsigned int *confidence);

#endif // QT2T_TRACKER_H
