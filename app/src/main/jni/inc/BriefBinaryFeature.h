/**=============================================================================

@file
BriefBinaryFeature.h

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

#ifndef qcv_briefbinaryfeature_h_
#define qcv_briefbinaryfeature_h_


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

#include "qcvMemory.h"
#include "qcvUtils.h"
#include "qcvImagePlanarUtils.h"


namespace qcv
{

/**************************************************************
The feature length is the same as the number of pixel pairs, i.e. numPixelPair 
The output feature is a vector of binary outputs
Remarks:
1. While in general the brief feature can be on any type of images, we only consider uint8_t input for now
2. only extract featuers from a single channel defined by "channelIndex" 
*****************************************************************/
template<class _DataType> //output data type
class BriefBinaryFeature : public ImageFeatureExtractor <_DataType, uint32_t>
{
    
public :
    BriefBinaryFeature(Size2D<uint32_t> winSize, uint32_t winStride,                 
                uint32_t channelIndex); //no memory allocation 

    BriefBinaryFeature(Size2D<uint32_t> winSize, uint32_t winStride, 
                uint32_t numPixelPair, const float32_t *pPairLocXY, 
                uint32_t channelIndex);  

    BriefBinaryFeature(Size2D<uint32_t> winSize, uint32_t winStride, 
                uint32_t numPixelPair, const float32_t *pPairLocXY, 
                const bool *inactiveFlag, uint32_t channelIndex);  

    ~BriefBinaryFeature();

    //public functions
public:   

    //reset the pixel pair locations (can be different number of pairs)
    void setPairLocXY(const float32_t *pPairLocXY, uint32_t numPixelPair);
    
    //reset the pixel pair locations (can be different number of pairs) with inactive feautre flags
    void setPairLocXY(const float32_t *pPairLocXY, uint32_t numPixelPairTotal, const bool *inactiveFlag);

    //extract the feature from an image        
    //*Params: 
    //      offset: the offset pointer relative to the input image pointer provided by ImgExt        
    bool extractImageFeature(ImageExtension &imgExt,  const Point2D<uint32_t> &locXY, 
        FeatureVectorView<_DataType> &featureVectorOutput);

    //the length of the feature
    uint32_t getFeatureLength(void) const {return numPixelPair;}

    // get/set the stride and re-compute the offset the image 
    uint32_t getWindowStride(void) const {return winStride;}
    void setWindowStride(uint32_t stride);

    Size2D<uint32_t> getWindowSize(void) const {return winSize;}
    void setWindowSize(const Size2D<uint32_t> &size);     

private:
    /////////////////////////////////////////////
    //internal functions

    //clear the memory
    void clear();

    void computeFeatureOffset(void);
            
    /////////////////////////////////////////////
    //internal member variables
    //the stride of the image that the window will be operate. 
    //This is important since the featur offset will be computed based on the offset
    Size2D<uint32_t> winSize;
    uint32_t winStride; 
            
    uint32_t numPixelPair; //number of pixel pairs, i.e output feature leangth
    float32_t *pixelPairLocXY; //the x, y relative locations for the pixel pairs
    uint32_t *pixelPairOffset; //the pixel offset in the image with respect to the uppler left pointer of the bounding box 

    uint32_t chanIdx; //the channel to extract feature 

    //scratch buffer for memory allocation 
    qcvMemory scratchBuffer;
            
};


///////////////////////////////////////////////////////////////
//  Class Definition
//////////////////////////////////////////////////////////////


template<class _DataType> 
BriefBinaryFeature<_DataType>::BriefBinaryFeature(Size2D<uint32_t> winSize, uint32_t winStride,                 
                uint32_t channelIndex) : //no memory allocation 
    winSize(winSize),         
    winStride(winStride),
    numPixelPair(0),    
    pixelPairLocXY(NULL),
    pixelPairOffset(NULL),
    chanIdx(channelIndex)
{

}

