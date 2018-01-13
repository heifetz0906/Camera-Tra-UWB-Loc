//
/**=============================================================================

@file
hogFeature32dim.h

@brief
Definition of an  HOG variant used by Felzenszwalb.

The computed HOG features are 3*nOrients+5 dimensional. There are
2*nOrients contrast sensitive orientation channels, nOrients contrast
insensitive orientation channels, 4 texture channels and 1 all zeros
channel (used as a 'truncation' feature). Using the standard value of
nOrients=9 gives a 32 dimensional feature vector at each cell.
For details please refer to work by Felzenszwalb et al.

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

#ifndef qcv_hogFeature32dim_h_
#define qcv_hogFeature32dim_h_


#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cstring>

#include "qcvMath.h"
#include "qcvImage.h"
#include "qcvImageExtension.h"
#include "Feature.h"
#include "ImageFeatureExtractor.h"

namespace qcv
{ 
    class hogFeature32dim : public ImageFeatureExtractor < float32_t >
    {

    public :
        hogFeature32dim(uint32_t imageWidth, uint32_t imageHeight, uint32_t imageStride, uint32_t hogCellSize, uint32_t hogBinNumber);
        ~hogFeature32dim();

        //public functions
    public:        
        //extract the feature from a location (window size fixed)
        //*Params: 
        //      locXY: the location relative to the input image pointer provided by ImgExt  
        bool extractImageFeature(ImageExtension &imgExt, const Point2D<uint32_t> &locXY, FeatureVectorView<float32_t> &featureVectorOutput);

        //the length of the feature
        uint32_t getFeatureLength(void) const {return numFeature[0]*numFeature[1]*numFeature[2];}

        // get/set the size of the image 
        Size2D<uint32_t> getWindowSize(void) const;
        void setWindowSize(const Size2D<uint32_t> &size);

    private:
        /////////////////////////////////////////////
        //internal functions    
        void init(uint32_t imageWidth, uint32_t imageHeight, uint32_t imageStride, uint32_t hogCellSize, uint32_t hogBinNumber);
        void clear();

        void constructLookUpTable(void);
        void computeGradientBlockHist(uint8_t *src);
        void computeHOG(float* hogdescriptor);

        //internal member variables
        uint32_t imageWidth;
        uint32_t imageHeight;
        uint32_t imageStride;

        uint32_t hogCellSize;
        uint32_t hogBinNumber;

        uint32_t numBlocks[2];
        uint32_t numFeature[3];
        
        //inner image
        float32_t *blockHist;
        float32_t *blockNorm;

        //look up table  
        float32_t *dx_lut;
        float32_t *dy_lut;

    };

}   // namespace qcv

#endif // qcv_hogFeature32dim_h_
//