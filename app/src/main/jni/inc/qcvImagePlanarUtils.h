#ifndef qcv_ImagePlanarUtils_h_
#define qcv_ImagePlanarUtils_h_

#include <cassert>

#include "qcvImage.h"

namespace qcv
{

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Some utility functions for a planar Image (can be of different channel and color types)
class ImagePlanarUtils
{
public:
    // plane's strides and ptr not filled.
    static PlanarView getViewSkeleton(ChannelType ch, size_t width, size_t height, ColorFormat color = ColorFormat::Matrix3);
    // plane's ptr not filled.
    static PlanarView getViewSkeleton(ChannelType ch, size_t width, size_t height, size_t stridex, ColorFormat color = ColorFormat::Matrix3);

    // get a 3 channel planar image view describing a buffer. ch is ChannelType::Uint8    
    static PlanarView getView(uint8_t* imgpt, size_t width, size_t height, size_t stridex, ColorFormat color = ColorFormat::Matrix3);

    //set the image pointer to a 3 channel planar image view, assuming the width, hight, stride doesn't change
    static inline void setPtr(PlanarView &v, uint8_t* imgpt); 

    //various format checks 
    static inline bool is3Ch(const PlanarView &v) {return v.getChannelCount()==3;}
    static inline bool isRGB(const PlanarView& v) {return v.getColorSpace()==ColorSpace::RGB;}    
    static inline bool isYUV(const PlanarView& v) {return v.getColorSpace()==ColorSpace::YUV;}        
    static bool isUint8(const PlanarView& v, uint32_t plane) {return v.planes[plane].channel == ChannelType::Uint8; }        

    //get image infomraiton 
    static inline uint8_t* getPlanePtru8(const PlanarView &v, uint32_t plane) {assert(isUint8(v, plane)); return v.planes[plane].ptr; } //assum uint8_t type 
    static inline size_t   getWidth(const PlanarView &v, uint32_t plane) { return v.planes[plane].width; }
    static inline size_t   getHeight(const PlanarView &v, uint32_t plane) { return v.planes[plane].height; }
    static inline size_t   getStride(const PlanarView &v, uint32_t plane) { return v.planes[plane].strideX; }

    static void convertRGB888InterleavedToRGBPlanar(const uint8_t *intImg, uint8_t *planImg, uint32_t width, uint32_t height, uint32_t intStride, uint32_t planStride, uint32_t bChannelIndex = 2);
    static void convertRGBPlanarToRGB888Interleaved(uint8_t *intImg, const uint8_t *planImg, uint32_t width, uint32_t height, uint32_t intStride, uint32_t planStride, uint32_t bChannelIndex = 2);
};

// plane's strides and ptr not filled.
inline PlanarView ImagePlanarUtils::getViewSkeleton(ChannelType ch, size_t width, size_t height, ColorFormat color)
{
    PlanarView v;
    v.color = color;    
    for(size_t i=0; i<qcv::getChannelCount(color); ++i) 
        v.planes[i] = Plane(ch, width, height, width, height);
    return v;
}

// plane's ptr not filled.
inline PlanarView ImagePlanarUtils::getViewSkeleton(ChannelType ch, size_t width, size_t height, size_t stridex, ColorFormat color)
{
    PlanarView v;
    v.color = color;    
    for(size_t i=0; i<qcv::getChannelCount(color); ++i) 
        v.planes[i] = Plane(ch, width, height, stridex, height);
    return v;
}

// get a 3 channel planar image view describing a buffer. ch is ChannelType::Uint8    
inline PlanarView ImagePlanarUtils::getView(uint8_t* imgpt, size_t width, size_t height, size_t stridex, ColorFormat color)
{
    PlanarView v;
    v.color = color;    
    for(size_t i=0; i<qcv::getChannelCount(color); ++i) 
    {
        v.planes[i] = Plane(ChannelType::Uint8, width, height, stridex, height, imgpt);
        imgpt += stridex*height*sizeof(uint8_t);
    }
    return v;
}


//set the image pointer to a 3 channel planar image view, assuming the width, hight, stride doesn't change
inline void ImagePlanarUtils::setPtr(PlanarView &v, uint8_t* imgpt)
{
    for(size_t i=0; i<v.getChannelCount(); ++i) 
    {
        assert(v.planes[i].channel == ChannelType::Uint8);
        v.planes[i].ptr = imgpt;
        imgpt += v.planes[i].strideX*v.planes[i].height; //*sizeof(uint8_t);
    }    
}


inline void ImagePlanarUtils::convertRGB888InterleavedToRGBPlanar(const uint8_t *intImg, uint8_t *planImg, uint32_t width, uint32_t height, uint32_t intStride, uint32_t planStride, uint32_t bChannelIndex)
{
    // de-interleave the image
    const uint8_t *interlRow = intImg;
    uint8_t *rrow = planImg;
    uint8_t *grow = rrow + planStride * height;
    uint8_t *brow = grow + planStride * height;
    const uint8_t *interl;
    uint8_t *r, *g, *b;
    for(uint32_t y=0;y<height;y++) 
    {
        interl = interlRow;
        r = rrow;
        g = grow;
        b = brow;
        for(uint32_t x=0; x<width; x++, r++, g++, b++, interl+=3) 
        {
            *r = interl[2-bChannelIndex];
            *g = interl[1];
            *b = interl[bChannelIndex];
        }
        interlRow += intStride;
        rrow += planStride;
        grow += planStride;
        brow += planStride;
    }
}


inline void ImagePlanarUtils::convertRGBPlanarToRGB888Interleaved(uint8_t *intImg, const uint8_t *planImg, uint32_t width, uint32_t height, uint32_t intStride, uint32_t planStride, uint32_t bChannelIndex)
{
    // de-interleave the image
    uint8_t *interlRow = intImg;
    const uint8_t *rrow = planImg;
    const uint8_t *grow = rrow + planStride * height;
    const uint8_t *brow = grow + planStride * height;
    uint8_t *interl;
    const uint8_t *r, *g, *b;
    for (uint32_t y = 0; y<height; y++)
    {
        interl = interlRow;
        r = rrow;
        g = grow;
        b = brow;
        for (uint32_t x = 0; x<width; x++, r++, g++, b++, interl += 3)
        {
            interl[2 - bChannelIndex] = *r;
            interl[1] = *g;
            interl[bChannelIndex] = *b;
        }
        interlRow += intStride;
        rrow += planStride;
        grow += planStride;
        brow += planStride;
    }
}

}   // namespace qcv

#endif // qcv_ImagePlanarUtils_h_
