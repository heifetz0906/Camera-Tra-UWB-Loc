/**=============================================================================

@file
Feature.h

@brief
Definition of Feature class, which will be a base class for different kind of features. 
Written in C++ 

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

#ifndef qcv_image_feature_extractor_h_
#define qcv_image_feature_extractor_h_


#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <memory.h>

#include "qcvTypes.h"
#include "qcvImage.h"
#include "qcvImageExtension.h"
#include "Feature.h"

namespace qcv
{


//Feature extractor for an image
//we assume that all image feature extractor is based on a window size, even if it is a single pixel
//the feature extractor doesn't provide data buffer, so the user need to manage their own data buffer 
//the length of the data buffer is give by getFeatLength()
template<class _DataType, class _LocType = uint32_t> 
class ImageFeatureExtractor
{
    
public :
    virtual ~ImageFeatureExtractor() {}

    //the length of the feature
    virtual uint32_t getFeatureLength(void) const =0; 
       
    //extract the feature from an image 
    //*Params: 
    //  locXY gives the upper-left conrer of the patch     
    virtual bool extractImageFeature(ImageExtension &imgExt,  const Point2D<_LocType> &locXY, 
        FeatureVectorView<_DataType> &featureVectorOutput) = 0;

    //public API to get window width and height
    virtual Size2D<_LocType> getWindowSize(void) const = 0;
    
    //set window size
    virtual void setWindowSize(const Size2D<_LocType> &size) =0;         
};




}   // namespace qcv

#endif // qcv_image_feature_extractor_h_
//