template<class _DataType> 
BriefBinaryFeature<_DataType>::BriefBinaryFeature(Size2D<uint32_t> winSize, uint32_t winStride, 
                uint32_t numPixelPair, const float32_t *pPairLocXY, uint32_t channelIndex) :     
    winSize(winSize),         
    winStride(winStride),
    numPixelPair(numPixelPair),    
    pixelPairOffset(NULL),
    chanIdx(channelIndex)
{
    //allocate scratch buffer memory
    uint32_t bufferSize = numPixelPair*4*sizeof(float32_t) + numPixelPair * 2 * sizeof(uint32_t);
    scratchBuffer.setSize(bufferSize * 2, 16); //give 2 times buffer just in case
    pixelPairLocXY = (float32_t *)scratchBuffer.reserveMem(numPixelPair*4*sizeof(float32_t));
    std::memcpy(pixelPairLocXY, pPairLocXY, numPixelPair*4*sizeof(float32_t));
    computeFeatureOffset();
}

template<class _DataType> 
BriefBinaryFeature<_DataType>::BriefBinaryFeature(Size2D<uint32_t> winSize, uint32_t winStride, 
                uint32_t numPixelPairTotal, const float32_t *pPairLocXY, const bool *inactiveFlag, uint32_t channelIndex) :     
    winSize(winSize),         
    winStride(winStride),    
    pixelPairOffset(NULL),
    chanIdx(channelIndex)
{
    //allocate scratch buffer memory
    uint32_t bufferSize = numPixelPairTotal*4*sizeof(float32_t) + numPixelPairTotal * 2 * sizeof(uint32_t);
    scratchBuffer.setSize(bufferSize * 2, 16); //give 2 times buffer just in case
    //allocate size as the full length, but set only activated features
    pixelPairLocXY = (float32_t *)scratchBuffer.reserveMem(numPixelPairTotal*4*sizeof(float32_t));    
    numPixelPair = 0;   
    const float32_t *pPairLocXYOrig = pPairLocXY;
    float32_t *pixelPairLocXYCur = pixelPairLocXY;
    for (uint32_t i=0; i < numPixelPairTotal; i++)
    {
        if (!inactiveFlag[i])
        {
            std::memcpy(pixelPairLocXYCur, pPairLocXYOrig, 4*sizeof(float32_t));
            pixelPairLocXYCur += 4;
            numPixelPair++;
        }
        pPairLocXYOrig += 4;
    }
    computeFeatureOffset();
}




//template <class ImgType, uint32_t _featLen>
template<class _DataType> 
BriefBinaryFeature<_DataType>::~BriefBinaryFeature()
{
    //destruction operations
    clear();
}


template<class _DataType> 
void BriefBinaryFeature<_DataType>::clear()
{
    //destruction operations
    scratchBuffer.resetMem();
    pixelPairLocXY = NULL;
    pixelPairOffset = NULL;    
}

//reset the pixel pair locations (can be different number of pairs)
template<class _DataType> 
void BriefBinaryFeature<_DataType>::setPairLocXY(const float32_t *pPairLocXY, uint32_t numPair)
{
    if (numPair != numPixelPair)
    {
        //reset memory
        clear();
        numPixelPair = numPair;
        uint32_t bufferSize = numPixelPair*4*sizeof(float32_t) + numPixelPair * 2 * sizeof(uint32_t);
        //increase the scratch buffer if necessary
        if (scratchBuffer.getRemainingSize() < bufferSize)
            scratchBuffer.setSize(bufferSize * 2, 16);        
        pixelPairLocXY = (float32_t *)scratchBuffer.reserveMem(numPixelPair*4*sizeof(float32_t));
    }
    std::memcpy(pixelPairLocXY, pPairLocXY, numPixelPair*4*sizeof(float32_t));
    computeFeatureOffset();
}



