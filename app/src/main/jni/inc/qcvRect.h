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


#ifndef qcv_Rect_h_
#define qcv_Rect_h_

// Some common shared global types

//#include <cstdarg>
//#include <cstddef>
#include <cstdint>
//#include <cassert>
//#include <iostream>
//#include <fstream>
#include <cmath>

//use data types defined in fastcv.h as much as possible
#include "fastcv.h"

#include "qcvTypes.h"
#include "qcvMath.h"

namespace qcv
{

//------------------------------------------------------------------------------
/// @brief
///    Structure representing a Rectangle.
/// 
/// @param nLocX
///    X co-ordinates of the top-left corner of the Rectangle
/// 
/// @param nLocY
///    Y co-ordinates of the top-left corner of the Rectangle
/// 
/// @param nWidth
///    Width of the Rectangle
/// 
/// @param nHeight
///    Height of the Rectangle
//------------------------------------------------------------------------------
template< class _T>
class Rect 
{
public:
    Rect() : nLocX(0), nLocY(0), nWidth(0), nHeight(0) { }

    Rect(_T x, _T y, _T w, _T h) :
        nLocX(x), nLocY(y),nWidth(w),nHeight(h) {  }
    
    virtual ~Rect() { }
        
    void clear();
    
    template<class _TSrc>
    void copyFrom(const Rect<_TSrc> &srcRect);

    template<class _Tdst> 
    Rect<_Tdst> convert() const;
    
    void set(_T x, _T y, _T w, _T h);
    void setByCenterWH(_T centerX, _T centerY, _T w, _T h);

    void clamp(float32_t minXLim, float32_t minYLim, float32_t maxXLim, float32_t maxYLim);

    _T area() const;
    _T areaLessBorder() const; //the area with (width-1)*(height-1)

    Point2D<_T> center( ) const;
    _T maxX() const;
    _T maxY() const;

    //enlarge the bbox size by "scaleSZ" on each side
    //maxW and maxH is used to limit the Rectangle is inside the image boundary
    //set it to a large number if you don't care about image
    void scale(const Size2D<_T> &scaleSZ, const Size2D<_T> &maxSZ = Size2D<_T>(0,0));
    void scale(const Size2D<_T> &scaleSZ, const Point2D<_T> &minXY, const Size2D<_T> &maxSZ = Size2D<_T>(0,0));

    //scale by a factor on each side
    void scaleByFactor(float32_t scaleSZFactor, const Size2D<_T> &maxSZ = Size2D<_T>(0,0)); 
    void scaleByFactor(float32_t scaleSZFactor, const Point2D<_T> &minXY, const Size2D<_T> &maxSZ = Size2D<_T>(0,0)) ;

    float32_t distance(const Rect<_T> &rect) const ;

    //set the width and height to zero
    void invalidate();

    bool isValid() const;

    bool equal(const Rect<_T> &rect) const;

    bool equalApprox(const Rect<_T> &rect, float32_t eps) const;

    //compute the intersectoin between two rectangles
    _T Intersection(const Rect<_T> &rect) const;

    //compute the union of two rects
    _T Union(const Rect<_T> &rect) const;

    //compute the overlap of two rects, and return value between 0 and 1
    //overlap = intersect(rect1, rect2)/union(rect1, rect2)
    float32_t overlap(const Rect<_T> &rect) const;

    //true if overlap, false otherwise
    bool hasOverlap(const Rect<_T> &rect) const;

    bool contains(const Rect<_T> &rect) const;

    void toBoundingBox(BoundingBox<_T> &bb) const;
    BoundingBox<_T> toBoundingBox() const;

    //convert BoundingBox to Rectangle
    void fromBoundingBox(const BoundingBox<_T> &bb);    

	//reload the operators
    const Rect<_T> operator*(const float32_t s) const;
	Rect<_T> & operator*=(const float32_t s) ;    

    const Rect<_T> operator+(const Rect<_T> &r) const;
	Rect<_T> & operator+=(const Rect<_T> &r) ;    

