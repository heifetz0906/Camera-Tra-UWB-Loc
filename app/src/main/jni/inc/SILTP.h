/**=============================================================================

@file
SILTP.h

@brief
compute scale invariant local ternary pattern on a given image

Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

#ifndef qcv_siltp_h_
#define qcv_siltp_h_


#include <cstddef>
#include <vector>

#include "qcvMath.h"
#include "qcvTypes.h"
#include "qcvMemory.h"
#include "qcvImage.h"
#include "qcvImageExtension.h"
//#include "Common/Image/qcvImageGray.h"
#include "qcvImagePlanarUtils.h"
#include "ImageFeatureExtractor.h"

namespace qcv
{
    class SILTPParam
    {        
    public: 
        SILTPParam(float32_t tauTh, uint32_t nbr_radius, uint32_t numNbrPoints) :
            tau(tauTh),
            radius(nbr_radius),
            numPoints(numNbrPoints)
        {        }

        SILTPParam(const SILTPParam &param) 
        {        
            *this = param;
        }

        //SILTP parameters 
        float32_t   tau; //0.3f;
        uint32_t    radius; // = [3, 5];
        uint32_t    numPoints; //numPoints 4 or 8                
    };
    

    /**************************************************************
    compute the scale invariant Local Ternary Pattern from a gray image patch
    *****************************************************************/
    template<class _outType>
    class SILTPFeature : public ImageFeatureExtractor < _outType, uint32_t>
    {

    public:

        //tauTh - threshold for upper and lower pattern relative to the current value. tau can be selected from 0.004 to 0.15. Smaller tau results in better discrimination but would be more sensitive to noise.
        //nbr_radius - the radius of neighborhood
        //numNbrPoints - the number of neighborhood point to compare. Only support 4 or 8. 
        SILTPFeature(const Size2D<uint32_t> &size, const SILTPParam &params) :
            winSize(size),
            param(params)            
        {
            //SILTP parameters 
            assert(param.numPoints == 4 || param.numPoints == 8);
            if (param.numPoints != 4 && param.numPoints != 8)
            {
                PRINTF("Changing the number of neighborhood points to 4!");
                param.numPoints = 4;
            }
            totalPatterns = (uint32_t)std::pow(3, param.numPoints); //the total number of hitogram bins

            //points offsets: from point to the right, then go counterclock-wise
            for (uint32_t i = 0; i < param.numPoints; i ++)
            {
                neighborOffsetX[i] = offset8ptX[i * 8 / param.numPoints] * param.radius; //the offsets of neightbor points in X
                neighborOffsetY[i] = offset8ptY[i * 8 / param.numPoints] * param.radius; //the offsets of neightbor points in Y
            }
        }

        virtual ~SILTPFeature()         {          }

        //public functions

        //the length of the feature
        uint32_t getFeatureLength(void) const
        {
            //the output is the length of the image patch size
            return winSize.w*winSize.h;
        }

        Size2D<uint32_t> getWindowSize(void) const
        {
            return winSize;
        }

        void setWindowSize(const Size2D<uint32_t> &size)
        {
            winSize = size;
        }

        uint32_t getTotalPatterns() const
        {
            return totalPatterns;
        }

        //quantize the multi-channel image 
        bool extractImageFeature(ImageExtension &imgExt, const Point2D<uint32_t> &locXY,
            FeatureVectorView<_outType> &featureVectorOutput);


    private:
        //variables
        Size2D<uint32_t> winSize;

        SILTPParam  param;
        uint32_t    totalPatterns; //the total number of LTP patterns

        //points offsets: from point to the right, then go counterclock-wise
        int32_t    neighborOffsetX[8]; //the offsets of neightbor points in X
        int32_t    neighborOffsetY[8]; //the offsets of neightbor points in Y
        static const int32_t offset8ptX[8];// = { 1, 1, 0, -1, -1, -1, 0, 1 };
        static const int32_t offset8ptY[8];// = { 0, -1, -1, -1, 0, 1, 1, 1 };
        static const int32_t codeBase[8]; // = { 1, 3, 9, 27, 81, 243, 729, 2187 };
        //static const int32_t offset8ptX[8] = { 1,  1,  0, -1, -1, -1, 0, 1 };
        //static const int32_t offset8ptY[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
        //static const int32_t offset4ptX[4] = { 1,  0, -1, 0 };
        //static const int32_t offset4ptY[4] = { 0, -1,  0, 1 };
        //static const int32_t codeBase[] = { 1, 3, 9, 27, 81, 243, 729, 2187 };
    };
    
    //define the constants
    template<class _outType>
    const int32_t SILTPFeature<_outType>::offset8ptX[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
    template<class _outType>
    const int32_t SILTPFeature<_outType>::offset8ptY[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
    //static const int32_t offset4ptX[4] = { 1,  0, -1, 0 };
    //static const int32_t offset4ptY[4] = { 0, -1,  0, 1 };
    template<class _outType>
    const int32_t SILTPFeature<_outType>::codeBase[8] = { 1, 3, 9, 27, 81, 243, 729, 2187 };

    
    //Compute the SILTP feature only for the first channel, i.e. channel[0] 
    template<class _outType>
    bool SILTPFeature<_outType>::extractImageFeature(ImageExtension &imgExt, const Point2D<uint32_t> &locXY,
        FeatureVectorView<_outType> &featureVectorOutput)
    {
        //get the image view 
        PlanarView imgView = imgExt.getImageView();

        //check the image is uint8_t
        assert(ImagePlanarUtils::isUint8(imgView, 0));
        
        //check the size to match  
        assert(featureVectorOutput.getLength() == getFeatureLength());
        if (featureVectorOutput.getLength() != getFeatureLength())
        {
            PRINTF("SILTPFeature<_outType>::extractImageFeature: Error! Different Feature Length or number of Channels!\n");
            return false;
        }

        uint32_t imgWidth = imgExt.imgWidth;
        uint32_t imgHeight = imgExt.imgHeight;
        uint32_t imStride = imgExt.imgStride;

        //check if the windiw is outside of the image
        assert(locXY.x >= 0 && locXY.y >= 0 && locXY.x + winSize.w <= imgWidth && locXY.y + winSize.h <= imgHeight);
        if (!(locXY.x >= 0 && locXY.y >= 0 && locXY.x + winSize.w <= imgWidth && locXY.y + winSize.h <= imgHeight))
        {
            PRINTF("ImageQuantizerND<_outType>::extractImageFeature: Error! window outside of the image!\n");
            return false;
        }       
                

        //check the size of the image is larger then the 2*neighborhood_size+1
        assert(imgWidth >= 2 * param.radius + 1 && imgHeight >= 2 * param.radius + 1);
        if (!(imgWidth >= 2 * param.radius + 1 && imgHeight >= 2 * param.radius + 1))
        {
            PRINTF("ImageQuantizerND<_outType>::extractImageFeature: Error! input image is too small (w<2*radius+1 or h<2*radius+1)!\n");
            return false;
        }

        _outType *outPt = featureVectorOutput.getData();
        uint8_t *inPtr = imgExt.getImage(0) + locXY.y * imStride + locXY.x;       
        int32_t iOffset[8];
        //loop through the image to quantize each pixel 
        for (int32_t i = 0; i < (int32_t)winSize.h; i++)
        {
            //considering the boundary case for Y, pre-compure the offset
            for (uint32_t pt = 0; pt < param.numPoints; pt++)
            {
                if (i + neighborOffsetY[pt] < 0)
                    iOffset[pt] = -i * (int32_t)imStride;
                else if (i + neighborOffsetY[pt] >= (int32_t)winSize.h)
                    iOffset[pt] = (winSize.h - 1 - i) * (int32_t)imStride;
                else
                    iOffset[pt] = neighborOffsetY[pt] * (int32_t)imStride;
            }

            for (int32_t j = 0; j < (int32_t)winSize.w; j++)
            {                
                //compute the upper and lower range
                float32_t lb = (1.0f - param.tau) * (float32_t)(*inPtr);
                float32_t ub = (1.0f + param.tau) * (float32_t)(*inPtr);

                //compute the code 
                _outType out = 0; 
                for (uint32_t pt = 0; pt < param.numPoints; pt++)
                {
                    uint8_t *curPtr = inPtr + iOffset[pt];
                    //considering the boundary case for X
                    if (j + neighborOffsetX[pt] < 0)
                        curPtr -= j;
                    else if (j + neighborOffsetX[pt] >= (int32_t)winSize.w)
                        curPtr += winSize.w - 1 - j; 
                    else
                        curPtr += neighborOffsetX[pt];
                    //quantize the current point by lb and ub
                    out += ((*curPtr < lb ? 1 : 0) + (*curPtr > ub ? 2 : 0)) * codeBase[pt];
                }
                //move to next pixel
                *outPt++ = out;
                inPtr++;
            }
             //move to next line
            inPtr += imStride - winSize.w;
        }
        return true;
    }


}   // namespace qcv

#endif // qcv_siltp_h_
//


