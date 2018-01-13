/**=============================================================================

@file
BriefFeature.h

@brief
Implementation of the BRIEF feature, which is a binary feature that compares the intensity of 
a pair of pixels. It is derived from FeatureVector
Reference: 
BRIEF: Binary Robust Independent Elementary Features, Michael Calonder, Vincent Lepetit, Christoph Strecha, 
Pascal Fua, ECCV 2010

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

#ifndef qcv_brieffeature_h_
#define qcv_brieffeature_h_


#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cstring>

#include "qcvMath.h"
#include "qcvTypes.h"
#include "qcvImage.h"
#include "qcvImageExtension.h"
#include "Feature.h"
#include "ImageFeatureExtractor.h"

namespace qcv
{

/**************************************************************
each brief feature has length of one, so feature length is the number of Brief features can be extracted
Hence, the total number pixel pairs = _featLen * numPixelPair 
Limitations: 
1. While in general the brief feature can be on any type of images, we only consider uint8_t input for now
2. we only consider the first channel of the input image
*****************************************************************/
class BriefFeature : public ImageFeatureExtractor <uint32_t, uint32_t>
{
    
public :
    BriefFeature(uint32_t width, uint32_t height, uint32_t stride, 
                uint32_t numBriefFeature, uint32_t numPixelPairPerFeature, const float32_t *pPairLocXY);
    ~BriefFeature();

    //public functions
public:   
    //extract the feature from an image        
    //*Params: 
    //      offset: the offset pointer relative to the input image pointer provided by ImgExt        
    bool extractImageFeature(ImageExtension &imgExt,  const Point2D<uint32_t> &locXY, 
        FeatureVectorView<uint32_t> &featureVectorOutput);

    //the length of the feature
    uint32_t getFeatureLength(void) const {return numFeature;}

    // get/set the stride and re-compute the offset the image 
    uint32_t getWindowStride(void) const {return winStride;}
    void setWindowStride(uint32_t stride);

    Size2D<uint32_t> getWindowSize(void) const;
    void setWindowSize(const Size2D<uint32_t> &size);     

private:
    /////////////////////////////////////////////
    //internal functions
    void computeFeatureOffset(void);
        
    uint32_t calculateFeature(const uint8_t *imgPatch, const uint32_t *featOffset) const;
    
    /////////////////////////////////////////////
    //internal member variables
    //the stride of the image that the window will be operate. 
    //This is important since the featur offset will be computed based on the offset
    uint32_t winWidth;
    uint32_t winHeight;
    uint32_t winStride; 
            
    uint32_t numPairPerFeature; //number of pixel pairs per feature
    uint32_t numFeature;    //number of features to be extracted each time
    uint32_t totalPixelPair; //total number pixel pairs = _featLen * numPixelPair
    const float32_t *pixelPairLocXY; //the x, y relative locations for the pixel pairs
    uint32_t *pixelPairOffset; //the pixel offset in the image with respect to the uppler left pointer of the bounding box 
            
};

}   // namespace qcv

#endif // qcv_classifier_h_
//


