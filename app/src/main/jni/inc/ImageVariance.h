/**=============================================================================

@file
ImageVariance.h

@brief
Extract the variance of an image patch using integral images

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

#ifndef qcv_image_variance_h_
#define qcv_image_variance_h_


#include <cstddef>
#include <vector>

#include "qcvMath.h"
#include "qcvTypes.h"
#include "qcvImage.h"
#include "qcvImageExtension.h"
#include "qcvImageGray.h"
#include "qcvImagePlanarUtils.h"
#include "ImageFeatureExtractor.h"

namespace qcv
{

/**************************************************************
compute the variance of an gray uint8_t image patch
Limitations: 
1. While in general the brief feature can be on any type of images, we only consider uint8_t input for now
2. we only consider the first channel of the input image
*****************************************************************/
class ImageVarianceFeature : public ImageFeatureExtractor < float32_t, uint32_t>
{
    
public :
    ImageVarianceFeature(const Size2D<uint32_t> &size) 
    {
        winSize = size;
    }


    virtual ~ImageVarianceFeature(){ }

    //public functions

    //the length of the feature
    uint32_t getFeatureLength(void) const {return 1;}

    Size2D<uint32_t> getWindowSize(void) const 
    {
        return winSize;
    }

    void setWindowSize(const Size2D<uint32_t> &size) 
    {
        winSize = size;
    }


    //extract the variance of an image patch
    bool extractImageFeature(ImageExtension &imgExt,  const Point2D<uint32_t> &locXY, 
        FeatureVectorView<float32_t> &featureVectorOutput)
    {
        assert(featureVectorOutput.getLength()==1); 
        if (featureVectorOutput.getLength() != 1)
        {
            return false;
        }
                
        //get the image view 
        PlanarView imgView = imgExt.getImageView( );    

        //get the integral image pointers 
        uint32_t imStride = (uint32_t) ImagePlanarUtils::getStride(imgView, imgExt.chIdxPydInteg); 
        uint32_t *iimg;
        uint64_t *iimg2;    
        imgExt.getIntegralImage2(iimg, iimg2);

        featureVectorOutput[0] = patchVariance(iimg, iimg2, locXY, imStride);
        
        return true;
    }
    
private: 

    //compute the variance of the pixels inside a window using integral image
    float32_t patchVariance(uint32_t *ii, uint64_t *ii2, const Point2D<uint32_t> &locXY, uint32_t stride) 
    {		
        uint32_t offset = locXY.y * stride + locXY.x;
	    // winRect = [x, y, width, height]
        uint32_t ul = offset; 
        uint32_t ur = ul + winSize.w-1;
        uint32_t ll = ul + (winSize.h-1)*stride;
        uint32_t lr = ll + winSize.w-1; 
        float32_t area = (float32_t)(winSize.w-1)*(winSize.h-1);

	    float32_t mX  = (float32_t)(ii[lr] + ii[ul] - ii[ll] - ii[ur] ) / area;        
	    float32_t mX2 = (float32_t)(ii2[lr] + ii2[ul] - ii2[ll] - ii2[ur] ) / area;

        assert(mX>=0 && mX2>=0);

        return mX2 - mX*mX;   
    }

private: 
    //variables
    Size2D<uint32_t> winSize;

};

}   // namespace qcv

#endif // qcv_image_variance_h_
//