    //average two rectangle by weights, normalize weight so that they sum to 1
    static Rect<_T> average(const Rect<_T> &r1, float32_t w1, const Rect<_T> &r2, float32_t w2, bool normalize, bool roundXY, bool roundWH);        
    //average a set of rectangles using different weights
    static Rect<_T> average(const std::vector<Rect<_T> > &rects, const std::vector<float32_t> &weights, bool normalize, bool roundXY, bool roundWH);
    //average a set of rectangles with no weights
    static Rect<_T> average(const std::vector<Rect<_T> > &r, bool roundXY, bool roundWH);

    //write the Rectangle into a ascii file
    //in format: nLocX nLocY nWidth  nheight
    //void write(std::ofstream &outfile) const;    
    void write(const char *filename) const;

public:    
    _T  nLocX;
    _T  nLocY;
    _T nWidth;
    _T nHeight;

};


/********************************************
Rect Definitions
********************************************/
template<class _T> 
void Rect<_T>::clear()
{
    nLocX = (_T)0;
    nLocY = (_T)0;
    nWidth =(_T)0;
    nHeight =(_T)0;
}


template<class _T>
template<class _TSrc>
void Rect<_T>::copyFrom(const Rect<_TSrc> &srcRect)
{
    nLocX =(_T)srcRect.nLocX;
    nLocY = (_T)srcRect.nLocY;
    nWidth = (_T)srcRect.nWidth;
    nHeight = (_T)srcRect.nHeight;    
}

template<class _T>
template<class _Tdst> 
Rect<_Tdst> Rect<_T>::convert() const
{
    Rect<_Tdst> dstRect;
    dstRect.nLocX = (_Tdst) nLocX;
    dstRect.nLocY = (_Tdst) nLocY;
    dstRect.nWidth = (_Tdst) nWidth;
    dstRect.nHeight = (_Tdst) nHeight;
    return dstRect;
}


template<class _T> 
void Rect<_T>::set(_T x, _T y, _T w, _T h) 
{
    nLocX = x;
    nLocY = y; 
    nWidth = w; 
    nHeight = h;
}


template<class _T> 
void Rect<_T>::setByCenterWH(_T centerX, _T centerY, _T w, _T h)
{
    nLocX = (_T) (centerX - (float32_t)(w-1) * 0.5f ); 
    nLocY = (_T) (centerY - (float32_t)(h-1) * 0.5f );
    nWidth = w;
    nHeight = h;
}


template<class _T> 
void Rect<_T>::clamp(float32_t minXLim, float32_t minYLim, float32_t maxXLim, float32_t maxYLim)
{
    _T rectMaxX = maxX();
    _T rectMaxY = maxY();
    
    if (nLocX < minXLim)
        nLocX = (_T)minXLim;

    if (nLocY < minYLim)
        nLocY = (_T)minYLim;

    if (rectMaxX > maxXLim)
        rectMaxX = (_T)maxXLim;
        
    if (rectMaxY > maxYLim)
        rectMaxY = (_T)maxYLim;

    //update width and height 
    if (nWidth>0)
        nWidth = rectMaxX - nLocX + 1;
    if (nHeight>0)
        nHeight = rectMaxY - nLocY + 1;

}


template<class _T> 
_T Rect<_T>::area() const
{
    return nWidth*nHeight;
}

//the area with (width-1)*(height-1)
template<class _T> 
_T Rect<_T>::areaLessBorder() const
{
    return (nWidth-1)*(nHeight-1);
}

template<class _T> 
Point2D<_T> Rect<_T>::center() const
{
    Point2D<_T> center;
    center.x = (_T)(nLocX + (float32_t)(nWidth-1) * 0.5f );
    center.y = (_T)(nLocY + (float32_t)(nHeight-1) * 0.5f );
    return center;
}

template<class _T> 
_T Rect<_T>::maxX() const
{
    if (nWidth>0)
        return nLocX + nWidth - 1;
    else
        return nLocX;
}


template<class _T> 
_T Rect<_T>::maxY() const
{
    if (nHeight>0)
        return nLocY + nHeight - 1;
    else
        return nLocY;
}

//scale the bbox size by "scaleSZ" on each side
//imW and imH is used to limit the Rectangle is inside the image boundary
//set them to 0 if you don't care about image
template<class _T> 
void Rect<_T>::scale(const Size2D<_T> &scaleSZ, const Size2D<_T> &maxSZ)
{    
    _T minx, maxx, miny, maxy;
    if (nLocX > scaleSZ.w)
        minx = nLocX - scaleSZ.w;
    else 
        minx = 0;    
    if (maxSZ.w>0)
        maxx = MIN(nLocX + (_T)nWidth + scaleSZ.w, maxSZ.w);
    else
        maxx = nLocX + (_T)nWidth + scaleSZ.w;
    
    if (minx >= maxx)
    {
        nLocX = center().x;
        nWidth = 1;
    }
    else
    {
        nLocX = minx;
        nWidth= maxx - minx;
    }
    
    if (nLocY > scaleSZ.h)
        miny = nLocY - scaleSZ.h;
    else
        miny = 0;
    if (maxSZ.h > 0)
        maxy = MIN(nLocY + (_T)nHeight + scaleSZ.h, maxSZ.h);
    else
        maxy = nLocY + (_T)nHeight + scaleSZ.h;
    
    if (miny >= maxy)
    {
        nLocY = center().y;
        nHeight = 1;
    }
    else 
    {
        nLocY = miny;    
        nHeight = maxy - miny;
    }
}

template<class _T> 
void Rect<_T>::scale(const Size2D<_T> &scaleSZ, const Point2D<_T> &minXY, const Size2D<_T> &maxSZ)
{    
    _T minx, maxx, miny, maxy;
    
    minx = MAX(nLocX - scaleSZ.w, minXY.x);    
    if (maxSZ.w>0)
        maxx = MIN(nLocX + (_T)nWidth + scaleSZ.w, maxSZ.w);
    else
        maxx = nLocX + (_T)nWidth + scaleSZ.w;
    
    if (minx >= maxx)
    {
        nLocX = center().x;
        nWidth = 1;
    }
    else
    {
        nLocX = minx;
        nWidth= maxx - minx;
    }
    
    miny = MAX(nLocY - scaleSZ.h, minXY.y);    
    if (maxSZ.h > 0)
        maxy = MIN(nLocY + (_T)nHeight + scaleSZ.h, maxSZ.h);
    else
        maxy = nLocY + (_T)nHeight + scaleSZ.h;
    
    if (miny >= maxy)
    {
        nLocY = center().y;
        nHeight = 1;
    }
    else 
    {
        nLocY = miny;    
        nHeight = maxy - miny;
    }
}


//scale by a factor on each side
template<class _T> 
void Rect<_T>::scaleByFactor(float32_t scaleSZFactor, const Size2D<_T> &maxSZ) 

{
    if (scaleSZFactor < -0.5f) //scale by -0.5 shrinks to a point
    {
        clear();
        return;
    }

    if (scaleSZFactor ==0)
        return;

    Size2D<_T> scaleSZ( (_T)((float32_t)nWidth*scaleSZFactor), (_T) ((float32_t)nHeight*scaleSZFactor));
    
    scale(scaleSZ, maxSZ);

}

//scale by a factor on each side
template<class _T> 
void Rect<_T>::scaleByFactor(float32_t scaleSZFactor, const Point2D<_T> &minXY, const Size2D<_T> &maxSZ) 

{
    if (scaleSZFactor < -0.5f) //scale by -0.5 shrinks to a point
    {
        clear();
        return;
    }

    if (scaleSZFactor ==0)
        return;

    Size2D<_T> scaleSZ( (_T)((float32_t)nWidth*scaleSZFactor), (_T) ((float32_t)nHeight*scaleSZFactor));
    
    scale(scaleSZ, minXY, maxSZ);

}



template<class _T> 
float32_t Rect<_T>::distance(const Rect<_T> &rect) const 
{    
    Point2D<_T> center1 = center();
    Point2D<_T> center2 = rect.center();   
    float32_t dist = sqrt(((float32_t)center1.x-(float32_t)center2.x)*((float32_t)center1.x-(float32_t)center2.x) + ((float32_t)center1.y-(float32_t)center2.y)*((float32_t)center1.y-(float32_t)center2.y));
    return dist;
}

//set the width and height to zero
template<class _T> 
void Rect<_T>::invalidate()
{
    nWidth = (_T)0;
    nHeight = (_T)0;
}

template<class _T> 
bool Rect<_T>::isValid() const
{
    return (nWidth>0 && nHeight>0);
}

template<class _T> 
bool Rect<_T>::equal(const Rect<_T> &rect) const
{
    return (nLocX == rect.nLocX && nLocY==rect.nLocY && nWidth==rect.nWidth && nHeight==rect.nHeight);    
}


template<class _T> 
bool Rect<_T>::equalApprox(const Rect<_T> &rect2, float32_t eps) const
{    
    return (  abs( (float32_t)nLocX - (float32_t)rect2.nLocX ) < eps  
            &&  abs( (float32_t)nLocY - (float32_t)rect2.nLocY ) < eps 
            &&  abs( (float32_t)nWidth - (float32_t)rect2.nWidth ) < eps 
            &&  abs( (float32_t)nHeight - (float32_t)rect2.nHeight) < eps );    
}


//compute the intersectoin between two rectangles
template<class _T> 
_T Rect<_T>::Intersection(const Rect<_T> &rect) const
{
    if (nLocX >= rect.nLocX + rect.nWidth || 
        nLocY >= rect.nLocY + rect.nHeight || 
        nLocX + nWidth <= rect.nLocX ||
        nLocY + nHeight <= rect.nLocY )
        return 0;

    _T cols = MIN(nLocX+nWidth, rect.nLocX+rect.nWidth) - MAX(nLocX, rect.nLocX);
    _T rows = MIN(nLocY+nHeight, rect.nLocY + rect.nHeight) - MAX(nLocY, rect.nLocY);
    
    _T intersect = (_T)cols * (_T)rows;

    return intersect;
}

//compute the union of two rects
template<class _T> 
_T Rect<_T>::Union(const Rect<_T> &rect) const 
{
    
    _T intersect;

    if (nLocX >= rect.nLocX + rect.nWidth || 
        nLocY >= rect.nLocY + rect.nHeight || 
        nLocX + nWidth <= rect.nLocX ||
        nLocY + nHeight <= rect.nLocY )
    {
        intersect = 0 ;
    }
    else
    {
        _T cols = MIN(nLocX+nWidth, rect.nLocX+rect.nWidth) - MAX(nLocX, rect.nLocX);
        _T rows = MIN(nLocY+nHeight, rect.nLocY + rect.nHeight) - MAX(nLocY, rect.nLocY);
    
        intersect = (_T)cols * (_T)rows;
    }

    return area() + rect.area() - intersect;
}

//compute the overlap of two rects, and return value between 0 and 1
//overlap = intersect(rect1, rect2)/union(rect1, rect2)
template<class _T> 
float32_t Rect<_T>::overlap(const Rect<_T> &rect) const 
{

    _T cols = 0;
    _T rows = 0;
    _T intersect = 0;
    _T area1 = 0;
    _T area2 = 0;

    if (nLocX >= rect.nLocX + rect.nWidth || 
        nLocY >= rect.nLocY + rect.nHeight || 
        nLocX + nWidth <= rect.nLocX ||
        nLocY + nHeight <= rect.nLocY )
        return 0.0f;

    cols = MIN(nLocX+nWidth, rect.nLocX+rect.nWidth) - MAX(nLocX, rect.nLocX);
    rows = MIN(nLocY+nHeight, rect.nLocY + rect.nHeight) - MAX(nLocY, rect.nLocY);
    
    intersect = (_T)cols * (_T)rows;

    return (float32_t)intersect / (float32_t)(area() + rect.area() - intersect);

}

//true if overlap, false otherwise
template<class _T>
bool Rect<_T>::hasOverlap(const Rect<_T> &rect) const
{
    if (nLocX >= rect.nLocX + rect.nWidth || 
        nLocY >= rect.nLocY + rect.nHeight || 
        nLocX + nWidth <= rect.nLocX ||
        nLocY + nHeight <= rect.nLocY )
        return false;
    else
        return true;
}

//true if the rect is contained inside, false otherwise
template<class _T>
bool Rect<_T>::contains(const Rect<_T> &rect) const
{    
    if (nLocX <= rect.nLocX && 
        nLocX + nWidth >= rect.nLocX + rect.nWidth &&
        nLocY <= rect.nLocY &&         
        nLocY + nHeight >= rect.nLocY + rect.nHeight )
        return true;
    else
        return false;
}


template<class _T> 
void Rect<_T>::toBoundingBox(BoundingBox<_T> &bb) const
{
    bb.topLeft.setLoc(nLocX, nLocY);
    bb.topRight.setLoc(nLocX + nWidth-1, nLocY);
    bb.botLeft.setLoc(nLocX, nLocY + nHeight-1);
    bb.botRight.setLoc(nLocX + nWidth-1, nLocY + nHeight-1);
}

template<class _T> 
BoundingBox<_T> Rect<_T>::toBoundingBox() const
{
    BoundingBox<_T> bb;    
    bb.topLeft.setLoc(nLocX, nLocY);
    bb.topRight.setLoc(MAX(1, nLocX + nWidth)-1, nLocY);
    bb.botLeft.setLoc(nLocX, MAX(1, nLocY + nHeight)-1);
    bb.botRight.setLoc(MAX(1, nLocX + nWidth)-1, MAX(1, nLocY + nHeight)-1);
    return bb;
}


//convert ofrom boundingbox
template<class _T> 
void Rect<_T>::fromBoundingBox(const BoundingBox<_T> &bb)    
{
    if (bb.isValid())
        set(bb.topLeft.x, bb.topLeft.y, bb.botRight.x-bb.topLeft.x+1, 
                        bb.botRight.y-bb.topLeft.y+1);
    else
        set(bb.topLeft.x, bb.topLeft.y, 0, 0);
}


//reload operators 
template<class _T> 
Rect<_T> &Rect<_T>::operator*=(const float32_t s) 
{
    nLocX = (_T)((float32_t)nLocX*s);
    nLocY = (_T)((float32_t)nLocY*s);
    nWidth = (_T)((float32_t)nWidth*s);
    nHeight = (_T) ((float32_t)nHeight*s);
    return *this;
}

template<class _T> 
const Rect<_T> Rect<_T>::operator*(const float32_t s) const
{    
    return Rect<_T>(*this) *= s;
}


template<class _T> 
const Rect<_T> Rect<_T>::operator+(const Rect<_T> &r) const
{
    return Rect<_T>(*this) += r;
}

template<class _T> 
Rect<_T> &Rect<_T>::operator+=(const Rect<_T> &r) 
{
    nLocX += r.nLocX;
    nLocY += r.nLocY;
    nWidth += r.nWidth;
    nHeight += r.nHeight;
    return *this;
}


//average two rectangle by weights, normalize weight so that they sum to 1
template<class _T> 
Rect<_T> Rect<_T>::average(const Rect<_T> &r1, float32_t w1, const Rect<_T> &r2, float32_t w2, bool normalize, bool roundXY, bool roundWH)     
{
    float32_t normW1, normW2;
    if (normalize)
    {
        normW1 = w1/(w1+w2);
        normW2 = 1-normW1;
    }
    else
    {
        normW1 = w1; 
        normW2 = w2;
    }

    Rect<_T> avgRect;
    
    if (roundXY)
    {
        avgRect.nLocX = (_T)Round( (float32_t) r1.nLocX * normW1 + (float32_t)r2.nLocX * normW2 );
        avgRect.nLocY = (_T)Round( (float32_t) r1.nLocY * normW1 + (float32_t)r2.nLocY * normW2 );
    }
    else
    {
        avgRect.nLocX = (_T)((float32_t) r1.nLocX * normW1 + (float32_t)r2.nLocX * normW2);
        avgRect.nLocY = (_T)((float32_t) r1.nLocY * normW1 + (float32_t)r2.nLocY * normW2);
    }

    if (roundWH)
    {
        avgRect.nWidth = (_T)Round( (float32_t) r1.nWidth * normW1 + (float32_t)r2.nWidth * normW2  ) ;
        avgRect.nHeight = (_T)Round( (float32_t) r1.nHeight * normW1 + (float32_t)r2.nHeight * normW2 ) ;
    }
    else
    {
        avgRect.nWidth = (_T) ((float32_t) r1.nWidth * normW1 + (float32_t)r2.nWidth * normW2);
        avgRect.nHeight = (_T) ((float32_t) r1.nHeight * normW1 + (float32_t)r2.nHeight * normW2);
    }

    return avgRect;
}


//average a set of rectangles using different weights
template<class _T> 
Rect<_T> Rect<_T>::average(const std::vector<Rect<_T> > &rects, const std::vector<float32_t> &weights, bool normalize, bool roundXY, bool roundWH)
{   
    float32_t x=0.0f;
    float32_t y=0.0f; 
    float32_t w=0.0f; 
    float32_t h=0.0f;
    
    for (uint32_t i=0;i<rects.size();i++)
    {
        x += (float32_t)rects[i].nLocX * weights[i];
        y += (float32_t)rects[i].nLocY * weights[i];
        w += (float32_t)rects[i].nWidth* weights[i];
        h += (float32_t)rects[i].nHeight* weights[i];
    }

    if (normalize)
    {
        float32_t weightSum=0;
        for (uint32_t i=0;i<weights.size();i++)
                weightSum += weights[i];        
        x /= weightSum;
        y /= weightSum;
        w /= weightSum;
        h /= weightSum;
    }
    
    //round the results 
    if (roundXY)
    {
        x = (float32_t)Round(x);
        y = (float32_t)Round(y);
    }
    if (roundWH)
    {
        w = (float32_t)Round(w);
        h = (float32_t)Round(h);
    }
    Rect<_T> avgRect((_T)x, (_T)y, (_T)w, (_T)h);
    return avgRect;
}

//average a vector of rectangles
template<class _T> 
Rect<_T> Rect<_T>::average(const std::vector<Rect<_T> > &r, bool roundXY, bool roundWH)     
{
    Rect<_T> avgRect;
    
    for (uint32_t i=0;i<r.size();i++)
    {
        avgRect.nLocX += r[i].nLocX;
        avgRect.nLocY += r[i].nLocY;
        avgRect.nWidth += r[i].nWidth;
        avgRect.nHeight += r[i].nHeight;
    }

    if (roundXY)
    {
        avgRect.nLocX = (_T)Round( (float32_t)avgRect.nLocX / (float32_t)r.size());
        avgRect.nLocY = (_T)Round( (float32_t)avgRect.nLocY / (float32_t)r.size());
    }
    else
    {
        avgRect.nLocX /= (_T)r.size();
        avgRect.nLocY /= (_T)r.size();
    }
    
    if(roundWH)
    {
        avgRect.nWidth =  (_T)Round( (float32_t)avgRect.nWidth  / (float32_t)r.size());
        avgRect.nHeight = (_T)Round( (float32_t)avgRect.nHeight / (float32_t)r.size());
    }
    else
    {
        avgRect.nWidth /= (_T)r.size();
        avgRect.nHeight /= (_T)r.size();
    }

    return avgRect;

}

//write the Rectangle into a ascii file
//in format: nLocX nLocY nWidth  nheight
//template<class _T> 
//void Rect<_T>::write(std::ofstream &outfile) const
//{                    
//    //outfile << std::to_string(nLocX) << " \t"; 
//    //outfile << std::to_string(nLocY) << " \t"; 
//    //outfile << std::to_string(nWidth) << " \t"; 
//    //outfile << std::to_string(nHeight) << " \t"; 
//}
    

template<class _T> 
void Rect<_T>::write(const char *filename) const
{
    //std::ofstream outfile;
    //outfile.open(filename, std::ios::out);
    //write(outfile);
    //outfile.close();
}


}   // namespace qcv

#endif // qcv_Rect_h_

