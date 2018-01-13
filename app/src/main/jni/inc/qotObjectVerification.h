//
//
/**=============================================================================

@file
NCCObjectVerification.h

@brief
Definition of an object verifier for QOT

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

#ifndef qcv_qotobjectverification_h_
#define qcv_qotobjectverification_h_

#include <cstddef>
#include <vector>
#include <cassert>

#include "qcvImageExtension.h"

#include "NCCPatchClassifier.h"
#include "NormalizedPatch.h"
#include "qcvMemory.h"

#include "qotDefs.h"
#include "qotTypes.h"


namespace qcv
{

namespace qot
{


/********************************************************
QOT object verifier
*******************************************************/
class qotObjectVerifier 
{
public: 
    qotObjectVerifier( NCCTemplateClassifierModel<NCC_PATCH_SIZE, NCC_PATCH_SIZE, NCC_MAX_POSITIVE_PATCHES, NCC_MAX_NEGATIVE_PATCHES> &  nccClassifierModel);
    ~qotObjectVerifier( ); 

    //void verify(ImageExtension &img, const std::vector<DetectionScaleResult> &rects, std::vector<DetectionResult> &rectVerified);
    void verify(ImageExtension &img, std::vector<DetectionScaleResult> &rects, std::vector<DetectionScaleResult> &rectVerified);

    bool verify(ImageExtension &img, const Rect<float32_t> &rect, float32_t &confidence);

    bool verify(ImageExtension &img, const Rect<float32_t> &rect, const Size2D<float32_t> &scanWinSize, float32_t &confidence);
    
    //add set classifier training model (has to be the final classifier)
    void setClassifierModel(NCCTemplateClassifierModel<NCC_PATCH_SIZE, NCC_PATCH_SIZE, NCC_MAX_POSITIVE_PATCHES, NCC_MAX_NEGATIVE_PATCHES> &model);
    
    //get/set the threshold for veriffication
    void setConfidenceThreshold(float32_t threshold);
    float32_t getConfidenceThreshold(void) const {return confidenceThreshold;}

    NCCImagePatchClassifier<NCC_PATCH_SIZE, NCC_PATCH_SIZE, NCC_MAX_POSITIVE_PATCHES, NCC_MAX_NEGATIVE_PATCHES> &getPatchClassifier(void) {return patchClassifier;}

private: 
    //patch classifier     
    NCCImagePatchClassifier<NCC_PATCH_SIZE, NCC_PATCH_SIZE, NCC_MAX_POSITIVE_PATCHES, NCC_MAX_NEGATIVE_PATCHES> patchClassifier;
    float32_t confidenceThreshold;

};



} //namespace qot

} //namespace qcv

#endif  //qcv_qotobjectverification_h_

