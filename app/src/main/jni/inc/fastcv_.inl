/*==============================================================================

@file
fastcv_.inl

@brief
Inline call to function table for internal API.

Copyright (c) 2010,2012-13 Qualcomm Technologies Inc.
All Rights Reserved Qualcomm Technologies Proprietary

Export of this technology or software is regulated by the U.S.
Government. Diversion contrary to U.S. law prohibited.

All ideas, data and information contained in or disclosed by
this document are confidential and proprietary information of
Qualcomm Technologies Inc. and all rights therein are expressly reserved.
By accepting this material the recipient agrees that this material
and the information contained therein are held in confidence and in
trust and will not be used, copied, reproduced in whole or in part,
nor its contents revealed in any manner to others without the express
written permission of Qualcomm Technologies Inc. 

==============================================================================*/

//==============================================================================
// Include Files
//==============================================================================

//==============================================================================
// Declarations
//==============================================================================
#ifdef ANDROID
#include <android/log.h>
#define FASTCV_ERROR_EXIT( FMT, ... ) \
    do { \
    __android_log_print( ANDROID_LOG_ERROR, "", FMT, __VA_ARGS__ );\
    exit(1); \
    } while (0)   
#else
#define FASTCV_ERROR_EXIT( FMT, ... ) \
    do { \
    fprintf( stderr, FMT, __VA_ARGS__ ); \
    exit(1); \
    } while(0)
#endif

#ifndef fcvASSERTHOOK
#define fcvASSERTHOOK( FASTCV_ASSERT_HOOK_IN, \
    FASTCV_ASSERT_HOOK_FILE, \
    FASTCV_ASSERT_HOOK_LINE ) \
    FASTCV_ERROR_EXIT( "%s@%d: %s Assertion failed\n", \
    FASTCV_ASSERT_HOOK_FILE, \
    FASTCV_ASSERT_HOOK_LINE, \
#FASTCV_ASSERT_HOOK_IN )

#define fcvASSERTHOOKv(  \
    FASTCV_ASSERT_FUNCTION_NAME ) \
    FASTCV_ERROR_EXIT( "%s: Assertion failed\n", \
    FASTCV_ASSERT_FUNCTION_NAME )
#endif

extern int
    (*fcvCopy_)
    (
    void *dstPtr,
    const void * srcPtr,
    size_t size
    );

extern void
    (*fcvFilterWaveletDenoiseu8_)
    (
    const uint8_t* __restrict ysrc,
    const uint8_t* __restrict cbsrc,
    const uint8_t* __restrict crsrc,
    unsigned int width,
    unsigned int height,
    unsigned int stride,
    uint8_t* __restrict ydst,
    uint8_t* __restrict cbdst,
    uint8_t* __restrict crdst
    );

extern void
    (*fcvFeatureExtract_v2_)
    (
    const uint8_t*srcImg,
    unsigned int width, unsigned int height,
    const uint8_t*mask,
    unsigned int maskWidth, unsigned int maskHeight,
    int32_t* thresholdPtr,
    int32_t* numFeaturesPtr,
    unsigned int maxNumFeatures,
    fcvFeatureType *features
    );
extern void
    (*fcvFeatureMatch_)
    (
    const int8_t * __restrict         nodeChildrenCenter,
    const uint32_t * __restrict       nodeChildrenInvLenQ32,
    const uint32_t * __restrict       nodeChildrenIndex,
    const uint8_t * __restrict        nodeNumChildren,
    uint32_t                          numNodes,
    const uint32_t * __restrict       leafs,
    uint32_t                          numLeafs,
    const int8_t * __restrict         featuresDBVec,
    const uint32_t * __restrict       featuresDBInvLenQ38,
    const uint16_t * __restrict       featuresDBTargetId,
    uint32_t                          numFeaturesDB,
    const int8_t * __restrict         featuresCamVec,
    const uint32_t * __restrict       featuresCamInvLenQ38,
    uint32_t                          numFeaturesCam,
    const uint16_t * __restrict       targetsToIgnore,
    uint32_t                          numTargetsToIgnore,
    uint32_t                          maxDistanceQ31,
    uint32_t * __restrict             correspondenceDBIdx,
    uint32_t * __restrict             correspondenceCamIdx,
    uint32_t * __restrict             correspondenceDistanceQ31,
    uint32_t                          maxNumCorrespondences,
    uint32_t * __restrict             numCorrespondences
    );

extern void
    (*fcvCopyTranslatedSubPixu8Q7_) 
    ( 
    const uint8_t* src, 
    int width, 
    int stride, 
    int height, 
    short dx, 
    short dy, 
    uint8_t* dst 
    );

