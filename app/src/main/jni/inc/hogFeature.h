/**=============================================================================

@file
hogFeature.h

@brief
Implementation from Ruiduo for the HOG feature

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

#ifndef qcv_hogfeature_h_
#define qcv_hogfeature_h_


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

#if 0
    int QHDAlgorithm::sizew = 24;
    int QHDAlgorithm::sizeh = 72;
    int QHDAlgorithm::hogcellsize=6;
    int QHDAlgorithm::hogblocksize=12;
    int QHDAlgorithm::hogblockstride=6;
    int QHDAlgorithm::hogbinnumber=9;
    int QHDAlgorithm::hogHeight=72; //set to 108 for full body hog. set to 36 for upper body hog.
#endif

/**************************************************************
 
*****************************************************************/
class hogFeature : public ImageFeatureExtractor < float32_t, uint32_t >
{
    
public :
    hogFeature(bool BILINEARHOG, uint32_t imageWidth, uint32_t imageHeight, uint32_t hogCellSize, uint32_t hogBlockSize, uint32_t hogBlockStride, uint32_t hogBinNumber);
    ~hogFeature();

    //public functions
public:
    //extract the feature from a location (window size fixed)
    //*Params: 
    //      locXY: the location relative to the input image pointer provided by ImgExt  
    bool extractImageFeature(ImageExtension &imgExt,  const Point2D<uint32_t> &locXY, 
        FeatureVectorView<float32_t> &featureVectorOutput);

    //the length of the feature
    uint32_t getFeatureLength(void) const {return numFeature;}
 
    // get/set the size the image 
    Size2D<uint32_t> getWindowSize(void) const;
    void setWindowSize(const Size2D<uint32_t> &size);

private:
    /////////////////////////////////////////////
    //internal functions    
    void constructLookUpTable(void);
    void EdgeAngle_Integer(uint8_t *src, uint32_t swidth, uint32_t sheight, uint32_t sstride, uint8_t *orientation, uint32_t dstride, uint8_t* dstWeight, uint32_t dstWeightstride);
    void ComputeHOG(int x, int y, uint8_t *src, uint32_t swidth, uint32_t sheight, uint32_t sstride, uint8_t * srcweight, uint32_t srcweightstride, float* hogdescriptor);
    /////////////////////////////////////////////
    //internal member variables
    bool BILINEARHOG;
    uint32_t imageWidth;
    uint32_t imageHeight;
    uint32_t hogCellSize;
    uint32_t hogBlockSize; 
    uint32_t hogBlockStride; 
    uint32_t hogBinNumber;
    uint32_t upperbound;
    uint32_t numFeature;

    //inner image
    uint8_t *magnitude;
    uint8_t *orientation;
    float32_t* buff;
    float32_t* buffOri;

    //look up table 
  /*  static uint8_t *index0_lut;
    static uint8_t *index1_lut;
    static float32_t *weight0_lut;
    static float32_t *weight1_lut;*/
    uint8_t *index0_lut;
    uint8_t *index1_lut;
    float32_t *weight0_lut;
    float32_t *weight1_lut;

};

}   // namespace qcv

#endif // qcv_hogfeature_h_
//
