/**=============================================================================

@file
ImageTransformation.h

@brief
functions for image transformation functions, such as normalization, etc.

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

#ifndef qcv_image_transformation_h_
#define qcv_image_transformation_h_


#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cstring>

#include "qcvTypes.h"
#include "qcvRect.h"

namespace qcv
{

////image normlizations function/////////////////////////////////////////////////////////////////

//subtract mean from an image patch 
template < class _SRC_TYPE, class _DST_TYPE>
void imageZeroMean(const _SRC_TYPE *src, uint32_t imW, uint32_t imH, uint32_t srcStride, _DST_TYPE *dst, uint32_t dstStride)
{    
    float32_t meanVal = 0.0f;
    const _SRC_TYPE *srcPtrLine = src;
    for(uint32_t i = 0; i < imH; i++)        
    {
        const _SRC_TYPE *srcPtr = srcPtrLine;
        for(uint32_t j=0; j< imW; j++)
        {
            meanVal += (float32_t) (*(srcPtr++));
        }
        srcPtrLine += srcStride;
    }

    meanVal /= float32_t(imW * imH);

    srcPtrLine = src;
    _DST_TYPE *dstPtrLine = dst;
    for(uint32_t i = 0; i < imH; i++)        
    {
        const _SRC_TYPE *srcPtr = srcPtrLine;
        _DST_TYPE *dstPtr = dstPtrLine;
        for(uint32_t j=0; j< imW; j++)
        {
            *(dstPtr++) = (_DST_TYPE)( *(srcPtr++) - meanVal );
        }
        srcPtrLine += srcStride;
        dstPtrLine += dstStride;
    }
}



//warp an src image with affine transformation at bounding box bb, and save the results into the same bounding box in the dest image.
int32_t warpImage(uint8_t *srcimg, const Rect<uint32_t> &bb, uint8_t *destimg, uint32_t imgWidth, uint32_t imgHeight, uint32_t imgStride, 
                  float32_t angle_in_degree, int32_t shiftX_in_pixel, int32_t shiftY_in_pixel, float32_t scale_change);

/*
//resize a bounding box (defined in subpixels) in an src image to output size.
int32_t imResizeBLSubPixel(const uint8_t *srcimg, uint32_t srcWidth, uint32_t srcHeight, uint32_t srcStride,  
                       float32_t centerX, float32_t centerY, float32_t bbWidth, float32_t bbHeight, 
                       uint8_t *dstimg, uint32_t dstWidth, uint32_t dstHeight, uint32_t dstStride);
*/

//resize a bounding box (defined in subpixels) in an src image to output size.
void imResizeBLSubPixel(const uint8_t *srcimg, uint32_t srcWidth, uint32_t srcHeight, uint32_t srcStride,  
                       float32_t upperleftX, float32_t upperleftY, float32_t bbWidth, float32_t bbHeight, 
                       uint8_t *dstimg, uint32_t dstWidth, uint32_t dstHeight, uint32_t dstStride, uint32_t *dstBorderBuffer);

}   // namespace qcv

#endif // qcv_image_transformation_h_
//


