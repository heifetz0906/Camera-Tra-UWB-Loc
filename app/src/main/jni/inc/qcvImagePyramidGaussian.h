/**=============================================================================

@file
qcvImagePyramidGaussian.h

@brief
Definition of a set of classes for Gaussian image pyramids

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

#ifndef qcv_ImagePyramidGaussian_h_
#define qcv_ImagePyramidGaussian_h_


#include <cstddef>
//#include <stdexcept>
#include <vector>

#include "qcvTypes.h"
#include "qcvImage.h"
#include "qcvImageExtension.h"
#include "fastcv.h"

#define IMG_PYR_NUM_OCTAVES_DEFAULT 4
#define IMG_PYR_NUM_STEPS_PER_OCTAVE_DEFAULT 3

namespace qcv
{

// Forward declaration of ImageExtension since a pointer to it exists inside ImagePyramidGaussia 
//extern class ImageExtension;    

/*********************************************************************
class ImagePyramidGaussian: class for Gaussian pyramid image
assuming uint8 image foramt for now
support multiple color channels
**********************************************************************/
class ImagePyramidGaussian 
{
public:

  //ImagePyramidGaussian();
  ImagePyramidGaussian(uint32_t imgW, uint32_t imgH, uint32_t imgStride, ColorFormat colorFormat, 
                       uint32_t nOctaves, uint32_t nStepsPerOctave);
  ImagePyramidGaussian(uint32_t imgW, uint32_t imgH, uint32_t imgStride, ColorFormat colorFormat, 
                       uint32_t nOctaves, uint32_t nStepsPerOctave, const Size2D<float32_t> &compress);
  ImagePyramidGaussian(uint32_t imgW, uint32_t imgH, uint32_t imgStride, ColorFormat colorFormat, 
                       uint32_t nOctaves, uint32_t nStepsPerOctave, const Size2D<float32_t> &compress, bool useOriginalStride);
  virtual ~ImagePyramidGaussian();

  //public member functions

  //compute the pyramid image for a give image
  void compute(const uint8_t* src);

  //get pyramid levels for scaling factor 
  std::vector<uint32_t> getPyramidLevelIndices(float32_t minScale, float32_t maxScale);

  //get the closest pyramid level Index to the scale
  uint32_t getPyramidLevelIndex(float32_t scale);

private:      
  void initialize( );
  void release();
		
public:  
  uint32_t                              baseWidth;
  uint32_t                              baseHeight;
  uint32_t                              baseStride;
  ColorFormat                           colorFormat; //the color space for each pyramid level (i.e. number of channels)

  uint32_t                              numOctaves;
  uint32_t                              stepsPerOctave;
  uint32_t                              numPyramidLevels;
  float32_t                             scaleFactor; //always greater than 1

  //Downscaling of original image for pyramid base
  Size2D<float32_t>                     scaleCompression;
  bool                                  sameStride;

  std::vector<ImageExtension>           pyrImgExt;
  std::vector<float32_t>                pyrImgAbsScaleX;
  std::vector<float32_t>                pyrImgAbsScaleY;
  std::vector<uint32_t>                 pyrImgOctiveLevel;
  std::vector<uint8_t *>                pyrImgData;

};

}   // namespace qcv
#endif