#ifndef qcv_ImageGray_h__
#define qcv_ImageGray_h__

#include <cassert>

#include "qcvImage.h"

namespace qcv
{

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Some utility functions for Gray Image (single plane, but can be of different data type)
class ImageGray
{
public:
    // plane's strides and ptr not filled.
    static PlanarView getGrayViewSkeleton(ChannelType ch, size_t width, size_t height);
    // plane's ptr not filled.
    static PlanarView getGrayViewSkeleton(ChannelType ch, size_t width, size_t height, size_t stridex);

    // get a planar gray image view describing a buffer. ch is normally ChannelType::Uint8    
    static PlanarView getGrayView(uint8_t* imgpt, ChannelType ch, size_t width, size_t height, size_t stridex);

    //set the image pointer to an gray image view, assuming the width, hight, stride doesn't change
    static inline void setGrayPtr(PlanarView &v, uint8_t* imgpt) {v.planes[0].ptr = imgpt;}

    //get gray image infomraiton 
    static inline uint8_t* getGrayPtr(const PlanarView &v) { return v.planes[0].ptr; }
    static inline size_t   getGrayWidth(const PlanarView &v) { return v.planes[0].width; }
    static inline size_t   getGrayHeight(const PlanarView &v) { return v.planes[0].height; }
    static inline size_t   getGrayStride(const PlanarView &v) { return v.planes[0].strideX; }
    
    //various format checks for Gray    
    static inline bool isGray(const PlanarView& v) {return v.getColorSpace()==ColorSpace::Grayscale;}    
    static bool isGrayUint8(const PlanarView& v) {return isGray(v) && (v.planes[0].channel == ChannelType::Uint8); }    
    
    //conversions 

