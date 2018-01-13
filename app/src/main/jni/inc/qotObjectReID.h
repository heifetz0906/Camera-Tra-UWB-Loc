//
//
/**=============================================================================

@file
qotObjectReID.h

@brief
Definition of an object ReID component for QOT, wrapper for person ReID

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

#ifndef qcv_qotobject_reid_h_
#define qcv_qotobject_reid_h_

#include <cstddef>
#include <vector>
#include <cassert>

#include "qcvImageExtension.h"

#include "qcvMemory.h"

#include "qotDefs.h"
#include "qotTypes.h"

#include "PersonReID.h"

namespace qcv
{

namespace qot
{


/********************************************************
QOT object verifier
*******************************************************/
class qotObjectReID 
{
public:     
    qotObjectReID(ImageExtension &img, const Rect<float32_t> &objBB);
    ~qotObjectReID( ); 

    bool initLearn(ImageExtension &img,  Rect<float32_t> initROI);

    bool learnPositive(ImageExtension &img, Rect<uint32_t> posROI);

    void verify(ImageExtension &img, std::vector<DetectionResult> &rects, std::vector<DetectionResult> &rectVerified);

    bool verify(ImageExtension &img, Rect<float32_t> rect, float32_t &confidence);

    bool learnSamples(ImageExtension &img, Rect<float32_t> posRect, const std::vector<DetectionResult> &negRects);

    uint32_t getNumSamples(uint32_t personID, PersonBodyParts partID);
    
    PersonReIDParams getReIDParameters(void) const;
    
private: 
    //person ReID
    PersonReID *reIDVerifier ; 
    bool initialized;
    
};



} //namespace qot

} //namespace qcv

#endif  //qcv_qotobject_reid_h_

