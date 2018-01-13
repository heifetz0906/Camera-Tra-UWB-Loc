#ifndef qcv_ColorFormat_h__
#define qcv_ColorFormat_h__

#include "qcvTypes.h"

namespace qcv
{

// [3...ColorCategory...0] = 4 bits. At most 16 color categories.
enum class ColorCategory : Uint32
{
    Null        = 0x0,  // invalid

    Grayscale   = 0x1,  // 
    RGB         = 0x2,  // Red, Green, Blue
    RGBA        = 0x3,  // Red, Green, Blue, and Alpha (for blending)
    YUV         = 0x4,  // 
    UV          = 0x5,  // 2-channel rip of YUV
    CMYK        = 0x6,  // 
    HSV         = 0x7,
    Lab         = 0x8,
    XYZ         = 0x9,

    Matrix      = Grayscale,    // colorless: grayscale

    __Max       = 0xf
};

//constexpr unsigned int ComposeColorSpace(ColorCategory cct, unsigned int channels) { return ; }
#define ComposeColorSpace(cc, channels)   ( (((qcv::Uint32)(cc)) << 3) | (qcv::Uint32)(channels) )

// [6...ColorCategory...3][2...ChannelCount...0] = 7 bits. 3 bits for channel count: At most 7 channels.
// ColorSpace is channel order neutral.
enum class ColorSpace : Uint32
{
    Null        = ComposeColorSpace(ColorCategory::Null, 0),    // invalid

    Grayscale   = ComposeColorSpace(ColorCategory::Grayscale, 1),
    RGB         = ComposeColorSpace(ColorCategory::RGB, 3),
    RGBA        = ComposeColorSpace(ColorCategory::RGBA, 4),
    YUV         = ComposeColorSpace(ColorCategory::YUV, 3),
    UV          = ComposeColorSpace(ColorCategory::UV, 2),
    CMYK        = ComposeColorSpace(ColorCategory::YUV, 4),
    HSV         = ComposeColorSpace(ColorCategory::HSV, 3),
    Lab         = ComposeColorSpace(ColorCategory::Lab, 3),
    XYZ         = ComposeColorSpace(ColorCategory::XYZ, 3),

    Matrix1     = ComposeColorSpace(ColorCategory::Matrix, 1),  // 1-channel matrix: each matrix element has 1 channel.  plain 2d matrix
    Matrix2     = ComposeColorSpace(ColorCategory::Matrix, 2),  // 2-channel matrix: each matrix element has 2 channels. say 2d matrix of elements (x,y)
    Matrix3     = ComposeColorSpace(ColorCategory::Matrix, 3),  // 3-channel matrix: each matrix element has 3 channels. say 2d matrix of elements (x,y,z)
    Matrix4     = ComposeColorSpace(ColorCategory::Matrix, 4),  // 4-channel matrix: each matrix element has 4 channels. say 2d matrix of elements (x,y,z,t)
};

// return category of color space
inline ColorCategory    getColorCategory(ColorSpace cs) { return static_cast<ColorCategory>((( ((Uint32)cs) >> 3 ) && 0xf)); }
// return channel count
inline size_t           getChannelCount(ColorSpace cs)  { return (size_t)(((Uint32)cs) & 0x7); }

// each channel takes 3 bits to represent its index into the image planes. Up to 8 channels: 24 bits.
#define ComposeChanneIndices1(ch0i)                             ( ((qcv::Uint32)(ch0i) << 0) )
#define ComposeChanneIndices2(ch0i, ch1i)                       ( ((qcv::Uint32)(ch1i) << 3) | ComposeChanneIndices1(ch0i) )
#define ComposeChanneIndices3(ch0i, ch1i, ch2i)                 ( ((qcv::Uint32)(ch2i) << 6) | ComposeChanneIndices2(ch0i, ch1i) )
#define ComposeChanneIndices4(ch0i, ch1i, ch2i, ch3i)           ( ((qcv::Uint32)(ch3i) << 9) | ComposeChanneIndices3(ch0i, ch1i, ch2i) )
#define ComposeChanneIndices5(ch0i, ch1i, ch2i, ch3i, ch4i)     ( ((qcv::Uint32)(ch4i) <<12) | ComposeChanneIndices4(ch0i, ch1i, ch2i, ch3i) )


#define ComposeColorFormat(cs, chis)    (  (((qcv::Uint32)(cs)) << 24) | (qcv::Uint32)(chis)  )

// [31...ColorCategory...28][27...ChannelCount...25][24...ChannelIndices...0]
enum class ColorFormat : Uint32
{
    Null        = ComposeColorFormat(ColorSpace::Null, ComposeChanneIndices1(0)),   // invalid

