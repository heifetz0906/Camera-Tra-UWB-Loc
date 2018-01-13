#ifndef FASTCV__H
#define FASTCV__H

/**=============================================================================

@file
   fastcv_.h

@breif
   Internal header file


Copyright (c) 2010,2012-2014 Qualcomm Technologies Incorporated.
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

/** ============================================================================
@mainpage FastCV

@section Overview Overview

   FastCV implements certain computer vision algorithms for specific HW units
   that yield an acceleration over the version running on the main CPU.

@section Interface_Guidelines Interface Guidelines

   - Platform independent reference implementation
   - CV functions contained are "stateless" and will not keep any internal data.
   - Variable naming: input variable names, unless otherwise necessary, will be
     width, height, src, dst.
   - All pointers shall not point to overlapping data and shall use the
     __restrict keyword as a qualifier.
   - Unless otherwise noted, source and destination image pointers should be
     128-bit aligned to get performance benefits from HW implementation
   - Unless otherwise noted, width and heights of images should be multiples of 8
     to get performance benefits from HW implementation.
   - Return code standard: 0 on success, -1 on error.
   - Comments shall be written in Doxygen parsable format.

@subsection Coding_Guidelines Coding guidelines

   - Functions shall have "C"-style name mangling
   - Function naming, ex: fcvComponentFunctionDimensionsType()
   - Function argument shall use "const" where appropriate

@section Loading_Opt Exposing an Optimized FastCV Implementation

@subsection Method_Overview Two Potential Methods for Exposing

   There are two ways to expose an optimized version to the user/developer. The
   decision is really dependent on the usage requirement. Whether this API will
   be exposed to an individual application developer for inclusion in their
   applcation or whether it will only be provided to OEMs as inclusion to
   differentiate QCAR, built-in Android CV applications, and/or OEM CV
   applications.

@subsection Method1 Method 1: libfastcv.so is included in OEM builds

   In this case libfastcv.so exists in /system/lib and is available for QCAR,
   Android face and guesture recognition, OEM CV applications, etc to use. The
   API is published to our partners. The libfastcv.so implementation is a target
   specific binary and therefore providing an optimized implementation is
   implicit and trivial.

@subsection Method2 Method 2: libfastcvopt.so is included in OEM builds

   In this case an individual developer could access libfastcv from a number of
   places.\n
   - /system/lib
   - Include libfastcv.so in Android APK using NDK.
   - Build against fastcv.h and libfastcv.a (static library)
   - Fully open sourced libfastcv reference implementation.

   Because the developer is potentially including the library with their
   application, portability will need to be supported while still ensuring
   an optimal experience. To do this, the reference implementation (libfastcv)
   will discover the optimized implementation located in
   /system/lib/libfastcvopt.so. When loaded libfastcvopt.so will overwrite function
   pointers so that the optimized implementations are called. \n

   There is some risk to instability here in a full OSS solution as the
   developer could change function signatures, etc internally in the reference.
   One important note though is that this Method 2 is inspired by exactly what
   Intel did with OpenCV to expose an optimized implementation for IPP (Intel
   Processor Primitives).

@subsection Comparasion Comparing the two methods

   In short, Method 1 is easier to maintain and in many ways is more elegant.
   Method 2 provides an end/open developer more flexibility and ensures a
   path to optimization in this environment. It adds some risk to instability.

@section Test_Methodology FastCV API Test Methodology

  To verify API integretity and compliance, test vectors associated with each
  function are provided. These test vectors contain input values to inject
  into the function and corresponding output values to compare with returned
  results.

  Ideally each set of test vectors is bit exact. If they are not, the tolerance
  shall be documented along with justification for not being bit exact.

  In the FastCV API a short summary of the test vectors for each function shall be
  included in the description for each function. This short summary should
  include the following.
  - Where the test vectors are located (either in source control or storage).
  - The name of the script used to generate the vectors (ideally the script's
  location in source control as well).
  - A very simplified explanation of what methodology was used to generate the
  vectors. For example:\n
  FAST corners determined using the method described here,
  http://mi.eng.cam.ac.uk/~er258/work/fast.html . All corners are found using
  thresholds ranging from 35 to 65, incremented by 2 and are sorted by score.
  The 480x320 gorilla.jpg image is used as input.

@todo
   - New functionality defined:
      - Connected Component Analysis, Contour extraction
      - Image skeletonization, distance transform
      - Polygon approximation
      - Check contour convexity
      - Dense optical flow
      - fcvFeatureDifferenceOfGaussian
      - fcvFeatureLaplacianOfGaussian
      - Accum of bytes
      - Accum of bytes squared
      - MSER, http://en.wikipedia.org/wiki/Maximally_stable_extremal_regions

=============================================================================**/


