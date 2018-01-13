/**=============================================================================

@file
ImageQuantizerND.h

@brief
Quantize an (multi-channel) image patch 

Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

#ifndef qcv_image_quantizer_h_
#define qcv_image_quantizer_h_


#include <cstddef>
#include <vector>

#include "qcvMath.h"
#include "qcvTypes.h"
#include "qcvMemory.h"
#include "qcvImage.h"
#include "qcvImageExtension.h"
#include "qcvImageGray.h"
#include "qcvImagePlanarUtils.h"
#include "ImageFeatureExtractor.h"

namespace qcv
{


class ImageQuantizerNDParam
{
public:
    ImageQuantizerNDParam(uint32_t nChan, float32_t *minmax, uint32_t *bins) :
        numChan(nChan)
    {
        minmaxValPerChan = (float32_t *)MEMALLOC(2 * sizeof(float32_t)*numChan, 16);
        std::memcpy(minmaxValPerChan, minmax, 2 * sizeof(float32_t)*numChan);
        binsPerChan = (uint32_t *)MEMALLOC(sizeof(uint32_t)*numChan, 16);
        std::memcpy(binsPerChan, bins, sizeof(uint32_t)*numChan);
    }

    ImageQuantizerNDParam(const ImageQuantizerNDParam &param) 
    {
        numChan = param.numChan;
        minmaxValPerChan = (float32_t *)MEMALLOC(2 * sizeof(float32_t)*numChan, 16);
        std::memcpy(minmaxValPerChan, param.minmaxValPerChan, 2 * sizeof(float32_t)*numChan);
        binsPerChan = (uint32_t *)MEMALLOC(sizeof(uint32_t)*numChan, 16);
        std::memcpy(binsPerChan, param.binsPerChan, sizeof(uint32_t)*numChan);
    }

    virtual ~ImageQuantizerNDParam()
    {
        if (minmaxValPerChan)
        {
            MEMFREE(minmaxValPerChan);
            minmaxValPerChan = NULL;
        }

        if (binsPerChan)
        {
            MEMFREE(binsPerChan);
            binsPerChan = NULL;
        }
    }

    uint32_t numChan;
    uint32_t *binsPerChan; //num of bins each channel, assuming the maximum is 10 channels
    //the range of each channel
    float32_t *minmaxValPerChan; //the min and max values for each channel; min0, max0, min1, max1, ...   
};
/**************************************************************
quantize a (multi-channel) input image patch into histogram bins 
*****************************************************************/
template<class _outType>   
class ImageQuantizerND : public ImageFeatureExtractor < _outType, uint32_t>
{
    
public :
    //minmax - the min and max values for each channel; min0, max0, min1, max1, ...
    //bins - num of bins each channel, assuming the maximum is 10 channels
    ImageQuantizerND(const Size2D<uint32_t> &size, const ImageQuantizerNDParam &params) :
        param(params)
    {
        winSize = size;

        //the base (multiplication to bins) of bins at each channel
        binBaseAtChan = (uint32_t *)MEMALLOC(sizeof(uint32_t)*param.numChan, 16);
        binStepPerChan = (float32_t *)MEMALLOC(sizeof(float32_t)*param.numChan, 16);
        for (uint32_t i = 0; i < param.numChan; i++)
        {
            if (i == 0)
                binBaseAtChan[i] = 1;
            else
                binBaseAtChan[i] = param.binsPerChan[i - 1] * binBaseAtChan[i - 1];

            binStepPerChan[i] = (param.minmaxValPerChan[2 * i + 1] - param.minmaxValPerChan[2 * i]+1) / (float32_t)param.binsPerChan[i];
        }
    }

    virtual ~ImageQuantizerND()
    { 

        if (binBaseAtChan)
        {
            MEMFREE(binBaseAtChan);
            binBaseAtChan = NULL;
        }

        if (binStepPerChan)
        {
            MEMFREE(binStepPerChan);
            binStepPerChan = NULL;
        }        
    }

    //public functions

