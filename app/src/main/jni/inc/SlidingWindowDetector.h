/**=============================================================================

@file
SlidingWindowDetector.hpp

@brief
Internal header file for sliding window detector.

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

#ifndef SLIDING_WINDOW_DETECTOR_H
#define SLIDING_WINDOW_DETECTOR_H

#include "ImageDetector.h"
#include "ImagePatchClassifier.h"
#include "qcvTypes.h"
#include "qcvImage.h"
#include "qcvImageGray.h"
#include "qcvImageExtension.h"
#include "qcvImagePyramidGaussian.h"

#include <math.h>
#include <cassert>
#include "fastcv.h"

using namespace std;

namespace qcv
{


//---------------------------------------------------------------------------------------------
/// @brief 
///    qcvlib::SlidingWindowDetector class that implements sliding window detector
///    
/// @details
///    qcvlib::SlidingWindowDetector class is a C++ implementation for sliding window detector
///
///    1. It provides a static constructor for creating a new instance of the sliding  
///       window detector This will initialize the detector with the provided parameters.
///    2. It provides a detector function that returns a vector of detection results.
///    3. Provides a utility function that 
//---------------------------------------------------------------------------------------------

class SlidingWindowDetector : public Detector
{  
public:
    //------------------------------------------------------------------------------
    /// @brief
    //------------------------------------------------------------------------------
    SlidingWindowDetector( ImagePatchClassifier<uint32_t> & patchClassifier, const Size2D<uint32_t> &shiftBB,
    uint32_t maxDetectedObjNum = -1, uint8_t balaceWin=false);
  
    //------------------------------------------------------------------------------
    /// @brief
    //------------------------------------------------------------------------------
    ~SlidingWindowDetector();
  
    //set the number of windows to skip between steps
    void setScanWindowStep(uint32_t scanStep, uint32_t scanOffsetX=0, uint32_t scanOffsetY=0); 

    //------------------------------------------------------------------------------
    /// @brief  detect object on an image
    //------------------------------------------------------------------------------
    void detect(ImageExtension &imgExt, const Rect<uint32_t> &roi, std::vector<DetectionResult> &result);
  
    //------------------------------------------------------------------------------
    /// @brief detect object on an image gaussian pyramid
    //------------------------------------------------------------------------------
    void detect(ImagePyramidGaussian &pyramid, const Rect<uint32_t> &roi, Size2D<uint32_t> bbScanSizeMin, 
                Size2D<uint32_t> bbScanSizeMax, std::vector<DetectionScaleResult> &result);
  
    
private:
  
    //sliding window detector parameters
    Size2D<uint32_t>                     shiftBB;                   // Shift of BB over ROI
    uint32_t                             scanWinSteps;              // how many window to skip between steps
    uint32_t                             scanWinOffsetX;             // number of windows to skip when starting the scan 
    uint32_t                             scanWinOffsetY;             // number of windows to skip when starting the scan 
    
    uint8_t                              balanceWinInOctaveBelowIdx;     //balance sliding window in the octave below this value such that they all have same amount of window to scan
    uint32_t                             maxDetectedObjNum;         // Limit to objects detected
    
    //Size2D<uint32_t>       	         bbScanSizeMin;             // Minimum BB used for scanning
    //Size2D<uint32_t>                   bbScanSizeMax;             // Maximum BB used for scanning

    ImagePatchClassifier <uint32_t> &               patchClassifier;


    

}; //class SlidingWindowDetector

} //namespace qcv

#endif //SLIDING_WINDOW_DETECTOR_H