//==============================================================================
// Defines
//==============================================================================

/// Minimum of two values
#define FASTCV_MIN(V1, V2)  ((V1)<(V2) ? (V1) : (V2))

/// Maximum of two values
#define FASTCV_MAX(V1, V2)  ((V1)>(V2) ? (V1) : (V2))

/// Clips the value of x between 0 and y-1
#define FASTCV_CLIP(x,y) ( (unsigned)(x) < (unsigned) (y) ? (x) : ( (x) < 0 ? 0 : (y - 1) ) )

/// Size of fixed-size array
#define FASTCV_ARRAY_SIZE( x ) ( sizeof( x ) / sizeof( x[ 0 ] ) )

/// Creates a compile time error message when ex evaluates to false or zero
/// - Inspired from Matthew Wilson's STLSoft library
#define FASTCV_STATIC_ASSERT(ex) do { typedef int ai[(ex) ? 1 : -1]; } while(0)

#ifdef __GNUC__
   #define FASTCV_HIDDEN __attribute__ ((visibility ("hidden")))   
#else
   #define FASTCV_HIDDEN   
#endif

//==============================================================================
// Included modules
//==============================================================================

#include <stddef.h>

#ifdef FASTCV_API_MGMT
//#include "platform/android/fastcvRemoteLibAndroid.h"
#endif

//==============================================================================
// Declarations
//==============================================================================
#ifdef __WOA__
#define __arm__ _ARM_
#endif /*#ifdef __WOA__*/

//------------------------------------------------------------------------------
/// @brief
///    Different supported units
//------------------------------------------------------------------------------
typedef enum
{
   /// Special case for fcvSetOperationUnit() to choose best case for each
   FASTCV_UNIT_AUTO   = (1 << 0),
   /// Central Processing Unit
   FASTCV_UNIT_ARM    = (1 << 1),
   /// VeNum Unit
   FASTCV_UNIT_VENUM  = (1 << 2),
   /// QDSP Unit
   FASTCV_UNIT_QDSP   = (1 << 3),
   /// CV FPGA
   FASTCV_UNIT_FPGA   = (1 << 4),

   FASTCV_UNIT_UNSUPPORTED,

   /// Graphics Processing Unit
   FASTCV_UNIT_GPU    = (1 << 5),
   /// Core 2D Unit
   FASTCV_UNIT_C2D    = (1 << 6),
   /// VFP
   FASTCV_UNIT_VFP    = (1 << 7),
   /// ARMv7
   FASTCV_UNIT_ARMv7  = (1 << 8),
   /// DMA
   FASTCV_UNIT_DMA    = (1 << 9),
   /// QDSP Test
   FASTCV_UNIT_QDSP_TEST = (1 << 10),
   /// Scorpion CPU (not just ARM)
   FASTCV_UNIT_SCORPION = (1 << 11),

#ifdef FASTCV_API_MGMT   
   FASTCV_UNIT_REMOTELIB   = (1 << 12)
#endif //FASTCV_API_MGMT

} FASTCV_UNIT;

//------------------------------------------------------------------------------
/// @brief
///    Different supported memory for buffers
//------------------------------------------------------------------------------
typedef enum
{
   /// To use Heap memory for buffers
   FASTCV_MEM_HEAP   = 0,
   /// To use ION memory for buffers
   FASTCV_MEM_ION   = 1,
   /// To use ION memory shared between GPU and other platforms
   FASTCV_MEM_SHARED = 2
} fcvMemOption;

//------------------------------------------------------------------------------
/// @brief
///    Define symmetry of filter kernels
//------------------------------------------------------------------------------
typedef enum
{
   /// General filter kernel assuming no symmetry
   FASTCV_KERNEL_GENERAL = 0,
   
   /// Filter kernel symmetric in the horizontal direction,
   /// i.e., kernel[x,y] = kernel[-x,y]
   FASTCV_KERNEL_X_SYMM,

   /// Filter kernel anti-symmetric in the horizontal direction,
   /// i.e., kernel[x,y] = -kernel[-x,y]
   FASTCV_KERNEL_X_ANTISYMM,

   /// Filter kernel symmetric in the vertical direction,
   /// i.e., kernel[x,y] = kernel[x,-y]
   FASTCV_KERNEL_Y_SYMM,

   /// Filter kernel anti-symmetric in the vertical direction,
   /// i.e., kernel[x,y] = -kernel[x,-y]
   FASTCV_KERNEL_Y_ANTISYMM

} fcvKernelSymmetry;

