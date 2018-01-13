/**=============================================================================

@file
UpperBodyLOMOModel.h

@brief
XQDA projection matrices for Upper Body LOMO feature

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

#ifndef qcv_upper_body_lomo_model_h_
#define qcv_upper_body_lomo_model_h_

#include "fastcv.h"
#include "qcvTypes.h"

namespace qcv
{
    namespace PersonBodyLOMOModels
    {

        const uint32_t UPPER_BODY_LOMO_DIM = 3558;
        const uint32_t UPPER_BODY_LOMO_PROJ_DIM = 57;
        extern const float32_t UPPER_BODY_LOMO_PROJ_MTX[UPPER_BODY_LOMO_PROJ_DIM][UPPER_BODY_LOMO_DIM];
        extern const float32_t UPPER_BODY_LOMO_XQDA_WEIGHT_MTX[UPPER_BODY_LOMO_PROJ_DIM][UPPER_BODY_LOMO_PROJ_DIM];

    } //namespace PersonBodyLOMOModels
} //namespace qcv

#endif //qcv_upper_body_lomo_model_h_