    //get a planar Gray from RGB interleaved or planar buffer
    //static PlanarView convertPixelsFromRGB(const InterleavedView& rgb, PlanarView &gray);
    //static PlanarView convertPixelFromRGB(const PlanarView &rgb, PlanarView &gray);
/*
private:
    // T: channel type. It must match channel type in yuv and dst.    
    template<typename T> static void convertPixelsFromYuv422(const PlanarView& yuv, InterleavedView& dst);
    template<typename T> static void convertPixelsFromYuv420(const PlanarView& yuv, InterleavedView& dst);
    template<typename T> static void convertPixelsToYuv420(const PlanarView& src, PlanarView& yuv);
    
    template<typename T> static T YUV2R(T y, T u, T v) { return qcv::clamp0_255(y                 + 1.140*(v-128)); }
    template<typename T> static T YUV2G(T y, T u, T v) { return qcv::clamp0_255(y - 0.395*(u-128) - 0.581*(v-128)); }
    template<typename T> static T YUV2B(T y, T u, T v) { return qcv::clamp0_255(y + 2.032*(u-128)                ); }

    template<typename T> static T RGB2Y(T r, T g, T b) { return qcv::clamp0_255( 0.299*r + 0.587*g + 0.114*b      ); }
    template<typename T> static T RGB2U(T r, T g, T b) { return qcv::clamp0_255(-0.147*r - 0.289*g + 0.436*b + 128); }
    template<typename T> static T RGB2V(T r, T g, T b) { return qcv::clamp0_255( 0.615*r - 0.515*g - 0.100*b + 128); }
*/
};

// plane's strides and ptr not filled.
inline PlanarView ImageGray::getGrayViewSkeleton(ChannelType ch, size_t width, size_t height)
{
    PlanarView v;
    v.color = ColorFormat::Grayscale;
    v.planes[0] = Plane(ch, width, height, width, height);
    return v;
}

// plane's ptr not filled.
inline PlanarView ImageGray::getGrayViewSkeleton(ChannelType ch, size_t width, size_t height, size_t stridex)
{
    PlanarView v;
    v.color = ColorFormat::Grayscale;
    v.planes[0] = Plane(ch, width, height, stridex, height);
    return v;

}

//get a planar gray image view describing a buffer. ch is normally ChannelType::Uint8    
//the function will assume that the memory of imgpt is consisitent with the channel type 
inline PlanarView ImageGray::getGrayView(uint8_t* imgpt, ChannelType ch, size_t width, size_t height, size_t stridex)
{
    PlanarView v;
    v.color = ColorFormat::Grayscale;
    v.planes[0] = Plane(ch, width, height, stridex, height, imgpt);
    return v;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
template<typename T> void ImageYuv::convertPixelsFromYuv422(const PlanarView& yuv, InterleavedView& dst)
{
    assert( isYuv422(yuv) );
    assert( dst.getColorSpace()==ColorSpace::RGB ); // only support RGB right now.
    assert( dst.channel==yuv.planes[0].channel && dst.width==yuv.planes[0].width && dst.height==yuv.planes[0].height );
    assert( dst.channel==ChannelType::Uint8 );

    size_t r = dst.getMemoryChannelIndex(0);
    size_t g = dst.getMemoryChannelIndex(1);
    size_t b = dst.getMemoryChannelIndex(2);

    const Plane& y = yuv.planes[0];
    const Plane& u = yuv.planes[1];
    const Plane& v = yuv.planes[2];
    
    Byte* dline = dst.ptr;
    const Byte* yline = y.ptr;
    const Byte* uline = u.ptr;
    const Byte* vline = v.ptr;
    for(size_t iy = 0; iy<dst.height; ++iy, dline+=dst.strideY, yline+=y.strideY, uline+=u.strideY, vline+=v.strideY)
    {
        Byte* dp = dline;
        const Byte* yp = yline;
        const Byte* up = uline;
        const Byte* vp = vline;
        for(size_t ix=0; ix<dst.width; ix+=2, dp+=2*dst.strideX, yp+=2*y.strideX, up+=u.strideX, vp+=v.strideX)  // do two pixels in each iteration
        {
            T* dx = reinterpret_cast<T*>(dp);
            const T* yx = reinterpret_cast<const T*>(yp);
            const T* ux = reinterpret_cast<const T*>(up);
            const T* vx = reinterpret_cast<const T*>(vp);
            dx[r] = static_cast<T>( YUV2R(*yx, *ux, *vx) );
            dx[g] = static_cast<T>( YUV2G(*yx, *ux, *vx) );
            dx[b] = static_cast<T>( YUV2B(*yx, *ux, *vx) );

            dx = reinterpret_cast<T*>(dp+dst.strideX);
            yx = reinterpret_cast<const T*>(yp+y.strideX);
            dx[r] = static_cast<T>( YUV2R(*yx, *ux, *vx) );
            dx[g] = static_cast<T>( YUV2G(*yx, *ux, *vx) );
            dx[b] = static_cast<T>( YUV2B(*yx, *ux, *vx) );
        }
    }
}


template<typename T> void ImageYuv::convertPixelsFromYuv420(const PlanarView& yuv, InterleavedView& dst)
{
    assert( isYuv420(yuv) );
    assert( dst.getColorSpace()==ColorSpace::RGB ); // only support RGB right now.
    assert( dst.channel==yuv.planes[0].channel && dst.width==yuv.planes[0].width && dst.height==yuv.planes[0].height );
    assert( dst.channel==ChannelType::Uint8 );

    size_t r = dst.getMemoryChannelIndex(0);
    size_t g = dst.getMemoryChannelIndex(1);
    size_t b = dst.getMemoryChannelIndex(2);

    const Plane& y = yuv.planes[0];
    const Plane& u = yuv.planes[1];
    const Plane& v = yuv.planes[2];
    
    Byte* dline = dst.ptr;
    const Byte* yline = y.ptr;
    const Byte* uline = u.ptr;
    const Byte* vline = v.ptr;
    for(size_t iy = 0; iy<dst.height; iy+=2, dline+=2*dst.strideY, yline+=2*y.strideY, uline+=u.strideY, vline+=v.strideY)
    {
        Byte* dp = dline;
        const Byte* yp = yline;
        const Byte* up = uline;
        const Byte* vp = vline;
        for(size_t ix=0; ix<dst.width; ix+=2, dp+=2*dst.strideX, yp+=2*y.strideX, up+=u.strideX, vp+=v.strideX)  // do 4 pixels in each iteration
        {
            T* dx = reinterpret_cast<T*>(dp);
            const T* yx = reinterpret_cast<const T*>(yp);
            const T* ux = reinterpret_cast<const T*>(up);
            const T* vx = reinterpret_cast<const T*>(vp);
            dx[r] = static_cast<T>( YUV2R(*yx, *ux, *vx) );
            dx[g] = static_cast<T>( YUV2G(*yx, *ux, *vx) );
            dx[b] = static_cast<T>( YUV2B(*yx, *ux, *vx) );

            dx = reinterpret_cast<T*>(dp+dst.strideX);
            yx = reinterpret_cast<const T*>(yp+y.strideX);
            dx[r] = static_cast<T>( YUV2R(*yx, *ux, *vx) );
            dx[g] = static_cast<T>( YUV2G(*yx, *ux, *vx) );
            dx[b] = static_cast<T>( YUV2B(*yx, *ux, *vx) );

            dx = reinterpret_cast<T*>(dp+dst.strideY);
            yx = reinterpret_cast<const T*>(yp+y.strideY);
            dx[r] = static_cast<T>( YUV2R(*yx, *ux, *vx) );
            dx[g] = static_cast<T>( YUV2G(*yx, *ux, *vx) );
            dx[b] = static_cast<T>( YUV2B(*yx, *ux, *vx) );

            dx = reinterpret_cast<T*>(dp+dst.strideY+dst.strideX);
            yx = reinterpret_cast<const T*>(yp+y.strideY+y.strideX);
            dx[r] = static_cast<T>( YUV2R(*yx, *ux, *vx) );
            dx[g] = static_cast<T>( YUV2G(*yx, *ux, *vx) );
            dx[b] = static_cast<T>( YUV2B(*yx, *ux, *vx) );
        }
    }
}

template<typename T> void ImageYuv::convertPixelsToYuv420(const PlanarView& src, PlanarView& yuv)
{
    assert( isYuv420(yuv) );
    assert( src.getColorSpace()==ColorSpace::RGB ); // only support RGB right now.
    assert( src.isHomogeneous() );
    assert( src.planes[0].channel==yuv.planes[0].channel && src.planes[0].width==yuv.planes[0].width && src.planes[0].height==yuv.planes[0].height );
    assert( src.planes[0].channel==ChannelType::Uint8 );

    const Plane& r = src.planes[src.getMemoryChannelIndex(0)];
    const Plane& g = src.planes[src.getMemoryChannelIndex(1)];
    const Plane& b = src.planes[src.getMemoryChannelIndex(2)];

    Plane& y = yuv.planes[0];
    Plane& u = yuv.planes[1];
    Plane& v = yuv.planes[2];
    
    const Byte* rline = r.ptr;
    const Byte* gline = g.ptr;
    const Byte* bline = b.ptr;
    Byte* yline = y.ptr;
    Byte* uline = u.ptr;
    Byte* vline = v.ptr;
    for(size_t iy = 0; iy<r.height; iy+=2, rline+=2*r.strideY, gline+=2*g.strideY, bline+=2*b.strideY, yline+=2*y.strideY, uline+=u.strideY, vline+=v.strideY)
    {
        const Byte* rp = rline;
        const Byte* gp = gline;
        const Byte* bp = bline;
        Byte* yp = yline;
        Byte* up = uline;
        Byte* vp = vline;
        for(size_t ix=0; ix<r.width; ix+=2, rp+=2*r.strideX, gp+=2*g.strideX, bp+=2*b.strideX, yp+=2*y.strideX, up+=u.strideX, vp+=v.strideX)  // do 4 pixels in each iteration
        {
            T r0 = *reinterpret_cast<const T*>(rp);
            T r1 = *reinterpret_cast<const T*>(rp+r.strideX);
            T r2 = *reinterpret_cast<const T*>(rp+r.strideY);
            T r3 = *reinterpret_cast<const T*>(rp+r.strideY+r.strideX);
            T g0 = *reinterpret_cast<const T*>(gp);
            T g1 = *reinterpret_cast<const T*>(gp+g.strideX);
            T g2 = *reinterpret_cast<const T*>(gp+g.strideY);
            T g3 = *reinterpret_cast<const T*>(gp+g.strideY+g.strideX);
            T b0 = *reinterpret_cast<const T*>(bp);
            T b1 = *reinterpret_cast<const T*>(bp+b.strideX);
            T b2 = *reinterpret_cast<const T*>(bp+b.strideY);
            T b3 = *reinterpret_cast<const T*>(bp+b.strideY+b.strideX);

            // Y
            *reinterpret_cast<T*>(yp)                       = static_cast<T>( RGB2Y(r0, g0, b0) );
            *reinterpret_cast<T*>(yp+y.strideX)             = static_cast<T>( RGB2Y(r1, g1, b1) );
            *reinterpret_cast<T*>(yp+y.strideY)             = static_cast<T>( RGB2Y(r2, g2, b2) );
            *reinterpret_cast<T*>(yp+y.strideY+y.strideX)   = static_cast<T>( RGB2Y(r3, g3, b3) );

            // U & V
            T rr = (T)( (r0+r1+r2+r3)/4 );
            T gg = (T)( (g0+g1+g2+g3)/4 );
            T bb = (T)( (b0+b1+b2+b3)/4 );
            *reinterpret_cast<T*>(up)                       = static_cast<T>( RGB2U(rr, gg, bb) );
            *reinterpret_cast<T*>(vp)                       = static_cast<T>( RGB2V(rr, gg, bb) );
        }
    }
}
*/
}   // namespace qcv

#endif // qcv_ImageGray_h__