//------------------------------------------------------------------------------
/// @brief
///    Define the convolution region
//------------------------------------------------------------------------------
typedef enum
{
   /// Filter the entire input image
   FASTCV_FILTER_ALL = 0,

   /// Filter the borders only
   FASTCV_FILTER_BORDER,

   /// Filter the interior region (non-border) only
   FASTCV_FILTER_INTERIOR

} fcvFilterRegion;

#define FCV_FLT_EPSILON     1.192092896e-07F

//------------------------------------------------------------------------------
/// @brief
///    QDSP6 performance modes
//------------------------------------------------------------------------------
typedef enum
{
   // Initial Q6 Mode value
   FASTCVHW_Q6_MODE_UNDEFINED = -1,
   FASTCVHW_Q6_MODE_POWER_SAVING = 0,
   FASTCVHW_Q6_MODE_NORMAL,
   FASTCVHW_Q6_MODE_MAX_PERFORMANCE
} FASTCVHW_Q6_MODE;

//------------------------------------------------------------------------------
/// @brief
///    QDSP6 timer modes for profiling
//------------------------------------------------------------------------------
typedef enum { Q6_TIMER_START, 
               Q6_TIMER_END 
             }q6ProfileCommand;

//------------------------------------------------------------------------------
/// @brief
///   Defines a structure for containing features
//------------------------------------------------------------------------------
typedef struct
{
   ///   Descriptor
   int8_t descriptor[36];

   ///   L2 norm square of the descriptor
   int32_t normSq;

   ///   X coordinate
   uint16_t x;

   ///   Y coordinate
   uint16_t y;

   ///   Orientation angle
   ///   \n10-bit fixed-point angle around unit circle.
   ///   \nNOTE: 0 = 0 degrees and 1024 = 360 degrees.
   int16_t orientation;

   ///   Padding to make this struct 64-bit aligned
   int16_t dummy;
} fcvFeatureType;

#ifdef __WOA__
//Added for CPU features
typedef enum {
	WINDOWS_CPU_FAMILY_UNKNOWN = 0,
	WINDOWS_CPU_FAMILY_ARM,
	WINDOWS_CPU_FAMILY_X86,
	WINDOWS_CPU_FAMILY_MAX
}fcvWINDOWSCpuFamily;

enum {
    WINDOWS_CPU_ARM_FEATURE_ARMv7 = (1 << 0),
    WINDOWS_CPU_ARM_FEATURE_VFPv3 = (1 << 1),
    WINDOWS_CPU_ARM_FEATURE_NEON  = (1 << 2),
};

//------------------------------------------------------------------------------
/// @brief
///   Defines a structure for containing CPU features
//------------------------------------------------------------------------------
typedef struct
{
	fcvWINDOWSCpuFamily   _FAM; //cpuFamily
	uint64_t           _FEAT; //cpuFeatures
	uint32_t           _HEXV; //implNumber
   
} fcvcpuFeatureType;

//---------------------------------------------------------------------------
/// @brief
///   Sets HW units for all routines at run-time.  Called for initializing
///   hardware settings
///
/// @param choice
///  Automatic from device enumeration
///
/// @return
///   0 if successful.
///   999 if minmum HW requirement not met.
///   other #'s if unsuccessful.
///
/// @ingroup misc
//---------------------------------------------------------------------------

void
fcvSetCPUfeatures( fcvcpuFeatureType *cpuFeatures );
#endif /*__WOA__*/

//---------------------------------------------------------------------------
/// @brief
///   Get time now.
///
/// @return
///   Time in microseconds
//---------------------------------------------------------------------------

unsigned long long
fcvGetTime( void );

#ifdef __cplusplus
extern "C"
{
#endif

//---------------------------------------------------------------------------
/// @brief
///   Availible HW units.
///
/// @return
///   bit:
///    0 = (LSB) ARM CPU \n
///    1 = VENUM \n
///    2 = QDSP \n
///    3 = GPU \n
///    4 = 2D Core \n
///    5 = RESERVED \n
///    ... \n
///   15 = (MSB) RESERVED
//---------------------------------------------------------------------------
unsigned short
fcvAvailableHardware( fcvOperationMode mode );

//---------------------------------------------------------------------------
/// @brief
///   Select QDSP6 performance mode.
///   User should check first if QDSP6 is available.
///
/// @param mode
///   See enum for details.
//---------------------------------------------------------------------------
void
fcvSelectQDSP6Mode( FASTCVHW_Q6_MODE mode );

//---------------------------------------------------------------------------
/// @brief
///   Get the number of available processors to run
///
/// @return returns the number of available processors
//---------------------------------------------------------------------------
uint32_t fcvAvailableProcessors();

#ifdef __cplusplus
}
#endif


