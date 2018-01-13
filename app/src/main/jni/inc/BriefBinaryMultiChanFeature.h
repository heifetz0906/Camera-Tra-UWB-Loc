/**=============================================================================

@file
BriefFeature.h

@brief
Implementation of the BRIEF feature for multiple color channels
BRIEF is is a binary feature that compares the intensity of 
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

#ifndef qcv_briefbinarychanfeature_h_
#define qcv_briefbinarychanfeature_h_


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
#include "BriefBinaryFeature.h"

namespace qcv
{

/**************************************************************
a multi-channel BRIEF Feature extractor
the output feature vector is the stacking of the binary features eatracted from each channel. 
each channel may have different number of pixel pairs
*****************************************************************/
template<class _DataType> //output data type
class BriefBinaryMultiChanFeature : public ImageFeatureExtractor < _DataType, uint32_t>
{
    
public :    

    //no pixel pair location provided yet 
    BriefBinaryMultiChanFeature(Size2D<uint32_t> winSize, uint32_t winStride, uint32_t numChannels) :
        numChan(numChannels)
    {
        numFeature = 0; 
        //allocation BRIEF extractors for each channel
        briefExtractors = (BriefBinaryFeature<_DataType> **)MEMALLOC(numChan*sizeof(BriefBinaryFeature<_DataType> *), 16);
        for (uint32_t i=0; i<numChan; i++)
            briefExtractors[i] = new BriefBinaryFeature<_DataType>(winSize, winStride, i);        
    }

    //same number of pixel pairs for each channel and same pixel locations 
    BriefBinaryMultiChanFeature(Size2D<uint32_t> winSize, uint32_t winStride,
                uint32_t numChannels, const uint32_t numPixelPairPerChan, const float32_t *pPairLocXYPerChan) :
        numChan(numChannels)
    {
        numFeature = numPixelPairPerChan * numChannels;
        //allocation BRIEF extractors for each channel
        briefExtractors = (BriefBinaryFeature<_DataType> **)MEMALLOC(numChan*sizeof(BriefBinaryFeature<_DataType> *), 16);
        for (uint32_t i=0; i<numChan; i++)
            briefExtractors[i] = new BriefBinaryFeature<_DataType>(winSize, winStride, numPixelPairPerChan, pPairLocXYPerChan, i);        
    }

    //same number of pixel pairs for each channel and same pixel location, but with a boolean indicator of whether a feature is inactive
    BriefBinaryMultiChanFeature(Size2D<uint32_t> winSize, uint32_t winStride,
                uint32_t numChannels, const uint32_t numPixelPairPerChan, const float32_t *pPairLocXYPerChan, const bool *inactiveFlag) :
        numChan(numChannels)
    {
        numFeature = 0;
        //allocation BRIEF extractors for each channel
        briefExtractors = (BriefBinaryFeature<_DataType> **)MEMALLOC(numChan*sizeof(BriefBinaryFeature<_DataType> *), 16);
        const bool *inactiveFlagChan = inactiveFlag;
        for (uint32_t i=0; i<numChan; i++)
        {
            briefExtractors[i] = new BriefBinaryFeature<_DataType>(winSize, winStride, numPixelPairPerChan, pPairLocXYPerChan, inactiveFlagChan, i);        
            numFeature += briefExtractors[i]->getFeatureLength();
            inactiveFlagChan += numPixelPairPerChan;
        }
    }



    //different pixel pairs for each channel, and same pixel locations 
    BriefBinaryMultiChanFeature(Size2D<uint32_t> winSize, uint32_t winStride,
                uint32_t numChannels, const uint32_t *numPixelPairPerChan, const float32_t **pPairLocXYPerChan) :
    numChan(numChannels)
    {
        numFeature = 0; 
        //allocation BRIEF extractors for each channel
        briefExtractors = (BriefBinaryFeature<_DataType> **)MEMALLOC(numChan*sizeof(BriefBinaryFeature<_DataType> *), 16);
        for (uint32_t i=0; i<numChan; i++)
        {
            briefExtractors[i] = new BriefBinaryFeature<_DataType>(winSize, winStride, numPixelPairPerChan[i], pPairLocXYPerChan[i], i); 
            numFeature += numPixelPairPerChan[i];
        }

    }
    
