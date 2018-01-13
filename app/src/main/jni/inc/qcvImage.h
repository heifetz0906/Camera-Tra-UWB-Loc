/***
for the purpose of binary transportation of image objects in graph.
***/

#ifndef qcv_Image_h__
#define qcv_Image_h__

#include "qcvTypes.h"
#include "qcvMath.h"
#include "qcvChannel.h"
#include "qcvColorFormat.h"
#include "qcvAlign.h"

#include <cstddef>
//#include <stdexcept>
#include <vector>
#include <cstring>

namespace qcv
{
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

Concepts:

ColorCategory
ColorSpace
ColorFormat
Channel
Plane
InterleavedView
PlanarView

PlanarView of one channel in a PlanarView  (channel subview)
PlanarView of InterleavedView
InterleavedView of PlanarView -- when they are compatible
Subview of view (ROI subview)
MirrorX view of view
MirrorY view of view

And all the combinations of above views...

InterleavedImage
PlanarImage

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C interface
// C friendly, some compile-time types erased. Instead the types given in data at run-time.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Plane
{
    ChannelType     channel;            // channel data type of the plane
    size_t          width;              // width  of the plane in pixels
    size_t          height;             // height of the plane in pixels
    ptrdiff_t       strideX;            // may be negative: right to left
    ptrdiff_t       strideY;            // may be negative: bottom to top
    Byte*           ptr;                // pointer to first element, which is NOT necessarily Byte type: see channel

public:
    Plane() : channel(ChannelType::Uint8), width(0), height(0), strideX(0), strideY(0), ptr(nullptr) {}
    Plane(ChannelType ch, size_t w, size_t h, ptrdiff_t stridex = 0, ptrdiff_t stridey = 0, Byte* data = nullptr) 
        : channel(ch), width(w), height(h), strideX(stridex), strideY(stridey), ptr(data) {}
    Plane(const Plane& v)
        : channel(v.channel), width(v.width), height(v.height), strideX(v.strideX), strideY(v.strideY), ptr(v.ptr) {}

public:
    size_t          getChannelSize() const      { return qcv::getChannelSize(channel); }                     // how many bytes a channel takes
    size_t          getRowSize() const          { return getChannelSize() * (size_t)(width); }          // how many bytes a row takes in most compact form.
    size_t          getAlignedRowSize(size_t align) const { return Aligner::align(getRowSize(), align); }    // how many bytes a row takes in most compact form with alignment.
    size_t          getAlignedSize(size_t align)    const { return getAlignedRowSize(align)*height; }      // how many bytes a plane takes in most compact form with alignment.
    size_t          getColumnSize() const       { return getChannelSize() * (size_t)(height); }         // how many bytes a column takes in most compact form.
    size_t          getSize() const             { return getRowSize() * (size_t)(height); }             // how many bytes a plane takes in most compact form.
    bool            isRowCompact() const        { return getChannelSize() == (size_t)qcv::abs(strideX); }    // if row's elements are next to each other
    bool            isColumnCompact() const     { return getChannelSize() == (size_t)qcv::abs(strideY); }    // if column's elements are next to each other
    bool            isNormal() const            { return strideX>0 && strideY>0; }
    bool            isRowMajor() const          { return isNormal() && strideX<=strideY; }
    bool            isColumnMajor() const       { return isNormal() && strideY<=strideX; }
    bool            isCompact() const           { return isRowCompact() && getRowSize()==(size_t)qcv::abs(strideY) || isColumnCompact() && getColumnSize()==(size_t)qcv::abs(strideX); }
    bool            isCompatibleLogical(Plane& p) const { return channel==p.channel && width==p.width && height==p.height; }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// describes a view of image laid out in memory.
// Compared to PlanarView, in InterleavedView all channels share the same channel type, width, height, columnStride, rowStride.
// an InterleavedView can always be represented by an PlanarView object.
struct InterleavedView
{
    ColorFormat     color;              // color format of image
    ChannelType     channel;            // channel data type. All channels use the same channel type. Note: this is NOT pixel type.
    size_t          width;              // width  of the image in pixels
    size_t          height;             // height of the image in pixels
    ptrdiff_t       strideX;            // offset in bytes when moving to next pixel's corresponding channel in the same row. May be negative: right to left
    ptrdiff_t       strideY;            // offset in bytes when moving to next row's same pixel's corresponding channel. May be negative: bottom to top
    Byte*           ptr;                // pointer to first element's first channel. Note: this may not be the first byte of the buffer, especially when any of the strides is negative.

public:
    InterleavedView() : color(ColorFormat::Null), channel(ChannelType::Uint8), width(0), height(0), strideX(0), strideY(0), ptr(nullptr) {}
    InterleavedView(ColorFormat clr, ChannelType ch, size_t w, size_t h, ptrdiff_t stridex = 0, ptrdiff_t stridey = 0, Byte* data = nullptr) 
        : color(clr), channel(ch), width(w), height(h), strideX(stridex), strideY(stridey), ptr(data) {}
    InterleavedView(const InterleavedView& v)
        : color(v.color), channel(v.channel), width(v.width), height(v.height), strideX(v.strideX), strideY(v.strideY), ptr(v.ptr) {}

public:
    bool            isNull() const { return ColorFormat::Null==color; }

    ColorCategory   getColorCategory() const { return qcv::getColorCategory(color); }
    size_t          getChannelCount() const { return qcv::getChannelCount(color); }
    ColorSpace      getColorSpace() const { return qcv::getColorSpace(color); }
    size_t          getMemoryChannelIndex(size_t channelIndex) const { return qcv::getMemoryChannelIndex(color, channelIndex); }

    size_t          getChannelSize() const      { return qcv::getChannelSize(channel); }
    size_t          getPixelSize() const        { return getChannelSize() * getChannelCount(); }
    size_t          getRowSize() const          { return getPixelSize() * (size_t)(width); }          // how many bytes a row takes in most compact form.
    size_t          getAlignedRowSize(size_t align) const { return Aligner::align(getRowSize(), align); }          // how many bytes a row takes in most compact form with alignment.
    size_t          getAlignedSize(size_t align)    const { return getAlignedRowSize(align)*height; }      // how many bytes a view takes in most compact form with alignment.
    size_t          getColumnSize() const       { return getPixelSize() * (size_t)(height); }              // how many bytes a column takes in most compact form.
    size_t          getSize() const             { return getRowSize() * (size_t)(height); }             // how many bytes a view takes in most compact form.
    bool            isRowCompact() const        { return getPixelSize() == (size_t)qcv::abs(strideX); }    // if row's elements are next to each other
    bool            isColumnCompact() const     { return getPixelSize() == (size_t)qcv::abs(strideY); }    // if column's elements are next to each other
    bool            isNormal() const            { return strideX>0 && strideY>0; }
    bool            isRowMajor() const          { return isNormal() && strideX<=strideY; }
    bool            isColumnMajor() const       { return isNormal() && strideY<=strideX; }
    bool            isCompact() const           { return isRowCompact() && getRowSize()==(size_t)qcv::abs(strideY) || isColumnCompact() && getColumnSize()==(size_t)qcv::abs(strideX); }
    bool            isCompatibleLogical(InterleavedView& v) const { return color==v.color && channel==v.channel && width==v.width && height==v.height; }
    bool            isCompatiblePhysical(InterleavedView& v) const { return isCompatibleLogical(v) && strideX==v.strideX && strideY==v.strideY; }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const size_t        PlanarMaxPlaneCount   = 4;    // currently we only support at most 4 channels/planes.

// describes a view of image laid out in memory.
// PlanarView is more general, because each plane can have different channel type, width, height, columnStride, rowStride and ptr.
// you may get an InterleavedView object if the PlanarView object's underlying pixel buffer is compatible with that of the InterleavedView.
// PlanarView and InterleavedView are binary compatible if they contain only one plane/channel.
struct PlanarView
{
    ColorFormat     color;                  // color format of image
    Plane           planes[PlanarMaxPlaneCount];  // the actual planes you can access is limited by getChannelCount(color);
        // make sure the baseline plane of the color space (channel 0 in color space) has the full resolution. 
        // For example, YUV422, where Y width/height will be considered as view width/height.

public:
    PlanarView() : color(ColorFormat::Null) {}
    // construct a single-plane planar Grayscale view
    PlanarView(const Plane& plane) : color(ColorFormat::Grayscale) { planes[0] = plane; }
    PlanarView(const PlanarView& v) : color(v.color) { for(size_t i=0; i<qcv::getChannelCount(color); ++i) planes[i] = v.planes[i]; }

public:
    bool            isHomogeneous() const;              // if all channels have the same channel type and width/height
    bool            isInterleavedCompatible() const;    // if you possibly can describe the same view with InterleavedView (subject to color space availability)

public:
    bool            isNull() const { return ColorFormat::Null==color; }

    ColorCategory   getColorCategory() const { return qcv::getColorCategory(color); }
    size_t          getChannelCount() const { return qcv::getChannelCount(color); }
    ColorSpace      getColorSpace() const { return qcv::getColorSpace(color); }
    size_t          getMemoryChannelIndex(size_t channelIndex) const { return qcv::getMemoryChannelIndex(color, channelIndex); }

    // how many bytes a planar view takes in most compact form with alignment.
    size_t          getAlignedSize(size_t align) const;

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// View conversions

// get the view to a channel of a planar view. literal channel index in planes[]
PlanarView getChannelPlanarView(const PlanarView& pv, size_t channelIndex);

// an interleaved view always has a planar view
PlanarView getPlanarView(const InterleavedView& iv);

// a planar view may have an interleaved view; make sure PlanarView::isInterleavedCompatible(), or it will throw.
InterleavedView getInterleavedView(const PlanarView& pv);

// get a view representing a rectangular ROI of the view
InterleavedView getSubview(const InterleavedView& iv, ptrdiff_t x, ptrdiff_t y, size_t w, size_t h);

// get a view representing a rectangular ROI of the view
// the ROI is specified in the baseline plane of the color space (channel 0 in color space). 
// For example, in YUV422, where Y is baseline plane in the color space YUV. Therefore ROI is defined in Y plane, and U/V planes will get smaller ROIs.
PlanarView getSubview(const PlanarView& pv, ptrdiff_t x, ptrdiff_t y, size_t w, size_t h);

// flip X (i.e., flip along Y axis) view
InterleavedView getHorizontalMirrorView(const InterleavedView& pv);
PlanarView getHorizontalMirrorView(const PlanarView& pv);

// flip Y (i.e., flip along X axis) view
InterleavedView getVerticalMirrorView(const InterleavedView& pv);
PlanarView getVerticalMirrorView(const PlanarView& pv);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// copy pixels between views.

// make sure the two planes are logically compatible, or it will throw.
void copyPixels(const Plane& src, Plane& dst);

template<class Ts, class Td> void copyPixels(const Plane& src, Plane& dst)
{
    assert( src.width==dst.width && src.height==dst.height );
    assert( src.getChannelSize()==sizeof(Ts) );
    assert( dst.getChannelSize()==sizeof(Td) );

    const Byte* sline = src.ptr;
    Byte* dline = dst.ptr;
    for(size_t y = 0; y<src.height; ++y, sline+=src.strideY, dline+=dst.strideY)
    {
        const Byte* spixel = sline;
        Byte* dpixel = dline;
        for(size_t x = 0; x<src.width; ++x, spixel+=src.strideX, dpixel+=dst.strideX)
            *reinterpret_cast<Td*>(dpixel) = *reinterpret_cast<const Ts*>(spixel);
    }
}


template<> inline void copyPixels<Int32, Uint8>(const Plane& src, Plane& dst)
{
    assert( src.width==dst.width && src.height==dst.height );
    assert( src.getChannelSize()==sizeof(Int32) );
    assert( dst.getChannelSize()==sizeof(Uint8) );

    const Byte* sline = src.ptr;
    Byte* dline = dst.ptr;
    for(size_t y = 0; y<src.height; ++y, sline+=src.strideY, dline+=dst.strideY)
    {
        const Byte* spixel = sline;
        Byte* dpixel = dline;
        for(size_t x = 0; x<src.width; ++x, spixel+=src.strideX, dpixel+=dst.strideX)
            *reinterpret_cast<Uint8*>(dpixel) = (Uint8)clamp(*reinterpret_cast<const Int32*>(spixel), 0, 255);
    }
}


// make sure the two interleaved views are logically compatible, or it will throw.
void copyPixels(const InterleavedView& src, InterleavedView& dst);

// make sure the two planar views are logically compatible, or it will throw.
void copyPixels(const PlanarView& src, PlanarView& dst);

// make sure the two views are logically compatible, or it will throw.
void copyPixels(const InterleavedView& src, PlanarView& dst);




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Images

// a row major compact interleaved image.
// an interleaved image (not view) will use left-right and top-bottom normal order. 
// It allows you to specify row alignment (normally 4 or 8), and optionally padding bytes between pixels in a row using gapChannelCount.
class InterleavedImage
{
public:
    InterleavedImage() : m_align(-1) {}
    InterleavedImage(ColorFormat color, ChannelType channel, size_t width, size_t height, size_t rowAlign, size_t gapChannelCount = 0)
        : m_view(color, channel, width, height), m_align(rowAlign) { assert(rowAlign!=-1); allocate(m_view, rowAlign, gapChannelCount); }
    InterleavedImage(const InterleavedImage& other) { copy(other.m_view, other.m_align, other.gapChannelCount()); }
    InterleavedImage(InterleavedImage&& other) { swap(*this, other); }
    InterleavedImage& operator=(const InterleavedImage& other) { copy(other.m_view, other.m_align, other.gapChannelCount()); }
    InterleavedImage& operator=(InterleavedImage&& other) { swap(*this, other); }

    friend void swap(InterleavedImage& a, InterleavedImage& b);

public:
    // make sure this image is able to accomodate the specified view. Often next step is copyPixels().
    void allocate(const InterleavedView& view, size_t rowAlign, size_t gapChannelCount);    // does not copy pixels
    void allocate(ColorFormat color, ChannelType channel, size_t width, size_t height, size_t rowAlign, size_t gapChannelCount);

    // clone the view to this image.
    void copy(const InterleavedView& view, size_t rowAlign, size_t gapChannelCount);

    // current view of this image
    const InterleavedView&  view() const { return m_view; }

    // the row alignment of this image
    size_t                  alignment() const { return m_align; }
    size_t                  gapChannelCount() const { }

    void clear() { m_view = InterleavedView(); }
    bool empty() const { return m_view.isNull(); }

private:
    // allocate data buffer according to some view information, then fill strides and ptr of view accordingly.
    // you can add a few gap channels between pixels using gapChannelCount
    void _allocate(InterleavedView& view, size_t rowAlign, size_t gapChannelCount);

private:
    InterleavedView     m_view;
    size_t              m_align;    // row alignment in bytes.
    std::vector<Byte>   m_data; // pixels.
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// an planar image (not view) will use left-right and top-bottom normal order. 
// all channels of one plane are packed together, followed by next plane.
// a row major compact planar image.
class PlanarImage
{
public:
    PlanarImage() : m_align(-1) {}
    PlanarImage(const PlanarImage& other) { clone(other.m_view, other.m_align); }
    PlanarImage(PlanarImage&& other) { swap(*this, other); }
    PlanarImage& operator=(const PlanarImage& other) { clone(other.m_view, other.m_align); }
    PlanarImage& operator=(PlanarImage&& other) { swap(*this, other); }

    friend void swap(PlanarImage& a, PlanarImage& b);

public:
    // make sure this image is able to accomodate the specified view. this does not copy pixels.
    // often next step is copyPixels().
    void allocate(const PlanarView& view, size_t rowAlign);

    // clone the view to this image.
    void clone(const PlanarView& view, size_t rowAlign);

    // current view of this image
    const PlanarView&   view() const { return m_view; }

    // the row alignment of this image (i.e., for all planes)
    size_t              alignment() const { return m_align; }

    void clear() { m_view = PlanarView(); }
    bool empty() const { return m_view.isNull(); }

private:
    // allocate data buffer according to some view information, then fill strides and ptr of view accordingly.
    void _allocate(PlanarView& view, size_t rowAlign);

private:
    PlanarView          m_view;
    size_t              m_align;    // row alignment in bytes.
    std::vector<Byte>   m_data; // pixels.
};


}   // namespace qcv

#endif // qcv_Image_h__