    //the length of the feature
    uint32_t getFeatureLength(void) const 
    {
        //the output is the length of the image patch size
        return winSize.w*winSize.h;       
    }

    Size2D<uint32_t> getWindowSize(void) const 
    {
        return winSize;
    }

    void setWindowSize(const Size2D<uint32_t> &size) 
    {
        winSize = size;
    }

    //quantize the multi-channel image 
    bool extractImageFeature(ImageExtension &imgExt, const Point2D<uint32_t> &locXY,
        FeatureVectorView<_outType> &featureVectorOutput);
    
    //get the number of quantization levels
    uint32_t getQuantizationLevels() const;
    

private: 
    //variables
    Size2D<uint32_t> winSize;

    //parameters
    ImageQuantizerNDParam param;

    uint32_t *binBaseAtChan; //the base (multiplication to bins) of bins at each channel
    float32_t *binStepPerChan;     //the binstep for each channel
    
};



//get the number of quantization levels
template<class _outType>
uint32_t ImageQuantizerND<_outType>::getQuantizationLevels() const
{
    uint32_t numLevels = 1;
    for (uint32_t i = 0; i < param.numChan; i++)
        numLevels *= param.binsPerChan[i];
    return numLevels;
}

//quantize the multi-channel image 
template<class _outType>
bool ImageQuantizerND<_outType>::extractImageFeature(ImageExtension &imgExt, const Point2D<uint32_t> &locXY,
FeatureVectorView<_outType> &featureVectorOutput)
{
    //get the image view 
    PlanarView imgView = imgExt.getImageView();
    uint32_t  nChan= imgExt.getNumChannels();
    uint32_t imgWidth = imgExt.imgWidth;
    uint32_t imgHeight = imgExt.imgHeight;
    uint32_t imStride = imgExt.imgStride;

    //check the sizes
    assert(featureVectorOutput.getLength() == getFeatureLength() && nChan == param.numChan);
    if (featureVectorOutput.getLength() != getFeatureLength() || nChan != param.numChan)
    {
        PRINTF("ImageQuantizerND<_outType>::extractImageFeature: Error! Different Feature Length or number of Channels!\n");
        return false;
    }
    
    //check if the windiw is outside of the image
    assert(locXY.x >= 0 && locXY.y >= 0 && locXY.x + winSize.w <= imgWidth && locXY.y + winSize.h <= imgHeight);
    if (!(locXY.x >= 0 && locXY.y >= 0 && locXY.x + winSize.w <= imgWidth && locXY.y + winSize.h <= imgHeight))
    {
        PRINTF("ImageQuantizerND<_outType>::extractImageFeature: Error! window outside of the image!\n");
        return false;
    }

    _outType *outPt = featureVectorOutput.getData();
    uint8_t **planePtr = (uint8_t **)MEMALLOC(param.numChan * sizeof (uint8_t*), 16);
    
    for (uint32_t i = 0; i < param.numChan; i++)
        planePtr[i] = imgExt.getImage(i) + locXY.y * imStride + locXY.x;

    //loop through the image to quantize each pixel 
    for (uint32_t i = 0; i < winSize.h; i++)       
    {
        for (uint32_t j = 0; j < winSize.w; j++)
        {
            //quantize the pixels 
            _outType out = 0;
            for (uint32_t ch = 0; ch < param.numChan; ch++)
            {
                uint32_t binIdx = (uint32_t) MIN(MAX(((float32_t)*(planePtr[ch]) - param.minmaxValPerChan[2 * ch]) / binStepPerChan[ch], 0), param.binsPerChan[ch] - 1);
                out += binBaseAtChan[ch] * binIdx;
                planePtr[ch]++;
            }
            *outPt++ = out;                
        }
        for (uint32_t ch = 0; ch < param.numChan; ch++)
            planePtr[ch] += imStride - winSize.w;
    }

    MEMFREE(planePtr);

    return true;
}



}   // namespace qcv

#endif // qcv_image_quantizer_h_
//


