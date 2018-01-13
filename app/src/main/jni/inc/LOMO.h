/**=============================================================================

@file
LOMO.h

@brief
Extract the LOMO feature from an image patch

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

#ifndef qcv_lomo_feature_h_
#define qcv_lomo_feature_h_


#include <cstddef>
#include <vector>

#include "qcvTypes.h"
#include "qcvImageExtension.h"
#include "ImageFeatureExtractor.h"
#include "ImageQuantizerND.h"
#include "SILTP.h"

namespace qcv
{

//LOMO parameters 
class LOMOParameters
{
public:
    LOMOParameters(uint32_t nNonOverlapHoriBands, bool overlapWindow, uint32_t nScales, bool norm) :
        numNonOvlpHoriBands(nNonOverlapHoriBands),
        overlapWin(overlapWindow),
        numScales(nScales),
        normalize(norm)
    {    }
   
    uint32_t numNonOvlpHoriBands; //number of non-overlapping horizontal bands (use number of bands to achieve scale invariance), band height is also the window size
    bool     overlapWin;     //Add vertical and horizontal overlap between bands and windows, this will increase the number of features
    uint32_t numScales;      //number of LOMO scales (each scale double the window size and half the number of bands)         
    bool normalize;

    //no need to contain parameters SILTP and HSV color histogram 
};


/**************************************************************
compute LOMO feature for an image patch 
reference: Person Re-Identification by Local Maximal Occurrence Representation and Metric Learning
Shengcai Liao, Yang Hu, Xiangyu Zhu, Stan Z. Li; The IEEE Conference on Computer Vision and Pattern Recognition (CVPR), 2015, pp. 2197-2206
*******************************************************
** NOTE: only support YUV or YVU planar input image. **
*******************************************************
*****************************************************************/
class LOMOFeature : public ImageFeatureExtractor < float32_t, uint32_t>
{
    
public :
    LOMOFeature(const Size2D<uint32_t> &roiSize, const LOMOParameters &lomoParam, const ImageQuantizerNDParam &HSVHistParams, const SILTPParam &siltpParams); 

    virtual ~LOMOFeature();

    //public functions

    //the length of the feature
    uint32_t getFeatureLength(void) const;

    Size2D<uint32_t> getWindowSize(void) const;

    void setWindowSize(const Size2D<uint32_t> &size);
    
    //extract the variance of an image patch
    /*******************************************************
    ** NOTE: only support YUV or YVU planar input image. **
    *******************************************************/
    bool extractImageFeature(ImageExtension &imgExt, const Point2D<uint32_t> &locXY,
        FeatureVectorView<float32_t> &featureVectorOutput);
    
private:

    //compute the total number of bands in the LOMO feature
    uint32_t getNumBands(void) const;

    //compute lomo feature given the input histogram bin index map on multi-scales
    bool computelomoMultiScale(uint32_t *histBinPatch, uint32_t patchWidth, uint32_t patchHeight, uint32_t histBinLevels, float32_t *out, uint32_t *outLen);

    //compute lomo feature given the input histogram bin index map on a single scale
    bool computelomoSingleScale(uint32_t *histBinPatch, uint32_t patchWidth, uint32_t patchHeight, uint32_t histBinLevels, uint32_t numNonOvlpBands, bool overlap, float32_t *out, uint32_t *outLen);

    //normalize feature    
    void normalize(float32_t *vect, uint32_t vectLen);
    
private: 

    //variables
    Size2D<uint32_t> winSize; 
    
    bool            initialized;
        
    LOMOParameters  param;
    
    //feature extractors    
    ImageQuantizerND<uint32_t>  *hsvHist;
    SILTPFeature<uint32_t>      *siltp;

    qcvMemory                   scratchBuffer;
    uint32_t                    numTotalBands;
};




}   // namespace qcv

#endif // qcv_lomo_feature_h_
//


