/**=============================================================================

@file
qcvImagePyramid.h

@brief
Definition of a set of classes for image pyramids

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

#ifndef qcv_ImagePyramid_h_
#define qcv_ImagePyramid_h_


#include <cstddef>
//#include <stdexcept>
#include <vector>

#include "qcvTypes.h"
#include "qcvImage.h"
#include "fastcv.h"


namespace qcv
{

/*********************************************************************
class ImagePyramid: wrapper class for fast pyramid image
**********************************************************************/
class ImagePyramid
{
public:
    ImagePyramid();
    ImagePyramid(uint32_t imgW, uint32_t imgH, uint32_t imgStride, uint32_t pydLevels);
    virtual ~ImagePyramid();
   
    //public member functions
    virtual void initialize(uint32_t imgW, uint32_t imgH, uint32_t imgStride, uint32_t pydLevels);
    virtual void uninitialize();
    
    //compute the pyramid image for a give image 
    virtual void compute(const uint8_t *baseImg);

public: 
    uint32_t            baseWidth;
    uint32_t            baseHeight;
    uint32_t            baseStride;
    int32_t             pyramidLevels;          //number of Pyramid Level	
    fcvPyramidLevel_v2     *pyr;                   //Pyramid images
    
    //TODO: add plannar views of each pyramid level
};

}   // namespace qcv
#endif