extern void
    (*fcvCopyTranslatedSubPixu8f32_) 
    ( 
    const uint8_t* src, 
    int width, 
    int stride, 
    int height, 
    float dx, 
    float dy, 
    float* dst 
    );

extern int
    (**ppfcvClusterEuclideanu8)( const uint8_t* __restrict  points,
    int                      numPoints,  // actually not used but helpful
    int                      dim,
    int                      pointStride,
    int                      numPixels,
    int                      numClusters,
    float* __restrict        clusterCenters,
    int                      clusterCenterStride,
    float* __restrict        newClusterCenters,
    size_t* __restrict       clusterMemberCounts,
    size_t* __restrict       clusterBindings,
    float*                   sumOfClusterDistances );

extern void
    (*fcvColorYUV420toRGB565u8Rotate180_)
    (
    const  uint8_t* __restrict yuv420,
    unsigned int                       width,
    unsigned int                       height,
    uint32_t* __restrict rgb565
    );

extern int
    (**ppfcvImageDetectLineSegmentsu8)
    ( const uint8_t* __restrict src,
    unsigned int             srcWidth,
    unsigned int             srcHeight,
    unsigned int             srcStride,
    int             maxnumLineSegments,
    uint32_t*  __restrict numLineSegments,
    uint32_t*  __restrict coordBeginPoints,
    uint32_t*  __restrict coordEndPoints );


extern void
    (**ppfcvImageDetectConvexQuadsu8)
    ( const uint8_t* __restrict src,
    unsigned int             srcWidth,
    unsigned int             srcHeight,
    unsigned int             srcStride,
    int             numMaxQuads,
    float             quadEdgeQuality,
    uint32_t* __restrict numConvexQuads,
    float* __restrict coordConvexQuadsPt1,
    float* __restrict coordConvexQuadsPt2,
    float* __restrict coordConvexQuadsPt3,
    float* __restrict coordConvexQuadsPt4 );

extern void
    (**ppfcvImageGradientScharrInterleaveds16)
    ( const uint8_t* __restrict  src,
    uint32_t               srcWidth,
    uint32_t               srcHeight,
    uint32_t               srcStride,
    int16_t* __restrict    gradients,
    uint32_t               gradStride );

extern void 
    (**ppfcvColorRGB888ToRGB565Rotate180u8)
    ( 
    const uint8_t* __restrict src,
    uint32_t                  srcWidth,
    uint32_t                  srcHeight,
    uint32_t                  srcStride,
    uint8_t* __restrict       dst,
    uint32_t                  dstStride
    );

extern void
(**ppfcvSumOfAbsoluteDiffs16x16u8)
    ( const uint8_t* __restrict patch,
    unsigned int patchStride,
    const uint8_t* __restrict src,
    unsigned int srcWidth, 
    unsigned int srcHeight,
    unsigned int srcStride,
    uint16_t* __restrict dst,
    unsigned int dstStride
    );

extern uint16_t
(**ppfcvSumOfAbsoluteDiff8x8u8)
    (
    const uint8_t * __restrict patch,
    uint32_t              patchStride,
    const uint8_t * __restrict src,
    uint32_t              srcStride
    );

extern uint16_t
(**ppfcvSumOfAbsoluteDiff16x16u8)
    (
    const uint8_t * __restrict patch,
    uint32_t              patchStride,
    const uint8_t * __restrict src,
    uint32_t              srcStride
    );

extern void
(**ppfcvScaleDownBy2Gaussian5x5f32)
   (
   const float32_t* __restrict src,
   uint32_t width,
   uint32_t height,
   float32_t* __restrict dst
   );

