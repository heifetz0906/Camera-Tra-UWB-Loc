#ifndef qcv_Channel_h__
#define qcv_Channel_h__

#include "qcvTypes.h"

namespace qcv
{

enum class ChannelCategoryType : Uint32
{
    Uint    = 0x0,
    Int     = 0x1,
    Float   = 0x2,
    Fixed   = 0x3
};

#define ComposeChannelType(cct, width)   (((qcv::Uint32)(cct)) << 8) | (qcv::Uint32)(1 << (width)) 

/*constexpr unsigned int ComposeChannelType(ChannelCategoryType cct, unsigned int width) { return (((unsigned int)cct) << 8) | (unsigned int)(1 << width); }*/


// [31...ChannelCategoryType...8][7...ChannelSize...0]
enum class ChannelType : Uint32
{
    Uint8   = ComposeChannelType(ChannelCategoryType::Uint, 0),
    Uint16  = ComposeChannelType(ChannelCategoryType::Uint, 1),
    Uint32  = ComposeChannelType(ChannelCategoryType::Uint, 2),    
    Uint64  = ComposeChannelType(ChannelCategoryType::Uint, 3),
    
    Int8    = ComposeChannelType(ChannelCategoryType::Int, 0),
    Int16   = ComposeChannelType(ChannelCategoryType::Int, 1),
    Int32   = ComposeChannelType(ChannelCategoryType::Int, 2),
    Int64   = ComposeChannelType(ChannelCategoryType::Int, 3),

    //Float16 = ComposeChannelType(ChannelCategoryType::Float, 1),
    Float32 = ComposeChannelType(ChannelCategoryType::Float, 2),
    Float64 = ComposeChannelType(ChannelCategoryType::Float, 3),

    Fixed16 = ComposeChannelType(ChannelCategoryType::Fixed, 1),
    Fixed32 = ComposeChannelType(ChannelCategoryType::Fixed, 2),
    Fixed64 = ComposeChannelType(ChannelCategoryType::Fixed, 3)
};

// return category of channel
inline ChannelCategoryType  getChannelCategory(ChannelType cht) { return (ChannelCategoryType)(((Uint32)cht) >> 8); }
// return size of channel, in bytes
inline size_t               getChannelSize(ChannelType cht) { return (size_t)(((Uint32)cht) & 0xff); }

}   // namespace qcv

#endif // qcv_Channel_h__
