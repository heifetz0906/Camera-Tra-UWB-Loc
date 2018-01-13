/**=============================================================================

@file
ImageStats.h

@brief
functions for computing image statistics

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

#ifndef qcv_image_stats_h_
#define qcv_image_stats_h_


#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cstring>

#include "qcvTypes.h"
#include "qcvRect.h"
#include "qcvImageExtension.h"

namespace qcv
{
namespace imStats
{

//compute the variance of the image 
float32_t imageVariance(ImageExtension &imgExt);

//compute the variance of the pixels inside a window using integral image
float32_t imagePatchVariance(ImageExtension &imgExt, const Rect<uint32_t> &patch);




}   //namespace imstats

}   // namespace qcv

#endif // qcv_image_transformation_h_
//


