/**=============================================================================

@file
LOMO.h

@brief
Extract the Person Signature features 

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

#ifndef qcv_person_signature_extraction_h_
#define qcv_person_signature_extraction_h_


#include <cstddef>
#include <vector>

#include "qcvTypes.h"
#include "qcvImageExtension.h"
#include "LOMO.h"
#include "qcvRect.h"
#include "PersonSignatureModel.h"

namespace qcv
{


/**************************************************************
extract person signature features for different parts
*****************************************************************/
class PersonSignatureExtraction
{
    
public :
    PersonSignatureExtraction(); 

    virtual ~PersonSignatureExtraction();

    //public functions

    //the length of the feature
    uint32_t getBodyFeatureLength(PersonBodyParts partID) const;
    
    //extract the variance of an image patch
    /*******************************************************
    ** NOTE: only support YUV or YVU planar input image due to the LOMO feature limitation . **
    *******************************************************/
    bool extractBodyFeature(PersonBodyParts partID, ImageExtension &imgExt, const Rect<uint32_t> roi, FeatureVectorView<float32_t> &featureVectorOutput);

private:
    bool initBodyLOMOExtractor();

private: 
    bool initialized;

    //full body
    LOMOFeature *fullBodyLOMO;
    //xqda dim reduction projectinon
    const float32_t *fullBodyLOMOProjMtx; 
    uint32_t  fullBodyLOMODim;
    uint32_t fullBodyLOMOProjDim;

    //upper body
    LOMOFeature *upperBodyLOMO;
    //xqda dim reduction projectinon
    const float32_t *upperBodyLOMOProjMtx;   
    uint32_t  upperBodyLOMODim;
    uint32_t  upperBodyLOMOProjDim;

    qcvMemory scratchBuffer;

};




}   // namespace qcv

#endif // qcv_person_signature_extraction_h_
//