    Grayscale   = ComposeColorFormat(ColorSpace::Grayscale, ComposeChanneIndices1(0)),
    RGB         = ComposeColorFormat(ColorSpace::RGB, ComposeChanneIndices3(0,1,2)),
    BGR         = ComposeColorFormat(ColorSpace::RGB, ComposeChanneIndices3(2,1,0)),
    RGBA        = ComposeColorFormat(ColorSpace::RGBA, ComposeChanneIndices4(0,1,2,3)),
    BGRA        = ComposeColorFormat(ColorSpace::RGBA, ComposeChanneIndices4(2,1,0,3)),
    YUV         = ComposeColorFormat(ColorSpace::YUV, ComposeChanneIndices3(0,1,2)), //YCbCr
    YVU         = ComposeColorFormat(ColorSpace::YUV, ComposeChanneIndices3(0,2,1)), //YCrCb
    UV          = ComposeColorFormat(ColorSpace::UV, ComposeChanneIndices2(0,1)),
    CMYK        = ComposeColorFormat(ColorSpace::CMYK, ComposeChanneIndices4(0,1,2,3)),
    HSV         = ComposeColorFormat(ColorSpace::HSV, ComposeChanneIndices3(0,1,2)),
    Lab         = ComposeColorFormat(ColorSpace::Lab, ComposeChanneIndices3(0,1,2)),
    XYZ         = ComposeColorFormat(ColorSpace::XYZ, ComposeChanneIndices3(0,1,2)),

    // all matrix color formats assume the in-order channels.
    Matrix1     = ComposeColorFormat(ColorSpace::Matrix1, ComposeChanneIndices1(0)),
    Matrix2     = ComposeColorFormat(ColorSpace::Matrix2, ComposeChanneIndices2(0,1)),
    Matrix3     = ComposeColorFormat(ColorSpace::Matrix3, ComposeChanneIndices3(0,1,2)),
    Matrix4     = ComposeColorFormat(ColorSpace::Matrix4, ComposeChanneIndices4(0,1,2,3)),
};

// if the image is a planar image.
//inline bool                 isPlanar(ColorFormat cf)                { return ((Uint32)cf) & 0x80000000; }

// set or clear the planar flag
//inline ColorFormat          setPlanar(ColorFormat cf, bool planar)  { return (ColorFormat)(planar? (((Uint32)cf) | 0x80000000) : (((Uint32)cf) & 0x7fffffff)); }

// get the color space. planar flag and channel order are removed. Use this to check if the two images are compatible.
inline ColorSpace           getColorSpace(ColorFormat cf)           { return (ColorSpace) ( (((Uint32)cf) >> 24) & 0x7f ); }

// get the color category.
inline ColorCategory        getColorCategory(ColorFormat cf)        { return getColorCategory(getColorSpace(cf)); }

// get the color space. planar flag and channel order are removed. Use this to check if the two images are compatible.
inline size_t               getChannelCount(ColorFormat cf)         { return getChannelCount(getColorSpace(cf)); }

// get the index of channel/plane in the image memory layout.
// Use the memory channel index to write the code that is channel order neutral, e.g., same code works for both ColorFormat::RGB and ColorFormat::BGR.
// assert( getMemoryChannelIndex(ColorFormat::RGB,0)==0 );  // ColorFormat::RGB's ColorSpace's 0-th channel, the R channel, is laid out in memory as the 0-th channel.
// assert( getMemoryChannelIndex(ColorFormat::BGR,0)==2 );  // ColorFormat::BGR's ColorSpace's 0-th channel, the R channel, is laid out in memory as the 2-nd channel.
inline size_t               getMemoryChannelIndex(ColorFormat cf, size_t channelIndex)   { return (size_t) ( (((Uint32)cf) >> (3*channelIndex)) & 0x7 ); }

}   // namespace qcv

#endif // qcv_ColorFormat_h__
