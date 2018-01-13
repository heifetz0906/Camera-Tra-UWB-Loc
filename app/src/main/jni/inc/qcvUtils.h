/**=============================================================================

@file
qcvUtils.h

@brief
some internally defined utility macros and functions 

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


//
#ifndef qcv_Utils_h_
#define qcv_Utils_h_

// Some common shared global types

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <string>

#include "fastcv.h"

namespace qcv
{

//Location computation
#ifndef LOCATION
#define LOCATION(x,y,w) ( ((int32_t)Round(x) + (int32_t)Round(y)*(int32_t)(w)))
#endif

//Aignment computation
#ifndef AlignSize
#define AlignSize(sz, n) ((sz + n-1) & -n)
#endif


//string to enum conversion 
template <class _EnumType> 
_EnumType stringToEnum(std::string name, const std::string enumNames[], const uint32_t size) 
{
    for (uint32_t i=0; i < size; i++) 
    {
        if (name == enumNames[i]) {
            return static_cast<_EnumType>(i);
        }
    }
    //default to return 0
    return static_cast<_EnumType>(0);
}

}

#endif //qcv_Utils_h_