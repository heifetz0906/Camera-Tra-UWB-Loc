/**=============================================================================

@file
NormalizedPatch.h

@brief
Implementation of the feature of normalized image patch. It resize the image patch if
the input patch size is not the same as target size

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

#ifndef qcv_normalized_patch_h_
#define qcv_normalized_patch_h_


#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cstring>

#include "qcvMath.h"
#include "qcvTypes.h"
#include "qcvRect.h"
#include "qcvImage.h"
#include "qcvImageGray.h"
#include "qcvImagePlanarUtils.h"
#include "qcvImageExtension.h"
#include "ImageFeatureExtractor.h"
#include "ImageTransformation.h"
#include "qcvMemory.h"

//temp
//#include "highgui.h"


namespace qcv
{

/*****************************************************************************
Implementation of the feature of normalized image patch (zero-mean mean). It resize the image patch if
the input patch size is not the same as target size
Limitation: only assume uint8 image grayscale image
*********************************************************************************/
template <uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT>
class NormalizedPatch : public ImageFeatureExtractor < float32_t, float32_t>
{
    
public :        
    NormalizedPatch(const Size2D<float32_t> &windowSize, bool flagZeroMean); //, uint8_t *tempBuffer);
    NormalizedPatch( );
        
    ~NormalizedPatch();

    //public functions
public:   

    //the length of the feature
    uint32_t getFeatureLength(void) const {return _PATCH_WIDTH*_PATCH_HEIGHT;}

    //get window width and height    
    Size2D<float32_t> getWindowSize(void) const {return winSize;}
    
    void setWindowSize(const Size2D<float32_t> &size) {winSize = size;}
        
    //extract the feature from an image 
    //*Params: 
    //  locXY gives the upper-left conrer of the patch     
    bool extractImageFeature(ImageExtension &imgExt,  const Point2D<float32_t> &locXY, 
        FeatureVectorView<float32_t> &featureVectorOutput);

    void setZeroMean(bool flagZeroMean) {zeroMean = flagZeroMean;}
    bool isZeroMean(void) const {return zeroMean;}

    Size2D<uint32_t> getNormalizedPatchSize(void) const {Size2D<uint32_t> sz(_PATCH_WIDTH, _PATCH_HEIGHT); return sz;}
    
private:
    Size2D<float32_t> winSize;
    //internal variables
    bool zeroMean;     
    uint8_t *tempBuffer; 
    uint32_t *tempWarpBuffer;
            
};


//////////////////////////////////////////////////////////////
//Implementation

template <uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT>
NormalizedPatch<_PATCH_WIDTH, _PATCH_HEIGHT>::NormalizedPatch(const Size2D<float32_t> &windowSize, bool flagZeroMean) : // , uint8_t *buffer) :
    winSize(windowSize),
    zeroMean(flagZeroMean) 
    //tempBuffer(buffer)
{
    tempBuffer = (uint8_t *)MEMALLOC(_PATCH_WIDTH*_PATCH_WIDTH, 16);
    tempWarpBuffer = (uint32_t *)MEMALLOC(2 * _PATCH_HEIGHT * sizeof(uint32_t), 16);
}

template <uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT>
NormalizedPatch<_PATCH_WIDTH, _PATCH_HEIGHT>::NormalizedPatch( )
{
    winSize.setSize(_PATCH_WIDTH, _PATCH_HEIGHT);
    zeroMean = true; 
    tempBuffer = (uint8_t *)MEMALLOC(_PATCH_WIDTH*_PATCH_WIDTH, 16);
    tempWarpBuffer = (uint32_t *)MEMALLOC(2 * _PATCH_HEIGHT * sizeof(uint32_t), 16);
}


template <uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT>
NormalizedPatch<_PATCH_WIDTH, _PATCH_HEIGHT>::~NormalizedPatch()
    { 
        if (tempBuffer)
            MEMFREE(tempBuffer);
        if (tempWarpBuffer)
            MEMFREE(tempWarpBuffer);

    }

