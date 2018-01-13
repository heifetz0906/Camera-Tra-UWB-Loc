/**=============================================================================

@file
qcvTime.h

@brief
some time related util functions

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

#ifndef qcv_utils_time_h__
#define qcv_utils_time_h__


#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cstring>
#include "qcvTypes.h"

namespace qcv
{

// For the timer
//////////////////////////////////////////////////////////////
/////some time related functions
uint32_t getTimeMicroSeconds();

} //namespace qcv 


#endif //qcv_utils_time_h__
