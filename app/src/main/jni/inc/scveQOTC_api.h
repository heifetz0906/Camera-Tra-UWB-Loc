/*========================================================================

*//** @file scveQOTC_api.h

@par DESCRIPTION:      
      Qualcomm object tracker (a.k.a. touch to track tracker), API calls 
	  to be exposed to upper layer code

@par EXTERNALIZED FUNCTIONS:
      See below.

    Copyright (c) 2012 QUALCOMM, Incorporated.  All Rights Reserved.
    QUALCOMM Proprietary. Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.

*//*====================================================================== */

#ifndef SCVEQOTC_API_H
#define SCVEQOTC_API_H

#include "fastcv.h"
#include "fastcv_.h"

//------------------------------------------------------------------------------
/// @brief
///    Handle refering to the particular instance of QOT. This handle
///    is passed to each function that deals with the QOT tracker.
//------------------------------------------------------------------------------
typedef void * scveQOTHandle;

//------------------------------------------------------------------------------
/// @brief
///    Handle refering to the particular instance of QOT objects. This handle
///    is passed to each QOT function that deals with objects.
//------------------------------------------------------------------------------
typedef void * scveQOTObjectHandle;


#define SCVE_TRACKER_LL_NUM_DF_SCALES 4 //1


enum TrackerParamModes {HIQUALITY = 0, HISPEED = 1};
enum QOTColorFormat {GRAY=0, YUV420 = 1 };

//==============================================================================
// Include Files
//==============================================================================
#ifdef __cplusplus
extern "C"
{
#endif


// -----------------------------------------------------------------------------
/// @brief
///   One time initialization of the object tracker. Once the tracker is
///   initialized, subsequent calls to scveQOT functions can be performed.
///  
///   Here is the typical usage:
///  
///     scveQOTInit(...)
///     scveQOTRegisterObject(...);
///       ... get camera frame ...
///			scveQOTTrackObjects(... )
///       ... do something with each tracked object's ROI provided confidence is high
///       ... continually loop and get camera frames until tracking is no longer required
///     scveQOTUnregisterObject(...);
///     scveRelease(...);
///
/// @param srcWidth
///   Input image width. The number of pixels in a row.
///   \n\b NOTE: must be 128-bit aligned.
///   \n\b Must be multiple of 16
///
/// @param srcHeight
///   Input image height.
///   \n\b Must be multiple of 16
///
/// @param srcStride
///   Input image stride, is the number of bytes between column 0 of row 1 and
///   column 0 of row 2 in data memory. If left at 0 srcStride is default to srcWidth. 
///   \n\b Must be multiple of 16
///
/// @param qotMode
///   Input tracker parameter modes: HIQUALITY, HISPEED
///
/// @param qotHandle
///   Output QOT tracker handle. If qotHandle is NULL then QOT was unable to
///   be initialized.
///
/// @ingroup Motion_and_Object_Tracking
void 
scveQOTInitC(uint32_t        srcWidth,
             uint32_t        srcHeight,
             uint32_t        srcStride,
             TrackerParamModes qotMode,
             QOTColorFormat colorFormat,
             scveQOTHandle   *qotHandle);


// -----------------------------------------------------------------------------
/// @brief
///   Register an object to track. The roi values bound the object that should
///   be tracked
///
/// @param qotHandle
///   QOT tracker handle returned by scveQOTInit
///
/// @param src1
///   Input image from frame #1.
///   \n\b NOTE: must be 128-bit aligned.
///   \n\b NOTE: src1 buffer size is consistent with  
//               srcWidth, srcHeight, srcStride set in fcvQOTInit
///
/// @param roiX           
///	  Region of Interest's top left's x value within the source.
///
/// @param roiY           
///   Region of Interest's top left's y value within the source.
///
/// @param roiWidth
///   Region of Interest's width.
///
/// @param roiHeight
///   Region of Interest's height. 
///
/// @param objectHandle
///   Output QOT object handle. If the result is NULL the object was unable to
///   be initialized.
///
/// @ingroup Motion_and_Object_Tracking
// -----------------------------------------------------------------------------
void
scveQOTRegisterObjectC(scveQOTHandle			qotHandle, 
					uint8_t* __restrict	src1, 
					uint32_t					roiX,
					uint32_t					roiY,
					uint32_t					roiWidth,
					uint32_t					roiHeight,
					scveQOTObjectHandle*		objectHandle );



// -----------------------------------------------------------------------------
/// @brief
///   The function will search for all objects in the object list passed in 
///   objectHandle. The function determines object positions 
///
/// @param qotHandle
///   QOT tracker handle returned by scveQOTInit
///
/// @param src
///   Input image. This src frame cannot be NULL.
///   \n\b NOTE: must be 128-bit aligned.
///				 src buffer size is consistent with  
//               srcWidth, srcHeight, srcStride set in scveQOTInit
///
/// @param objectHandle
///   Array of QOT tracker object handles returned by scveQOTRegisterObject
///
/// @param roiX           
///	  Output array region of Interest's top left's x value within the source.
///   Each array entry corresponds to the same array index of the object
///   handle array.
///
/// @param roiY           
///   Output array region of Interest's top left's y value within the source.
///   Each array entry corresponds to the same array index of the object
///   handle array.
///
/// @param roiWidth
///   Output array region of Interest's width.
///   Each array entry corresponds to the same array index of the object
///   handle array.
///
/// @param roiHeight
///   Output array region of Interest's height. 
///   Each array entry corresponds to the same array index of the object
///   handle array.
///
/// @param confidence
///   Output array of confidence values for the array of associated object 
///   handles. Confidence values range from 0 - 100. The higher the confidence
///   value returned, the more likely that the associated ROI returned is 
///   the object being tracked (object associated with the object handle).
///   A returned confidence value of 0 indicates that the object associated 
///   with the object handle was not found in the src2 frame.
///   Each array entry corresponds to the same array index of the object
///   handle array.
///
/// @param numObjects
///   the number of objects to track in the object handle
///
/// @ingroup Motion_and_Object_Tracking
// -----------------------------------------------------------------------------
void 
scveQOTTrackObjectsC(scveQOTHandle					qotHandle, 
					uint8_t* __restrict		src,
					scveQOTObjectHandle* __restrict	objectHandle, 
					uint32_t* __restrict 			roiX,
					uint32_t* __restrict			roiY,
					uint32_t* __restrict			roiWidth,
					uint32_t* __restrict			roiHeight,
					uint32_t* __restrict			confidence,
					uint32_t						numObjects );



// -----------------------------------------------------------------------------
/// @brief
///   Unregisters an object with QOT and releases all resources associated 
///   with it. The object handle is no longer valid when this function 
///   returns.
///
/// @param objectHandle
///   QOT tracker object handle returned by scveQOTRegisterObject
///
/// @ingroup Motion_and_Object_Tracking
// -----------------------------------------------------------------------------
void
scveQOTUnregisterObjectC(scveQOTHandle			qotHandle,
                        scveQOTObjectHandle     objectHandle );


// -----------------------------------------------------------------------------
/// @brief
///   Releases and deinitializes the tracker. The tracker handle is no longer 
///   valid when this function returns.
///
/// @param qotHandle
///   QOT tracker handle returned by scveQOTInit
///
/// @ingroup Motion_and_Object_Tracking
// -----------------------------------------------------------------------------
void 
scveQOTReleaseC( scveQOTHandle qotHandle );


#ifdef __cplusplus
}
#endif

#endif //SCVEQOTC_API_H