    ~BriefBinaryMultiChanFeature()
    {        
        if (briefExtractors)
        {
            for (uint32_t i=0; i<numChan; i++)
                    delete briefExtractors[i];
            MEMFREE(briefExtractors);
            briefExtractors = NULL;
        }        
    }

    //public functions
public:   

    //reset the pixel pair locations for the a channel
    void setPairLocXY(const float32_t *pPairLocXY, uint32_t numPixelPair, uint32_t chanIdx)
    {
        assert(chanIdx < numChan);
        numFeature = numFeature - briefExtractors[chanIdx]->getFeatureLength() + numPixelPair;
        briefExtractors[chanIdx]->setPairLocXY(pPairLocXY, numPixelPair);
    }

    //reset the pixel pair locations for the a channel with inactive feature flags
    void setPairLocXY(const float32_t *pPairLocXY, uint32_t numPixelPair, uint32_t chanIdx, const bool *inactiveFlag)
    {
        assert(chanIdx < numChan);        
        numFeature -= briefExtractors[chanIdx]->getFeatureLength();
        briefExtractors[chanIdx]->setPairLocXY(pPairLocXY, numPixelPair, inactiveFlag );
        numFeature += briefExtractors[chanIdx]->getFeatureLength();
    }

    //reset the pixel pair locations for all channels with inactive feature flags
    void setPairLocXY(const float32_t *pPairLocXY, uint32_t numPixelPairPerChan, const bool *inactiveFlag)
    {
        numFeature = 0;
        const bool *inactiveFlagChan = inactiveFlag;
        for (uint32_t chanIdx = 0; chanIdx < numChan; chanIdx++)
        {
            briefExtractors[chanIdx]->setPairLocXY(pPairLocXY, numPixelPairPerChan, inactiveFlagChan);
            numFeature += briefExtractors[chanIdx]->getFeatureLength();
            inactiveFlagChan += numPixelPairPerChan;
        }
    }

    //extract the feature from an image        
    //*Params: 
    //      offset: the offset pointer relative to the input image pointer provided by ImgExt        
    bool extractImageFeature(ImageExtension &imgExt,  const Point2D<uint32_t> &locXY, 
        FeatureVectorView<_DataType> &featureVectorOutput)
    {
        //eatract from each feature then concatenate
        _DataType *fvPtr = featureVectorOutput.getData();    
        for(uint32_t c = 0; c < numChan; c++)
        {                
            FeatureVectorView<_DataType> subFV(fvPtr, briefExtractors[c]->getFeatureLength());
            briefExtractors[c]->extractImageFeature(imgExt, locXY, subFV);
            fvPtr += briefExtractors[c]->getFeatureLength();
        }
        return true;
    }

    //the length of the feature
    uint32_t getFeatureLength(void) const {return numFeature;}

    // get/set the stride and re-compute the offset the image 
    uint32_t getWindowStride(void) const {return briefExtractors[0]->getWindowStride();}

    void setWindowStride(uint32_t stride)
    {
        for (uint32_t i=0;i<numChan;i++)
            briefExtractors[i]->setWindowStride(stride);
    }

    Size2D<uint32_t> getWindowSize(void) const {return briefExtractors[0]->getWindowSize();}

    void setWindowSize(const Size2D<uint32_t> &size)
    {
        for (uint32_t i=0;i<numChan;i++)
            briefExtractors[i]->setWindowSize(size);
    }

private:
    
    /////////////////////////////////////////////
    //internal member variables    
    uint32_t numChan; //the number of channels
    uint32_t numFeature; //number of features (i.e. total number of pixel pair)
    BriefBinaryFeature<_DataType> **briefExtractors;            
};


}   // namespace qcv

#endif // qcv_briefbinarychanfeature_h_
//


