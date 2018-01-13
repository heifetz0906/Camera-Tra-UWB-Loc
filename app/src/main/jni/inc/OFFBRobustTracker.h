//
/**=============================================================================

@file
OFFBRobustTracker.h

@brief
Definition of an forward-backward optical flow tracker.

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved Qualcomm Technologies Proprietary

Export of this technology or software is regulated by the U.S.
Government. Diversion contrary to U.S. law prohibited.

All ideas, data and information contained in or disclosed by
this document are confidential and proprietary information of
Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
By accepting this material the recipient agrees that this material
and the information contained therein are held in confidence and in
trust and will not be used, copied, reproduced in whole or in part,
nor its contents revealed in any manner to others without the express
written permission of Qualcomm Technologies Incorporated.

=============================================================================**/

#ifndef qcv_offbrobustTracker_h_
#define qcv_offbrobusttracker_h_

#include <cstddef>
#include <cstdint>
#include <cassert>

#include "fastcv.h"

#include "ImageTracker.h"
#include "qcvMemory.h"
#include "qcvTypes.h"
#include "qcvRect.h"
#include "qcvImageExtension.h"
#include "MotionAnalysis.h"

namespace qcv
{

//#define OFFB_ROBUST_DEBUG
#define NA_MV -1000.0f

/*==========================================================================
Optical Flow Tracker Component Data Type
===========================================================================*/

struct OFFBRobustTrackerParams
{
    //parameters
    int32_t             numLKX; //number of pixels in row
    int32_t             numLKY; //number of pixels in column
    int32_t             numBBPoints;

    int32_t             LKOFWindowSizeWidth;
    int32_t             LKOFWindowSizeHeight;

    int32_t             maxIter;    
    float32_t           scaleRatio;
    float32_t           thresholdMedianMinRel;

    float32_t            minOFBBSize;
};


class OFFBRobustTracker : public ImageTracker<float32_t>
{

public:
    OFFBRobustTracker(uint32_t imW, uint32_t imH, float32_t minBBSize);

    virtual ~OFFBRobustTracker();

    //track image patch (object) at prevBB in prevImg to BB1 in currImg. currBB is the output.    
    //return true if the tracking is performed, otherwise return false
    bool track(ImageExtension &prevImg, ImageExtension &currImg, const BoundingBox<float32_t> &prevBB, BoundingBox<float32_t> &currBB, float32_t &confidence);
    bool track(ImageExtension &prevImg, ImageExtension &currImg, const Rect<float32_t> &prevBB, Rect<float32_t> &currBB, float32_t &confidence);

private: 

    /*==========================================================================
    Optical Flow Tracker Function Declarations
    ===========================================================================*/
    //Init the buffers for optical flow tracker
    void init(uint32_t imW, uint32_t imH, float32_t minBBSize);

    //Get the points of the tracked BB
    void getBBPoints(float32_t* BB, float32_t *featureXY0); //, float32_t *featureXY1, float32_t *featureXY2);

    //BB points for Global motion
    void getGlobalBBPoints(float32_t* BB, float32_t *featureXY0);

    //Predict the new BB based on the results of the optical flow tracking
    bool ofPredict(float32_t *BB0, float32_t *BB1, Point2D<float32_t> *startPoints, Point2D<float32_t> *endPoints, int32_t numUsed);

    bool ofPredictWeighted(float32_t *BB0, float32_t *BB1, Point2D<float32_t> *startPoints, Point2D<float32_t> *endPoints, int32_t numUsed);

    bool ofPredictWeightedLS(float32_t *BB0, float32_t *BB1, Point2D<float32_t> *startPoints, Point2D<float32_t> *endPoints, int32_t numUsed);
    
    //Track each BB
    bool ofTrackBB(float32_t* BB0, float32_t* BB1);

    void ofCreatBB(Rect<float32_t> &ofCurrBB, int32_t imgWidth, int32_t imgHeight, float32_t x, float32_t y, float32_t w, float32_t h);
    
    //member variables 
private:    

    OFFBRobustTrackerParams params;
    qcvMemory   scratchbuff;

    ImageExtension *prevFrame;
    ImageExtension *currFrame;

    //Motion Analysis Module
    MotionAnalyzer M;

#ifdef  OFFB_ROBUST_DEBUG   
    int32_t frameID;
#endif

};


}

#endif  //qcv_offbrobustTracker_h_