#ifdef WIN32
//---------------------------------------------------------------------------
/// @brief
///   Copy source string to destination string
///   A maximum of siz characters will be copied
///   Needed only for windows since on android strlcpy is used   
///
/// @return
///   number of characters copied or -1 for failure
//---------------------------------------------------------------------------
size_t 
strlcpy(char *dst, const char *src, size_t siz);

//---------------------------------------------------------------------------
/// @brief
///
/// @return
///   number of characters copied or -1 for failure
//---------------------------------------------------------------------------
size_t 
strlcat(char *dst, const char *src, size_t siz);
#endif

#ifdef __cplusplus
extern "C"
{
#endif
//---------------------------------------------------------------------------
/// @brief
///   Perform a copy from source pointer to dest pointer given size
///
/// @param dstPtr
///   Destination pointer.
///   \n\b WARNING: should be 128-bit aligned.
///
/// @param srcPtr
///   Source pointer.
///
/// @param size
///   Copy Size in bytes.
///
/// @return
///   0 = success
//---------------------------------------------------------------------------

int
fcvCopy( void * __restrict dstPtr,
         const void * __restrict srcPtr,
         size_t size );

//------------------------------------------------------------------------------
/// @brief
///   Color conversion from YUV 4:2:0 to RGB 565 with 180 rotation
///
/// @details
///   TBD.
///
/// @param src
///   8-bit image of input YUV 4:2:0 values.
///
/// @param dst
///   16-bit image of output RGB 565 values.
///   \n\b WARNING: size must match input yuv420.
///
/// @param srcWidth
///   Image width.
///
/// @param srcHeight
///   Image height.
///
/// @test
///   -# Proper interpolation of sharp chromaticity changes. A 1D color
///                     pattern with abrupt color transitions was created for testing.
///   -# Proper interpolation of smooth chromaticity changes. A 2D color
///                     pattern was created by putting primative colors (RGBY) on four
///                     corners of a 16x16 image patch and bilinearly interpolate the
///                     pixels in the middle.
///   -# Proper preservation of achromaticity. A gray image (all channels
///                     having the same intensity of 128) was used for testing.
///   -# Test the range of color variations. An image patch with (u,v)
///                     values varying from 0:16:255 was created for testing.
///   -# Test intensity variations. An image patch with constant (u,v)
///                     and intensities from 0:255 was create for testing. 4 different
///                     extreme colors in the UV space was used to create the patterns.
///   -# Test patches from real image. 50 image patches cropped from an
///                     large color image was used for the tests.
///   -# Test a relatively large (768x512) image.
///
/// @ingroup color_conversion
//------------------------------------------------------------------------------

void fcvColorYUV420toRGB565u8Rotate180( const uint8_t* __restrict src,
                          unsigned int              srcWidth,
                          unsigned int              srcHeight,
                          uint32_t*  __restrict     dst );

//------------------------------------------------------------------------------
/// @brief
///   Creates a 2D gradient image from source luminance data. This function computes
///   central differences on 3x3 neighborhood and then convolves the result with Scharr
///   kernel. The output is in interleaved format (i.e.) [dx][dy][dx][dy]....
///   \n
///   \n      [ -3  0 +3  ]              [ -3 -10 -3 ]
///   \n dx = [ -10 0 +10 ] * src   dy = [  0  0  0  ] * src
///   \n      [ -3  0 +3  ]              [ +3 +10 +3 ]
/// 
///
/// @param src
///   Input image/patch, 8-bit unsigned.
///
/// @param srcWidth
///   Width of src data to create gradient.
///
/// @param srcHeight
///   Height of src data to create gradient.
///
/// @param srcStride
///   Stride of image (i.e., how many bytes between column 0 of row 1 and
///   column 0 of row 2).
///
/// @param gradients
///   Buffer to store horizontal and vertical gradients interleavedly. Must be at least
///   (width-2)*(height-2) *2 in size.
///   \n\b NOTE: borders at top/bottom/left/right are not computed.    
/// 
/// @param gradStride
///   Stride (in bytes) of the interleaved gradients array.
///   (number of bytes between column 0 of row 1 and column 0 of row 2 in the interleaved array
///   \n\b WARNING: must be multiple of 16 ( 8 * 2-byte values ).
///
/// @ingroup image_processing
//------------------------------------------------------------------------------
void
fcvImageGradientScharrInterleaveds16( const uint8_t* __restrict  src,
                                        uint32_t               srcWidth,
                                        uint32_t               srcHeight,
                                        uint32_t               srcStride,
                                        int16_t* __restrict    gradients,
                                        uint32_t               gradStride );

//------------------------------------------------------------------------------
/// @brief
///   Color conversion from RGB888 to RGB565 or from BGR888 to BGR565 with 180 
///   rotation 
///
/// @details
///   This function performs RGB conversion from 24-bit interleaved RGB888 to 
///   16-bit interleaved RGB565, it can be used to convert 24-bit interleaved 
///   BGR888 to 16-bit interleaved BGR565 as well. The output is rotated 180
///   degree.
///
/// @param src
///   Pointer to the input RGB888 image
///   \n\b NOTE: should be 128-bit aligned.
///
/// @param srcWidth
///   Width of the input image
///   \n\b NOTE: should be a multiple of 8.
///
/// @param srcHeight
///   Height of the input image
///
/// @param srcStride
///   Stride of input RGB888 image (i.e., number of bytes between column 0 
///   of row 0 and column 0 of row 1). If left at 0, srcStride is default to 
///   srcWidth * 3.
///   \n\b NOTE: should be a multiple of 8.
///
/// @param dst
///   Output RGB565 
///   \n\b WARNING: size must match input RGB888
///   \n\b NOTE: should be 128-bit aligned.
///
/// @param dstStride
///   Stride of output RGB565 image (i.e., number of bytes between column 0 of 
///   row 0 and column 0 of row 1). If left at 0, dstStride is default to 
///   srcWidth * 2.
///   \n\b NOTE: should be a multiple of 8.
///
/// @ingroup color_conversion
//------------------------------------------------------------------------------

void
fcvColorRGB888ToRGB565Rotate180u8( const uint8_t* __restrict src,
                                   uint32_t                  srcWidth,
                                   uint32_t                  srcHeight,
                                   uint32_t                  srcStride,
                                   uint8_t* __restrict       dst,
                                   uint32_t                  dstStride );

//------------------------------------------------------------------------------
/// @brief
///   Downscale a float image by a factor of two using a 5x5 Gaussian filter kernel
///   
///   \n\b ATTENTION: This function's signature should be changed to include Stride with
///   release of library (2.0.0). Also this is a candidate to be moved to fastcv.h in the future
///
/// @details
///   Downsamples the image using a 5x5 Gaussian filter kernel.
///
/// @param src
///   Input 32-bit float image.
///   \n\b WARNING: should be 128-bit aligned.
///
/// @param srcWidth
///   Image width.
///   \n\b NOTE: should be multiple of 8
///
/// @param srcHeight
///   Image height.
///   \n\b NOTE: must be multiple of 2
///
/// @param dst
///   Output 32-bit float downscaled image of size (width / 2) x (height / 2).
///
/// @ingroup image_transform
//------------------------------------------------------------------------------

void
fcvScaleDownBy2Gaussian5x5f32( const float32_t* __restrict srcImg,
                               uint32_t width,  uint32_t height,    
                               float32_t* __restrict       dstImg );

//------------------------------------------------------------------------------
/// @brief
///   Downscale a float image by a factor of two using a 5x5 Gaussian filter kernel
///   
///   \n\b ATTENTION: This function is revised in v2 version with stride added
///
/// @details
///   Downsamples the image using a 5x5 Gaussian filter kernel.
///
/// @param src
///   Input 32-bit float image.
///   \n\b WARNING: should be 128-bit aligned.
///
/// @param srcWidth
///   Image width.
///   \n\b NOTE: should be multiple of 8
///
/// @param srcHeight
///   Image height.
///   \n\b NOTE: must be multiple of 2
///
/// @param srcStride
///   Stride of image is the number of bytes between column 0 of row 1 and
///   column 0 of row 2 in data memory. If left at 0 srcStride is default to srcWidth*sizeof(float32_t).
///   \n\b NOTE: should be a multiple of 8.
///
/// @param dst
///   Output 32-bit float downscaled image of size (width / 2) x (height / 2).
///
/// @param dstStride
///   Stride of image is the number of bytes between column 0 of row 1 and
///   column 0 of row 2 in data memory. If left at 0 dstStride is default to srcWidth*sizeof(float32_t)/2.
///   \n\b NOTE: should be a multiple of 8.
///
/// @ingroup image_transform
//------------------------------------------------------------------------------

void
fcvScaleDownBy2Gaussian5x5f32_v2( const float32_t* __restrict  src,
                                  uint32_t                     srcWidth,
                                  uint32_t                     srcHeight,
                                  uint32_t                     srcStride,
                                  float32_t* __restrict        dst,
                                  uint32_t                     dstStride );

//------------------------------------------------------------------------------
/// @brief
///   SAD values of a source image against an 16x16 patch.
///
/// @details
///   16x16 sum of ||A-B||. The template patch is swept over the entire image and
///   the results are put in dst.
///
/// @param patch
///   16x16 template
///  Required: should be 128-bit aligned
/// 
/// @param patchStride
///   Stride of the patch (in bytes) - i.e., how many bytes between column 0 of row N 
///   and column 0 of row N+1.
///   Required: should be multiple of 16
///
/// @param src
///   Reference Image.
///
/// @param srcWidth
///    Width of the src image.
///
/// @param srcHeight
///    Height of the src image.
///
/// @param srcStride
///   Stride of image (in bytes) - i.e., how many bytes between column 0 of row N 
///   and column 0 of row N+1.
///
/// @param dst
///   The dst buffer shall be at least ( width x height ) values in length.
///   Output of SAD(A,B). dst[0] correspondes to the 0,0 pixel of the template
///   aligned with the 0,0 pixel of src. The dst border values not covered by
///   entire 16x16 patch window will remain unmodified by the function. The caller
///   should either initialize these to 0 or ignore.
/// 
/// @param dstStride
///   Stride of destination (in bytes) - i.e., how many bytes between column 0 of row N 
///   and column 0 of row N+1.
///
/// @test
///   -# Test ability to handle max output values (255x256).
///   -# Test capability of localizing a random dot pattern in a scrambled
///                     image.
///   -# Real images with varying image sizes.
///
/// @ingroup object_detection
//------------------------------------------------------------------------------

void
fcvSumOfAbsoluteDiffs16x16u8( const uint8_t* __restrict patch,
                             uint32_t                    patchStride,
                             const uint8_t* __restrict src,
                             uint32_t                    srcWidth,
                             uint32_t                    srcHeight, 
                             uint32_t                    srcStride,
                             uint16_t* __restrict      dst,
                             uint32_t                    dstStride );

//---------------------------------------------------------------------------
/// @brief
///   SAD of 8x8 patch over the source pixel window
///
/// @details
///   8x8 sum of ||A-B|| for a given pixel location. Sweeps over the src from Stride
///   offset and subtracts if from the (0,0) location of patch to the entire 8x8 and 
///   the accumated sum is returned
///
/// @param patch
///   8x8 patch
///  Required: should be 128-bit aligned
/// 
/// @param patchStride
///   Stride in bytes
///   Required: should be multiple of 8
/// 
/// @param src
///   Reference Image.
///   \n\b NOTE: data should be 128-bit aligned
/// 
/// @param srcStride
///   Stride in bytes
///
/// @param imgSAD
///   Output of SAD(A,B).
/// 
/// @test
///   -# Test ability to handle max output values (255x64).
///   -# Test capability of localizing a random dot pattern in a scrambled
///                     image.
///   -# Real images with varying image sizes.
///   -# Currently a subset of the fcvSumOfAbsoluteDiffs8x8 API Test Framework
/// @ingroup object_detection
//---------------------------------------------------------------------------

uint16_t fcvSumOfAbsoluteDiff8x8u8 ( const uint8_t * __restrict patch,
                               uint32_t              patchStride,
                               const uint8_t * __restrict src,
                               uint32_t              srcStride );

//---------------------------------------------------------------------------
/// @brief
///  SAD of 16x16 patch over the source pixel window
///
/// @details
///   16x16 sum of ||A-B|| for a given pixel location. Sweeps over the src from Stride
///   offset and subtracts if from the (0,0) location of patch to the entire 16x16 and 
///   the accumated sum is returned
///
/// @param patch
///   16x16 patch
///   Required: should be 128-bit aligned
/// 
/// @param patchStride
///   Stride in bytes
///   Required: should be a multiple of 16
/// 
/// @param src
///   Reference Image.
///   \n\b NOTE: data should be 128-bit aligned
/// 
/// @param srcStride
///   Stride in bytes
///
/// @param imgSAD
///   Output of SAD(A,B).
/// 
/// @test
///   -# Test ability to handle max output values (255x256).
///   -# Test capability of localizing a random dot pattern in a scrambled
///                     image.
///   -# Real images with varying image sizes.
///   -# Currently a subset of the fcvSumOfAbsoluteDiffs16x16 API Test Framework
/// @ingroup object_detection
//---------------------------------------------------------------------------
uint16_t fcvSumOfAbsoluteDiff16x16u8 ( const uint8_t * __restrict patch,
	                              uint32_t              patchStride,
                                  const uint8_t * __restrict src, 
                                  uint32_t              srcStride );


///---------------------------------------------------------------------------
/// @brief
///   Function to run MSER with separate outputs.
///   Image width has to be greater than 50, and image height has to be greater than 5.
///   Pixels at the image boundary are not processed. If boundary pixels are important
///   for a particular application, please consider padding the input image with dummy
///   pixels of one pixel wide.
///   Here is the typical usage:
///    void *mserHandle;
///     if (fcvMserInit (width,........,&mserHandle))
///      {
///          if ( !fcvMserSplitExtu8 (mserHandle,...) )
///          {
///            // Error handle
///          }
///          fcvMserRelease(mserHandle);
///      }
/// 
/// @param mserHandle
///   The MSER Handle returned by init.
/// 
/// @param srcPtr
///   Pointer to an image array (unsigned char ) for which MSER has to be done.
/// 
/// @param srcWidth
///   Width of the source image.
/// 
/// @param srcHeight
///   Height of the source image.
/// 
/// @param srcStride
///   Stride of the source image.
/// 
/// @param pointsArraySize
///   Size of the output points Array.
///   Typical size: (# of pixels in source image) * 30
/// 
/// @param pointsArrayMinus
///   Output. This is the points in the contours of MSER minus. 
///   This is a linear array, whose memory has to be allocated by the caller.
///   Typical allocation size:  pointArraySize.
///   pointsArray[0...numPointsInContourMinus[0]-1] defines the first MSER- region, 
///   pointsArray[numPointsInContourMinus[0] .. numPointsInContourMinus[1]-1] defines 2nd MSER- region
///   and so on.
/// 
/// @param pointsArrayPlus
///   Output. This is the points in the contours of MSER plus. 
///   This is a linear array, whose memory has to be allocated by the caller.
///   Typical allocation size:  pointArraySize.
///   pointsArray[0...numPointsInContourPlus[0]-1] defines the first MSER+ region, 
///   pointsArray[numPointsInContourPlus[0] .. numPointsInContourPlus[1]-1] defines 2nd MSER+ region
///   and so on.
/// 
/// @param maxContours
///   Maximum contours allowed in MSER minus and MSER plus. 
///   If MSER minus or MSER plus detects more contours than maxContours,
///   the function will return failure. Please adjust it accordingly.
/// 
/// @param numContoursMinus
///   Output, Number of MSER minus contours in the region.
/// 
/// @param numPointsInContourMinus
///   Output, Number of points in each minus contour. This will have values filled up
///   for the first (*numContours) values. This memory has to be allocated by 
///   the caller. 
/// 
/// @param recArrayMinus 
///   Output, This is the bounding rectangle info for each minus contour.  
///   If recArray is specified as NULL, MSER will skip finding the bounding rectangles of the contours.
///   Typical allocation size: 4*maxContours.   
///   rectArrayMinus[0..3] defines the xMin, xMax, yMax, yMin positions of the first MSER minus contour, 
///   rectArrayMinus[4..7] defines the xMin, xMax, yMax, yMin positions of the second MSER minus contour, and so on. 
/// 
/// @param contourVariationMinus
///   Output, Variation for each minus contour from previous grey level.
///   This will have values filled up for the first (*numContours) values. 
///   This memory has to be allocated by the caller with size of maxContours.
/// 
/// @param contourNodeIdMinus
///   Output, Node id for each minus contour.  This will have values filled up
///   for the first (*numContours) values. This memory has to be allocated by 
///   the caller with size of maxContours 
/// 
/// @param contourNodeCounterMinus
///   Output, Node counter for each minus contour. This will have values filled up
///   for the first (*numContours) values. This memory has to be allocated by 
///   the caller with size of maxContours. 
///
/// @param numContoursPlus
///   Output, Number of MSER plus contours in the region.
/// 
/// @param numPointsInContourPlus
///   Output, Number of points in each plus contour. This will have values filled up
///   for the first (*numContours) values. This memory has to be allocated by 
///   the caller. 
/// 
/// @param recArrayPlus 
///   Output, This is the bounding rectangle info for each plus contour.  
///   If recArray is specified as NULL, MSER will skip finding the bounding rectangles of the contours.
///   Typical allocation size: 4*maxContours.   
///   rectArrayPlus[0..3] defines the xMin, xMax, yMax, yMin positions of the first MSER plus contour, 
///   rectArrayPlus[4..7] defines the xMin, xMax, yMax, yMin positions of the second MSER plus contour, and so on. 
/// 
/// @param contourVariationPlus
///   Output, Variation for each plus contour from previous grey level.
///   This will have values filled up for the first (*numContours) values. 
///   This memory has to be allocated by the caller with size of maxContours.
/// 
/// @param contourNodeIdPlus
///   Output, Node id for each plus contour.  This will have values filled up
///   for the first (*numContours) values. This memory has to be allocated by 
///   the caller with size of maxContours 
/// 
/// @param contourNodeCounterPlus
///   Output, Node counter for each plus contour. This will have values filled up
///   for the first (*numContours) values. This memory has to be allocated by 
///   the caller with size of maxContours. 
/// 
/// @return 
///   1 if successful.
///   0 if failure.
///
/// @ingroup object_detection
//------------------------------------------------------------------------------  
FASTCV_API int
fcvMserSplitExtu8 ( void *mserHandle,
      const uint8_t* __restrict srcPtr,
      uint32_t srcWidth,
      uint32_t srcHeight,
      uint32_t srcStride,
      uint32_t pointsArraySize,
      uint16_t* __restrict pointsArrayMinus,
      uint16_t* __restrict pointsArrayPlus,
      uint32_t maxContours,
      uint32_t* __restrict numContoursMinus,
      uint32_t* __restrict numPointsInContourMinus,
      uint16_t* __restrict recArrayMinus,
      uint32_t* __restrict contourVariationMinus,
      uint32_t* __restrict contourNodeIdMinus,
      uint32_t* __restrict contourNodeCounterMinus,
      uint32_t* __restrict numContoursPlus,
      uint32_t* __restrict numPointsInContourPlus,
      uint16_t* __restrict recArrayPlus,
      uint32_t* __restrict contourVariationPlus,
      uint32_t* __restrict contourNodeIdPlus,
      uint32_t* __restrict contourNodeCounterPlus);

//---------------------------------------------------------------------------
/// @brief
///   Query QDSP6 clock frequency.
///   This function involves remoting to QDSP to fetch its clock and
///   return the uin64_t clock frequency
///
/// @param mode
///   See enum for details.
//---------------------------------------------------------------------------
uint64_t fcvQ6GetClk();

//---------------------------------------------------------------------------
/// @brief
///   MxN convolution with seperable kernels for rows and columns respectively.
///   The row filter and column filter do not have to be the same.
///   They can assume different lengths.
///   The filters are not necessarily normalized.
///
/// @param src
///   Input image. Size of buffer is srcStride*srcHeight bytes.
///   \n\b NOTE: data should be 128-bit aligned
///
/// @param srcWidth
///   Image tile width.
///   \n\b WARNING: should be multiple of 8.
///
/// @param srcHeight
///   Image tile height.
///
/// @param srcStride
///   Src image stride, stride of image is the number of bytes between column 0 of 
///   row 1 and column 0 of row 2 in data memory.
///   \n\b NOTE: if 0, srcStride is set as srcWidth.
///   \n\b WARNING: should be multiple of 8, and at least as much as srcWidth if not 0.
///
/// @param xKnl
///   1-D kernel of int16_t to filter the columns.
///   \n\b NOTE: data should be 128-bit aligned
///
/// @param xKnlSize
///   1-D kernel size of the horizontal filter. Must be an odd number.
///
/// @param xKnlSymm
///   Specifies the symmetry property of the horizontal filter    
///
/// @param yKnl
///   1-D kernel of int16_t to filter the rows.
///   \n\b NOTE: data should be 128-bit aligned
///
/// @param yKnlSize
///   1-D kernel size of the vertical filter. Must be an odd number.
///
/// @param yKnlSymm
///   Specifies the symmetry property of the vertical filter    
///
/// @param dst
///   Output image of int16_t. 
///   \n\b NOTE: Size = dstStride * srcHeigth
///   \n\b NOTE: data should be 128-bit aligned
///
/// @param dstStride
///   Output stride, stride of image is the number of bytes between column 0 of 
///   row 1 and column 0 of row 2 in data memory.
///   \n\b NOTE: if 0, dstStride is set as srcWidth*sizeof(int16_t).
///   \n\b WARNING: must be multiple of 8, and at least as much as srcWidth if not 0.
///
/// @param borderType
///   Specifies how to handle the border pixels.
///
/// @param borderConst
///   Specifies the constant value for the borderType FASTCV_BORDER_CONSTANT.
///   Ignored otherwise.
///
/// @param policy
///   Specifies the conversion policy for overflow integers.
///
/// @param filterRegion
///   Defines what parts of the image to filter.
///   See fcvFilterRegion.
///
/// @return
///   FASTCV_SUCCESS if successful,
///   otherwise if unsuccessful.
///
/// @ingroup image_processing
//---------------------------------------------------------------------------

fcvStatus
fcvFilterConvSepMxNs16( const uint8_t* __restrict  src,
                   uint32_t srcWidth,
                   uint32_t srcHeight,
                   uint32_t srcStride,
                   const int16_t* __restrict xKnl,
                   uint32_t xKnlSize,
                   fcvKernelSymmetry xKnlSymm,
                   const int16_t* __restrict yKnl,
                   uint32_t yKnlSize,
                   fcvKernelSymmetry yKnlSymm,
                   int16_t* __restrict dst,
                   uint32_t  dstStride,
                   fcvBorderType borderType,
                   uint8_t borderConst,
                   fcvFilterRegion filterRegion);


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

uint64_t
probeQ6Time(q6ProfileCommand profileCmd, uint32_t _qdspLoops);

#ifdef __cplusplus
}
#endif

#endif