//only assume uint8 image, and extract template from one channel
//*Params: 
//      offset: the offset pointer relative to the input image pointer of ImgExt
template <uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT>
bool NormalizedPatch<_PATCH_WIDTH, _PATCH_HEIGHT>::extractImageFeature(ImageExtension &imgExt,  const Point2D<float32_t> &locXY, 
        FeatureVectorView<float32_t> &featureVectorOutput)
{
    //check the input length is the same as feature length
    assert(featureVectorOutput.getLength() == getFeatureLength()); 

    PlanarView imgView = imgExt.getImageView();

    //check the image is uint8
    assert(ImagePlanarUtils::isUint8(imgView, imgExt.chIdxPydInteg));

    //get the image pointer and add the offset     
    uint32_t imStride = ImagePlanarUtils::getStride(imgView, imgExt.chIdxPydInteg);
    uint32_t imWidth = ImagePlanarUtils::getWidth(imgView, imgExt.chIdxPydInteg);
    uint32_t imHeight = ImagePlanarUtils::getHeight(imgView, imgExt.chIdxPydInteg);
    uint8_t *imgPtr = ImagePlanarUtils::getPlanePtru8(imgView, imgExt.chIdxPydInteg);
    float32_t *featVect = featureVectorOutput.getData();

    /////////////////////////////////////////////////////////////////////
    //DGAO: check the minimum size on winSize.This may cause inaccurate template, but it is necessary if we use fcvScaleDownMNu8 to improve speed
    //////////////////////////////////////////////////////////////////////    
    /*
    if (winSize.w < _PATCH_WIDTH)
        winSize.w = _PATCH_WIDTH;
    if (winSize.h < _PATCH_HEIGHT)
        winSize.h = _PATCH_HEIGHT;    
        */
    //////////////////////////////////////////////////////////////////////

    //resize image if necessary
    Rect<float32_t> srcBB(locXY.x, locXY.y, winSize.w, winSize.h);    
    Rect<float32_t> imRect(0, 0, (float32_t)imWidth, (float32_t)imHeight);
    if (!imRect.contains(srcBB) || locXY.x != (uint32_t)locXY.x || locXY.y != (uint32_t)locXY.y || winSize.w != _PATCH_WIDTH || winSize.h !=_PATCH_HEIGHT )
    //if (winSize.w != _PATCH_WIDTH || winSize.h !=_PATCH_HEIGHT )
    {        
        //fcvScaleDownMNu8(imgPtr, winSize.w, winSize.h, imStride, tempBuffer, _PATCH_WIDTH, _PATCH_HEIGHT, _PATCH_WIDTH);
        //fcvScaleDownBLu8(imgPtr, winSize.w, winSize.h, imStride, tempBuffer, _PATCH_WIDTH, _PATCH_HEIGHT, _PATCH_WIDTH);
        imResizeBLSubPixel(imgPtr, imWidth, imHeight, imStride, locXY.x, locXY.y, winSize.w, winSize.h, tempBuffer, _PATCH_WIDTH, _PATCH_HEIGHT, _PATCH_WIDTH, tempWarpBuffer);
        
        //temp save the image 
        /*
        //save current image     
        CvMat a= cvMat(imgExt.imgHeight, imgExt.imgWidth,CV_8UC1, ImageGray::getGrayPtr(imgView));
        cvSaveImage("c:/temp/origImg.bmp", &a);
        */
        //temp: save the results
        /*
        FeatureVectorView<uint8_t> v(tempBuffer, 16*16);
        v.write("c:/temp/downscale_patch.txt");
        */

        if (zeroMean)
            imageZeroMean<uint8_t, float32_t>(tempBuffer, _PATCH_WIDTH, _PATCH_HEIGHT, _PATCH_WIDTH, featVect, _PATCH_WIDTH);       

        //temp: save the results
        /*
        featureVectorOutput.write("c:/temp/downscale_patch_zeromean.txt");
        */
    }
    else 
    {
        if (zeroMean)
            imageZeroMean<uint8_t, float32_t>(imgPtr, _PATCH_WIDTH, _PATCH_HEIGHT, imStride, featVect, _PATCH_WIDTH);                
        else
        {
            //diretctly copy the image
            uint8_t *imgPtrLine = imgPtr;
            float32_t *featPtr = featVect;
            for(uint32_t i = 0; i < winSize.h; i++)        
            {
                uint8_t *srcPtr = imgPtrLine;
                for(uint32_t j=0; j< winSize.w; j++)
                {
                    *(featPtr++) = (float32_t)( *(srcPtr++) );
                }
                imgPtrLine += imStride;
            }
        }
    }
    return true;
}


}   // namespace qcv

#endif // qcv_normalized_patch_h_
//


