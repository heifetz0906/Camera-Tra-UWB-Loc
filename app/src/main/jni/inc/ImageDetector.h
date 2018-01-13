/**=============================================================================

@file
ImageDetector.h

@brief
Definition of a base class for an image detector. 
The class finds candidate locations of an object on an image.

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

#ifndef qcv_imagedetector_h_
#define qcv_imagedetector_h_


#include <cstddef>
//#include <stdexcept>
#include <vector>
#include <cstring>

#include "qcvTypes.h"
#include "qcvRect.h"
#include "qcvImage.h"
#include "qcvImageExtension.h"

namespace qcv
{

//data structure of an object location 
class DetectionResult
{
public:
    DetectionResult( ) : confidence(0) { };

    DetectionResult &average(const DetectionResult &r, bool roundXY, bool roundWH)
    {
        rect = Rect<float32_t>::average(rect, confidence, r.rect, r.confidence, true, roundXY, roundWH);
        //confidence = (confidence + r.confidence)/2.0f;
        confidence = MAX(confidence,  r.confidence);
        return *this;
    }

    static DetectionResult average(const DetectionResult &r1, const DetectionResult &r2, bool roundXY, bool roundWH) 
    {
        DetectionResult avgResult(r1);
        avgResult.average(r2, roundXY, roundWH);
        return avgResult;
    }

    //comparison function 
    inline bool operator< (const DetectionResult &b) const
    { 
        return (confidence > b.confidence);
    }

    static bool order(const DetectionResult &a, const DetectionResult &b) 
    { 
        return (a.confidence > b.confidence);
    }

public:
	Rect<float32_t> rect;
	float32_t confidence;   
};

//data structure of an object location 
struct DetectionScaleResult
{
	DetectionResult result; //on the original scale 
    Rect<float32_t> scaleRect; 
    uint8_t scaleLevel;
    float32_t absScaleFactorX;
    float32_t absScaleFactorY;
    
    //comparison function 
    inline bool operator< (const DetectionScaleResult &b) 
    { 
        return (result.confidence > b.result.confidence);
    }
    
    static bool order(const DetectionScaleResult &a, const DetectionScaleResult &b) 
    { 
        return (a.result.confidence > b.result.confidence);
    }

};

//Base class (abstract) for an image detector  
//The class finds candidate locations of an object on an image.
class Detector
{
    
public :
    
    virtual ~Detector(){ }
        
    //public member functions     
    //------------------------------------------------------------------------------
    /// @brief  detect object on an image
    //------------------------------------------------------------------------------
    virtual void detect(ImageExtension &imgExt, const Rect<uint32_t> &roi, std::vector<DetectionResult> &result) = 0;
  
    //------------------------------------------------------------------------------
    /// @brief detect object on an image gaussian pyramid
    //------------------------------------------------------------------------------
    virtual void detect(ImagePyramidGaussian &pyramid, const Rect<uint32_t> &roi, Size2D<uint32_t> bbScanSizeMin, Size2D<uint32_t> bbScanSizeMax,
        std::vector<DetectionScaleResult> &result) = 0;
};

}   // namespace qcv

#endif // qcv_imagedetector_h_
//


