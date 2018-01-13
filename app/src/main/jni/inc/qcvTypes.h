/**=============================================================================

@file
qcvTypes.h

@brief
Definition of some data types, such as point, rectangle, bounding box, etc

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


#ifndef qcv_Types_h__
#define qcv_Types_h__

// Some common shared global types

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cstdarg>
#include <cmath>

//use data types defined in fastcv.h as much as possible
#include "fastcv.h"
//#include "qcvRect.h"

//use the following defs for memory allocation and free
#define MEMALLOC    fcvMemAlloc
#define MEMFREE     fcvMemFree

namespace qcv
{
// Integer types. Currently relying on optional <cstdint>. If they are not available, we'll have to define otherwise here.

typedef uint8_t        Uint8;
typedef uint16_t       Uint16;
typedef uint32_t       Uint32;
typedef uint64_t       Uint64;
    
typedef int8_t         Int8;
typedef int16_t        Int16;
typedef int32_t        Int32;
typedef int64_t        Int64;

typedef float               Float32;
typedef double              Float64;

typedef unsigned char       Byte;


#ifdef ANDROID
#include <android/log.h>
//==============================================================================
// Declarations
//==============================================================================
#define LOG_TAG    "qcvLib-QOT"
#define DPRINTF(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define IPRINTF(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define EPRINTF(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define PRINTF	DPRINTF
#else
#ifndef PRINTF
#define PRINTF	printf
#endif
#endif


//------------------------------------------------------------------------------
/// @brief
///    Structure representing a Point.
/// 
/// @param nLocX
///    X co-ordinate
/// 
/// @param nLocY
///    Y co-ordinate
//------------------------------------------------------------------------------
template< class _T>
class Point2D
{
public:
    Point2D(void) { }
    Point2D(_T locX, _T locY): x(locX), y(locY) {}

    void setLoc(_T locX, _T locY) {x = locX; y = locY;}
    void clear() { setLoc((_T)0,(_T)0);}
    bool equal(const Point2D<_T> &pt) const { return (x == pt.x && y == pt.y); }
    float32_t distance(const Point2D<_T> &pt) const {   return (float32_t)sqrtf(((float32_t)x - (float32_t)pt.x)*((float32_t)x - (float32_t)pt.x) + ((float32_t)y - (float32_t)pt.y)*((float32_t)y - (float32_t)pt.y));}
        
   _T x;
   _T y;

};

template <class _T>
class Size2D
{
public:
    Size2D(void) { }
    Size2D(_T width, _T height): w(width), h(height) {}

    void setSize(_T width, _T height) {w = width; h = height;}
    void clear() { setSize((_T)0, (_T)0);}    
    bool equal(const Size2D<_T> &sz) const {return (w==sz.w && h==sz.h);}
    bool empty(const Size2D<_T> &sz) const {return (w==0 && h==0);}
    
    //multiply by a constant (scale effect)	 
	Size2D<_T> &operator*=(const float32_t s) 
    {
        if (s<=0)
            clear();
        else
        {
            w = (_T)((float32_t)w*s);
            h = (_T)((float32_t)h*s);
        }
        return *this;
    }

    const Size2D<_T> operator*(const float32_t s) const
    {        
        
        return Size2D<_T>(*this)*=s;
    }

    
   _T w;
   _T h;


};


//------------------------------------------------------------------------------
/// @brief
///    Structure representing a Bounding Box.
/// 
/// @param topLeft
///    co-ordinates of the top-left corner of the Bounding Box
/// 
/// @param topRight
///    co-ordinates of the top-right corner of the Bounding Box
/// 
/// @param botLeft
///    co-ordinates of the bottom-left corner of the Bounding Box
/// 
/// @param botRight
///    co-ordinates of the bottom-right corner of the Bounding Box
//------------------------------------------------------------------------------
template< class _T>
struct BoundingBox
{
   Point2D<_T> topLeft;
   Point2D<_T>  topRight;
   Point2D<_T>  botLeft;
   Point2D<_T>  botRight;
public:
   void clear() 
   {
       topLeft.clear();
       topRight.clear();
       botLeft.clear();
       botRight.clear();
   }

   bool isValid() const
   {
       return !topLeft.equal(botRight);
   }
};


}   // namespace qcv

#endif // qcv_Types_h__

