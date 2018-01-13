/**=============================================================================

@file
qcvImageExtension.h

@brief
Definition of a set of classes for augmented images, e.g. pyramid, integral images, etc.  

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

#ifndef qcv_ImageExtension_h_
#define qcv_ImageExtension_h_


#include <cstddef>
//#include <stdexcept>
#include <vector>

#include "qcvImagePyramid.h"
//#include "qcvImagePyramidGaussian.h"
#include "qcvTypes.h"
#include "qcvImage.h"
#include "qcvMemory.h"

#include "qcvACFPyramid.h"

#include "fastcv.h"


namespace qcv
{

// Forward declaration of ImageExtension since a pointer to it exists inside ImagePyramidGaussia 
class ImagePyramidGaussian;    

/*********************************************************************
class ImageExtension
**********************************************************************/
//a set of extensions of an images, such as pyramic, integral images 
//only support Planar uint8 image, but can be multiple channel
class ACFPyramid;

class ImageExtension
{
public:

    //number of channels is defined by the colorFormat, default is single channel (grayscale)
    //assume the channel type is uint8
    ImageExtension(uint32_t width, uint32_t height, uint32_t stride, ColorFormat color = ColorFormat::Grayscale, uint32_t chIdxPydInteg=0); 
    

    virtual ~ImageExtension();
  
    void resetFlags();
  
    void setImage(uint8_t* img);
    uint8_t* getImage(uint32_t planeIdx=0) const {return origImgView.planes[planeIdx].ptr;}
  
    void setImageView(PlanarView &imview) {origImgView = imview;}
    PlanarView getImageView( ) const {return origImgView;}
  
    uint32_t getNumChannels() const {return (uint32_t)(origImgView.getChannelCount());} 

    void setPyramidLevel(int32_t pydlevels);
    //will compute a pyramid image if not ready
    ImagePyramid* getImagePyramid();
  
    //will compure the integral images if not ready
    bool getIntegralImage(uint32_t* &iimg);
    bool getIntegralImageView( PlanarView &iimgView );
  
    //uint32_t* getIntegralImage2( );
    bool getIntegralImage2( uint32_t* &iimg, uint64_t* &iimg2);
    bool getIntegralImage2View( PlanarView &iimgView, PlanarView &iimg2View);
  
    void setImagePyramidGaussian(uint32_t nOctaves, uint32_t nStepsPerOctave, 
                            const Size2D<float32_t> &compress, bool useOriginalStride); 

    ImagePyramidGaussian* getImagePyramidGaussian();
    ImagePyramidGaussian* getImagePyramidGaussianNoReadyFlagCheck();

    //convert image to HSV plane
    ImageExtension* getHSVImage();

    //write out the image 
    bool write(ofstream &out, char format = 't', uint32_t outStride = 0) const;
    
    bool write(const char *filename, char format = 't', uint32_t outStride = 0) const;    
       
    //ACF Variables and functions
	ACFPyramid* getACFPyramid();
	ACFPyramid* getACFPyramid(int32_t _num_octaves, int32_t _num_scales_per_octave, int32_t _num_scales,uint32_t _shrink, uint32_t _padWidth, uint32_t _padHeight, uint32_t _smooth);   
	   
    //void setGradients(int16_t *gx, int16_t *gy);
    //bool getGradients(int16_t * &gx, int16_t * &gy);
  
private:
    bool computePyramidImage(); //return false is unsuccessful
  
    bool computeIntegralImage(); //compute only the integral image, return false is unsuccessful
    bool computeIntegralImage2(); //compute both the integral image of pixel and pixel^2, return false if unsuccessful

    bool computeImagePyramidGaussian();  
    void releaseImagePyramidGaussian();

    bool computeHSVImage(); //conver the current image into HSV image

public:
    uint32_t                imgWidth;           //Image width
    uint32_t                imgHeight;          //Image height
    uint32_t                imgStride;          //Image stride
    uint32_t                chIdxPydInteg;      //the channel index for computing pyramid image and integral images (e.g. 0 for YUV images if we want to compute on Y)

protected:
    PlanarView              origImgView;      //the original image
  
    ImagePyramid            imgPyramid;         //image pyramid (only apply to one channel)
    bool                    imgPyramidReady;    // flag whether the pyramid is available
  
    uint32_t                *integImg;		//integral image (only apply to one channel for now)
    PlanarView              integImgView;   //the planar view of the integram image
    bool                    integImgReady;
  
    uint64_t                *integImg2;		//integral image of pixel^2 (only apply to one channel for now)
    PlanarView              integImg2View;  //the planar view of the integral image of pixel^2
    bool                    integImg2Ready;
  
    ImagePyramidGaussian    *imgPyramidGaussian;
    bool                    imgPyramidGaussianReady;

    uint8_t                 *imgHSVPlanar;  //the data of hsv planar image of the current one
    ImageExtension          *hsvExt;         //image extension of the input image 
    bool                    hsvReady;
	
	//ACF Variables and functions
	ACFPyramid* acfPyramid;
	bool acfPyramidReady;
	int32_t num_octaves, num_scales_per_octave, num_scales;
    uint32_t shrink;
    uint32_t padWidth;
    uint32_t padHeight;
    uint32_t smooth;

	bool computeACFPyramid();
	void releaseACFPyramid();

    qcvMemory               scratchBuffer;
  
};

}   // namespace qcv

#endif // qcv_ImageExtension_h_