//reset the pixel pair locations (can be different number of pairs) with inactive feautre flags
template<class _DataType> 
void BriefBinaryFeature<_DataType>::setPairLocXY(const float32_t *pPairLocXY, uint32_t numPixelPairTotal, const bool *inactiveFlag)
{
    if (numPixelPairTotal != numPixelPair)
    {   
        //need to reallocate memory 
        clear();
        uint32_t bufferSize = numPixelPairTotal*4*sizeof(float32_t) + numPixelPairTotal * 2 * sizeof(uint32_t);
        //increase the scratch buffer if necessary
        if (scratchBuffer.getRemainingSize() < bufferSize)
            scratchBuffer.setSize(bufferSize * 2, 16);    
        //allocate size as the full length, but set only activated features
        pixelPairLocXY = (float32_t *)scratchBuffer.reserveMem(numPixelPairTotal*4*sizeof(float32_t));            
    }
    numPixelPair = 0;
    const float32_t *pPairLocXYOrig = pPairLocXY;
    float32_t *pixelPairLocXYCur = pixelPairLocXY;
    for (uint32_t i=0; i < numPixelPairTotal; i++)
    {
        if (!inactiveFlag[i])
        {
            std::memcpy(pixelPairLocXYCur, pPairLocXYOrig, 4*sizeof(float32_t));
            pixelPairLocXYCur += 4;
            numPixelPair++;
        }
        pPairLocXYOrig += 4;
    }
    computeFeatureOffset();
    
}


// set the stride and re-compute the offset the image 
template<class _DataType> 
void BriefBinaryFeature<_DataType>::setWindowStride(const uint32_t stride)
{
    if (stride != winStride)
    {
        winStride = stride;
        computeFeatureOffset();
    }
}


template<class _DataType> 
void BriefBinaryFeature<_DataType>::setWindowSize(const Size2D<uint32_t> &size)
{
    if ( size.w!=winSize.w || size.h!= winSize.h)
    {
        winSize = size;
        computeFeatureOffset();
    }
}

//compute feature offset
template<class _DataType> 
void BriefBinaryFeature<_DataType>::computeFeatureOffset(void)
{
    //allocate memory if necessary
    if (!pixelPairOffset)        
        pixelPairOffset = (uint32_t *)scratchBuffer.reserveMem(numPixelPair * 2 * sizeof(uint32_t));
    
    const float32_t *pLocXY = pixelPairLocXY;
    for(uint32_t i = 0; i < numPixelPair * 2; i++)
    {        
        pixelPairOffset[i] = LOCATION( (winSize.w-1) * pLocXY[0], (winSize.h-1) * pLocXY[1], winStride);           
        pLocXY += 2;
    }
}

//extract the feature from an image
//we only consider the first channel of the input image
template<class _DataType> 
bool BriefBinaryFeature<_DataType>::extractImageFeature(ImageExtension &imgExt,  const Point2D<uint32_t> &locXY, 
        FeatureVectorView<_DataType> &featureVectorOutput)
{
    //check the input length is the same as feature length
    assert(featureVectorOutput.getLength()== numPixelPair); 

    //get the image view 
    PlanarView imgView = imgExt.getImageView( );
    
    //check the image is unit8 and has the right channel
    assert(chanIdx < imgView.getChannelCount() && ImagePlanarUtils::isUint8(imgView, chanIdx) );
   
    //get image stride
    uint32_t imStride = (uint32_t) ImagePlanarUtils::getStride(imgView, chanIdx);
    setWindowStride(imStride);

    //get the image pointer and add the offset     
    uint8_t *imgPtr = ImagePlanarUtils::getPlanePtru8(imgView, chanIdx) + (locXY.y * imStride + locXY.x);
    
    //extract image features from the image patch 
    uint32_t *featOffset = pixelPairOffset;
    for (uint32_t i=0; i< numPixelPair; i++)
    {        
        if(imgPtr[featOffset[0]] > imgPtr[featOffset[1]])
            featureVectorOutput[i]  = (_DataType) 1;
        else 
            featureVectorOutput[i] = (_DataType) 0;
        featOffset += 2;
    }
    return true;
}


}   // namespace qcv

#endif // qcv_briefbinaryfeature_h_
//