//==============================================================================
// Function Definitions
//==============================================================================

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline
    fcvBuffer::fcvBuffer( ) : _memSize(0), _dataSize(0),
    _ptr(0), _ptr16(0),
    _memOption(FASTCV_MEM_HEAP)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline
    fcvBuffer::~fcvBuffer( void )
{
    Free(_memOption);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline
    fcvBuffer::fcvBuffer( unsigned nBytes ) :
_memSize(0), _dataSize(0), _ptr(0), _ptr16(0)
{
    Size( nBytes );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inline const void* fcvBuffer::Ptr(void) const
{
   return (void *)_ptr16;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inline void* fcvBuffer::Ptr(void)
{
   return (void *)_ptr16;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inline unsigned fcvBuffer::Size(void) const
{
   return _dataSize;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inline int fcvCopy(void *__restrict dstPtr, const void *__restrict srcPtr, size_t size)
{
   return fcvCopy_(dstPtr, srcPtr, size);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline void
    fcvFilterWaveletDenoiseu8( const uint8_t* __restrict ysrc,
    const uint8_t* __restrict cbsrc,
    const uint8_t* __restrict crsrc,
    unsigned int width, unsigned int height, unsigned int stride,
    uint8_t* __restrict ydst,
    uint8_t* __restrict cbdst,
    uint8_t* __restrict crdst )
{
#ifndef FASTCV_DISABLE_API_ENFORCEMENT
    fcvAssert( ((int)ysrc & 0xF) == 0 );   // 128-bit alignment
    fcvAssert( ((int)cbsrc & 0xF) == 0 );  // 128-bit alignment
    fcvAssert( ((int)crsrc & 0xF) == 0 );  // 128-bit alignment
    fcvAssert( (width & 0x7) == 0 );       // multiple of 8
    fcvAssert( (stride & 0x7) == 0 );      // multiple of 8
    fcvAssert( ((int)ydst & 0xF) == 0 );   // 128-bit alignment
    fcvAssert( ((int)cbdst & 0xF) == 0 );  // 128-bit alignment
    fcvAssert( ((int)crdst & 0xF) == 0 );  // 128-bit alignment
#endif

    return fcvFilterWaveletDenoiseu8_( ysrc, cbsrc, crsrc,
        width, height, stride,
        ydst, cbdst,crdst );
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

inline void
    fcvFeatureExtract( const uint8_t* __restrict srcImg,
    unsigned int width, unsigned int height,
    const uint8_t* __restrict mask,
    unsigned int maskWidth, unsigned int maskHeight,
    int32_t* __restrict thresholdPtr,
    int32_t* __restrict numFeaturesPtr,
    unsigned int maxNumFeatures,
    fcvFeatureType* __restrict features )
{
#ifndef FASTCV_DISABLE_API_ENFORCEMENT
    fcvAssert( ((int)srcImg & 0xF) == 0 );    // 128-bit alignment
    fcvAssert( ((int)features & 0x7) == 0 );  // 64-bit alignment
    fcvAssert( (width & 0x7) == 0 );          // multiple of 8
#endif
    fcvAssert( width <= 2048 );
    fcvFeatureExtract_v2_( srcImg, width, height,
        mask, maskWidth, maskHeight,
        thresholdPtr,
        numFeaturesPtr,
        maxNumFeatures, features );
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

inline void
    fcvFeatureMatch( const int8_t * __restrict         nodeChildrenCenter,
    const uint32_t * __restrict       nodeChildrenInvLenQ32,
    const uint32_t * __restrict       nodeChildrenIndex,
    const uint8_t * __restrict        nodeNumChildren,
    uint32_t                          numNodes,
    const uint32_t * __restrict       leafs,
    uint32_t                          numLeafs,
    const int8_t * __restrict         featuresDBVec,
    const uint32_t * __restrict       featuresDBInvLenQ38,
    const uint16_t * __restrict       featuresDBTargetId,
    uint32_t                          numFeaturesDB,
    const int8_t * __restrict         featuresCamVec,
    const uint32_t * __restrict       featuresCamInvLenQ38,
    uint32_t                          numFeaturesCam,
    const uint16_t * __restrict       targetsToIgnore,
    uint32_t                          numTargetsToIgnore,
    uint32_t                          maxDistanceQ31,
    uint32_t * __restrict             correspondenceDBIdx,
    uint32_t * __restrict             correspondenceCamIdx,
    uint32_t * __restrict             correspondenceDistanceQ31,
    uint32_t                          maxNumCorrespondences,
    uint32_t * __restrict             numCorrespondences )
{
    fcvFeatureMatch_( nodeChildrenCenter,
        nodeChildrenInvLenQ32,
        nodeChildrenIndex,
        nodeNumChildren,
        numNodes,
        leafs,
        numLeafs,
        featuresDBVec,
        featuresDBInvLenQ38,
        featuresDBTargetId,
        numFeaturesDB,
        featuresCamVec,
        featuresCamInvLenQ38,
        numFeaturesCam,
        targetsToIgnore,
        numTargetsToIgnore,
        maxDistanceQ31,
        correspondenceDBIdx,
        correspondenceCamIdx,
        correspondenceDistanceQ31,
        maxNumCorrespondences,
        numCorrespondences );
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inline void
    fcvCopyTranslatedSubPixu8Q7( const uint8_t* src, 
    int width, 
    int stride, 
    int height, 
    short dx, 
    short dy, 
    uint8_t* dst )
{

    fcvCopyTranslatedSubPixu8Q7_( src, width, stride, 
        height, dx, dy, dst ); 

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inline void
    fcvCopyTranslatedSubPixu8f32( const uint8_t* src, 
    int width, 
    int stride, 
    int height, 
    float dx, 
    float dy, 
    float* dst )
{

    fcvCopyTranslatedSubPixu8f32_( src, width, stride, 
        height, dx, dy, dst ); 

}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline void
    fcvColorYUV420toRGB565u8Rotate180
    (
    const uint8_t* __restrict src,
    unsigned int                       width,
    unsigned int                       height,
    uint32_t* __restrict dst
    )
{
#ifndef FASTCV_DISABLE_API_ENFORCEMENT
    fcvAssert( ((int)src & 0xF) == 0 );     // 128-bit alignment
    fcvAssert( ((int)dst & 0xF) == 0 );     // 128-bit alignment
    fcvAssert( (width & 0x3) == 0 );           // multiple of 4
#endif

    return fcvColorYUV420toRGB565u8Rotate180_( src, width, height, dst );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline int
    fcvImageDetectLineSegmentsu8( const uint8_t* __restrict src,
    unsigned int              srcWidth,
    unsigned int              srcHeight,
    unsigned int              srcStride,
    int              maxnumLineSegments,
    uint32_t* __restrict      numLineSegments,
    uint32_t* __restrict      coordBeginPoints,
    uint32_t* __restrict      coordEndPoints )
{
    return (**ppfcvImageDetectLineSegmentsu8) (src, srcWidth, srcHeight, srcStride,
        maxnumLineSegments, numLineSegments, 
        coordBeginPoints, coordEndPoints);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline void
    fcvImageDetectConvexQuadsu8( const uint8_t* __restrict src,
    unsigned int              srcWidth,
    unsigned int              srcHeight,
    unsigned int              srcStride,
    int                       numMaxQuads,
    float                     quadEdgeQuality,
    uint32_t* __restrict      numConvexQuads,
    float* __restrict         coordConvexQuadsPt1,
    float* __restrict         coordConvexQuadsPt2,
    float* __restrict         coordConvexQuadsPt3,
    float* __restrict         coordConvexQuadsPt4 )
{
    return (**ppfcvImageDetectConvexQuadsu8) ( src, srcWidth, srcHeight, srcStride,
        numMaxQuads, quadEdgeQuality, numConvexQuads,
        coordConvexQuadsPt1, coordConvexQuadsPt2,
        coordConvexQuadsPt3, coordConvexQuadsPt4);
}

inline int
    fcvClusterEuclideanu8( const uint8_t* __restrict  points,
    int                      numPoints,  // actually not used but helpful
    int                      dim,
    int                      pointStride,
    int                      numPixels,
    int                      numClusters,
    float* __restrict        clusterCenters,
    int                      clusterCenterStride,
    float* __restrict        newClusterCenters,
    size_t* __restrict       clusterMemberCounts,
    size_t* __restrict       clusterBindings,
    float*                   sumOfClusterDistances )
{
    return (**ppfcvClusterEuclideanu8)(points,numPoints,dim,pointStride,numPixels,numClusters,
        clusterCenters,clusterCenterStride,
        newClusterCenters,clusterMemberCounts,clusterBindings,
        sumOfClusterDistances);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inline void
    fcvImageGradientScharrInterleaveds16( const uint8_t* __restrict  src,
    uint32_t               srcWidth,
    uint32_t               srcHeight,
    uint32_t               srcStride,
    int16_t* __restrict    gradients,
    uint32_t               gradStride )
{
    srcStride  = (srcStride==0 ? srcWidth : srcStride);
   gradStride = (gradStride==0 ? (srcWidth-2)*2*sizeof(int16_t) : gradStride); 

#ifndef FASTCV_DISABLE_API_ENFORCEMENT
    fcvAssert( ((int)src & 0xF) == 0 );          // 128-bit alignment
    fcvAssert( ((int)gradients & 0xF) == 0 );    // 128-bit alignment
    fcvAssert( (srcWidth & 0x7) == 0 );          // multiple of 8
    fcvAssert( (srcStride & 0x7) == 0 );         // multiple of 8
    fcvAssert( (gradStride & 0xF) == 0 );        // multiple of 16 (8 values)
    fcvAssert( (srcStride >= srcWidth) );        // at least as much as width
   fcvAssert( (gradStride >=((srcWidth-2)*2*sizeof(int16_t))) );   // at least as much as 2*width values (or 4*width bytes)
#endif

    (**ppfcvImageGradientScharrInterleaveds16)( src, srcWidth, srcHeight, srcStride,
        gradients, gradStride); 
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline void
    fcvColorRGB888ToRGB565Rotate180u8( const uint8_t* __restrict src,
    uint32_t                  srcWidth,
    uint32_t                  srcHeight,
    uint32_t                  srcStride,
    uint8_t* __restrict       dst,
    uint32_t                  dstStride )
{
#ifndef FASTCV_DISABLE_API_ENFORCEMENT
   fcvAssert( ((int)src  & 0xF) == 0 );      // 128-bit alignment
   fcvAssert( ((int)dst  & 0xF) == 0 );      // 128-bit alignment
   fcvAssert( (srcWidth  & 0x7) == 0 );      // multiple of 8
   fcvAssert( (srcStride & 0x7) == 0 );      // multiple of 8
   fcvAssert( (dstStride & 0x7) == 0 );      // multiple of 8
#endif

    (**ppfcvColorRGB888ToRGB565Rotate180u8)( src, srcWidth, srcHeight, srcStride, dst, dstStride );
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inline void
    fcvSumOfAbsoluteDiffs16x16u8( const uint8_t* __restrict patch,
    unsigned int patchStride,
    const uint8_t* __restrict src,
    unsigned int srcWidth, 
    unsigned int srcHeight,
    unsigned int srcStride,
    uint16_t* __restrict dst,
    unsigned int dstStride )
{
    patchStride = (patchStride==0 ? 8 : patchStride);
    srcStride   = (srcStride==0 ? srcWidth : srcStride);
    dstStride   = (dstStride==0 ? srcWidth*2 : dstStride);

#ifndef FASTCV_DISABLE_API_ENFORCEMENT
    fcvAssert( (patch != NULL) );
    fcvAssert( (src   != NULL) );
    fcvAssert( (dst   != NULL) );
    fcvAssert( ((int)src & 0xF) == 0 );       // 128-bit alignment
    fcvAssert( ((int)dst & 0xF) == 0 );       // 128-bit alignment
    fcvAssert( (patchStride >= 16) );          // Stride is at least as much as width
    fcvAssert( (srcStride >= srcWidth) );     // Stride is at least as much as width
    fcvAssert( (dstStride >= (srcWidth*2)) ); // Stride is at least as much as 2*width
#endif

    return (**ppfcvSumOfAbsoluteDiffs16x16u8)( patch, patchStride, src, srcWidth, srcHeight, 
        srcStride, dst, dstStride );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inline uint16_t
    fcvSumOfAbsoluteDiff16x16u8(  const uint8_t * __restrict patch,
    uint32_t              patchStride,
    const uint8_t * __restrict src, 
    uint32_t              srcStride )
{

#ifndef FASTCV_DISABLE_API_ENFORCEMENT
    fcvAssert( (patch != NULL) );
    fcvAssert( (src   != NULL) );
    fcvAssert( (patchStride >= 16) );          // Stride is at least as much as width
#endif

    return (**ppfcvSumOfAbsoluteDiff16x16u8)( patch, patchStride, src, srcStride );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inline uint16_t
    fcvSumOfAbsoluteDiff8x8u8(   const uint8_t * __restrict patch,
    uint32_t              patchStride,
    const uint8_t * __restrict src, 
    uint32_t              srcStride )
{
#ifndef FASTCV_DISABLE_API_ENFORCEMENT
    fcvAssert( (patch != NULL) );
    fcvAssert( (src   != NULL) );
    fcvAssert( ((int)src & 0xF) == 0 );       // 128-bit alignment
    fcvAssert( ((int)patch & 0xF) == 0 );       // 128-bit alignment
    fcvAssert( (patchStride >= 8) );          // Stride is at least as much as width
#endif

    return (**ppfcvSumOfAbsoluteDiff8x8u8)( patch, patchStride, src, srcStride );
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inline void
    fcvScaleDownBy2Gaussian5x5f32( const float32_t* __restrict srcImg,
     uint32_t width,  uint32_t height,    
     float32_t* __restrict       dstImg )
{
#ifndef FASTCV_DISABLE_API_ENFORCEMENT
    fcvAssert( (srcImg   != NULL) );
    fcvAssert( ((int)srcImg & 0xF) == 0 );       // 128-bit alignment
    fcvAssert( (width  & 0x7) == 0 );      // multiple of 8
    fcvAssert( (height & 0x1) == 0 );      // multiple of 2
#endif
    return (**ppfcvScaleDownBy2Gaussian5x5f32)( srcImg, width, height, dstImg );
}


