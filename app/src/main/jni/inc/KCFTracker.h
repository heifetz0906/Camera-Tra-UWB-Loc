//
/**=============================================================================

@file
KCFTracker.h

@brief
Definition of an Kernelized Correlation Filters tracker.

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

#ifndef qcv_kcftracker_h_
#define qcv_kcftracker_h_

#include <cstddef>
#include <cstdint>
#include <cassert>

#include "fastcv.h"

#include "ImageTracker.h"
#include "qcvMemory.h"
#include "qcvTypes.h"
#include "qcvRect.h"
#include "qcvImageExtension.h"
#include "Feature.h"
#include "hogFeature.h"
#include "hogFeature32dim.h"


namespace qcv
{

    /*==============================================================================================
    Kernelized Correlation Filters Tracker Component Data Type that can be config by users
    ===============================================================================================*/
    typedef enum 
    {
        KCF_Gaussian, 
        KCF_Linear 
    } KCFKernelType;

    typedef enum 
    {
        KCF_Gray, 
        KCF_HOG,
        KCF_HOGdim32
    } KCFFeatureType;

    struct KCFKernel
    {
        KCFKernelType kernel;
        float32_t gaussianSigma;
        uint32_t  poly_a;
        uint32_t poly_b;
    };

    struct KCFTrackerParams
    {
        //parameters for KCF
        float32_t           padding;           //padding = 1.5;  //extra area surrounding the target
        KCFKernel           kcfKernelParams;   //
        float32_t           lambda;            //lambda = 1e-4   //regularization 
        float32_t           outputSigmaFactor; //output_sigma_factor = 0.1;  //spatial bandwidth (proportional to target)
        float32_t           interpFactor;      //interp_factor = 0.02;
        uint32_t            cellSize;          //cell_size = 4;
        KCFFeatureType      kcfFeature;        // kcfFeature = KCF_HOG
        uint32_t            minKCFBBSize;

        //parameters for multiscale
        uint32_t            maxBboxSize[2];       // 80 //the maximum (width or height) size of the bbox; 
        uint32_t            numSearchScale;    // 3  // number of scales that the tracker will search through
        float32_t           searchScaleFactor[3]; // [1 0.97 1.03] // multiply factor for the search scales
        uint32_t            numFramePerMultiScale; //number of frame per each multiscale search 

        //parameters for re-initialize the tracker when the bbox becomes smaller than a threshold
        bool                enableRestart;     // 1(enable to re-initiallize the tracker when the bbox resize factor becomes smaller than the restartRsFactor) or 0 
        float32_t           restartRsFactor;   // 1; if track_rsfactor < restartRsFactor && enableRestart ==1, then restart the tracker
        float32_t           restartMinConf;    //the minimum confidence value for a restart

        //parameter for threshold for stopping updating the model 
        float32_t           thUpdateModel; 


    };

    class KCFTracker : public ImageTracker <float32_t>
    {

    public:
        KCFTracker(KCFTrackerParams &kcfParams,const Rect<float32_t> &initBB, ImageExtension &img);

        virtual ~KCFTracker();

        //track image patch (object) at prevBB in prevImg to BB1 in currImg. currBB and confidence are the output.    
        //return true if the tracking is performed, otherwise return false
        bool track(ImageExtension &prevImg, ImageExtension &currImg, const BoundingBox<float32_t> &prevBB, BoundingBox<float32_t> &currBB, float32_t &confidence);
        bool track(ImageExtension &prevImg, ImageExtension &currImg, const Rect<float32_t> &prevBB, Rect<float32_t> &currBB, float32_t &confidence);

        //reset the tracking template 
        void reInitialize(const Rect<float32_t> &initBB,ImageExtension &img);

        //set the flag for resetting the tracking BB
        void setFlagResetTrackingBB(bool flag) {resetTrackingBB = flag;}

        //set the flag for template update
        //void setFlagTemplateUpdate(bool flag) {templateUpdate = flag;} 

    private: 

        /*==========================================================================
        KCF Tracker Function Declarations
        ===========================================================================*/
        //Init the buffers, set the parameters for KCF tracker
        void init(const Rect<float32_t> &initBB,ImageExtension &img);
        
        //reset the memory buffer
        void ReleaseMem();

        //Predict the new BB based on the results of the optical flow tracking
        //bool ofPredict(float32_t *BB0, float32_t *BB1, Point2D<float32_t> *startPoints, Point2D<float32_t> *endPoints, int32_t numUsed);

        //Performe the tracking of one BBox
        //bool KCFTrackBB(float32_t* BB0, float32_t* BB1);
        void KCFTrackBB(); //const Rect<float32_t> &preBB, Rect<float32_t> &currBB);


        //Create the tracking results based on ofTrackBB output
        //void KCFCreatBB(Rect<float32_t> &kcfCurrBB, int32_t imgWidth, int32_t imgHeight, float32_t x, float32_t y, float32_t w, float32_t h);
        
        //functions used by KCFTrackBB
        void gaussianShapedLabels(float32_t sigma, uint32_t sizeW, uint32_t sizeH, uint32_t strideW, uint32_t strideH, float32_t* label);        
        void getFeaturesf(ImageExtension &imgExt, uint32_t featureWidth, uint32_t featureHeight,uint32_t featureWStride, uint32_t featureHStride, KCFTrackerParams &params, float32_t* cosWindow,FeatureVectorView<float32_t> &featureVectorOutput, float32_t* Featuref);
        void gaussianCorrelation(float32_t* xf, float32_t* yf, uint32_t featureWidth, uint32_t featureHeight,uint32_t featureWStride, uint32_t featureHStride, uint32_t numBin, float32_t sigma, float32_t *kf);
        void linearCorrelation(float32_t* xf, float32_t* yf, uint32_t featureWidth, uint32_t featureHeight,uint32_t featureWStride, uint32_t featureHStride, uint32_t numBin, float32_t *kf);
        void getResponse(float32_t* kzf, float32_t*modelAlphaf,uint32_t width, uint32_t height, float32_t* response);
        void findMaxResponse(float32_t* response, uint32_t width, uint32_t height, float32_t &conf, int32_t &vertDelta, int32_t &horizDelta);
        void updateKCFModel(float32_t* labelf, float32_t* kf, float32_t* Featuref, uint32_t width, uint32_t height, float32_t* modelAlphaf, float32_t* modelFeaturef,bool mode);
        void FFT2d(float32_t* matrix, int32_t fftSizeX, int32_t fftSizeY, float32_t* matrixf, int32_t mode);
        void getCosWindow(uint32_t sizeW, uint32_t sizeH, float32_t* cosWindow);
        //void polynomial_correlation();

    //member variables 
    private:    

        KCFTrackerParams params; 
        qcvMemory   scratchBuffer;

        ImageExtension *prevFrame;
        ImageExtension *currFrame;

        //float32_t     searchScale;       // rescale factor.  rsfactor = [ rsfactor rsfactor(1)*rsOff(scaleID)];
        //Point2D<float32_t> cropSize;        
        //Point2D<float32_t> cropSizeTemp;
        Rect<float32_t>    cropRect;        
        //Point2D<float32_t> cropSizeFinal;
        //Point2D<float32_t> centerPos;        

        Point2D<uint32_t> windowSize;
        Point2D<uint32_t> targetSize;
        Point2D<uint32_t> featureSize;
        Point2D<uint32_t> fftSize;

        float32_t* label;
        float32_t* labelf;
        float32_t* cosWindow;

        //HOG feature extractor
        ImageFeatureExtractor<float32_t, uint32_t> *hogExtractor;
        
        FeatureVectorView<float32_t> *Feature;
        double* HOGfeaturedim32;
        uint32_t featureLen;
        float32_t* Featuref;
        ImageExtension *resizedImg;
        uint8_t* resizedImgP;

        //model
        float32_t* modelFeaturef;
        float32_t* kf;
        float32_t* modelAlphaf;
        float32_t* response_;
        float32_t* alphaf;

        //track scale
        float32_t confScoreFinal;
        //float32_t searchScaleFinal;
        float32_t *FeatureFinalf;

        //
        uint32_t* cropImageTemp;

        //flag for updating the template
        //bool                templateUpdate;

        //flag for reset the tracking bounding box 
        bool        resetTrackingBB;

public: 
        uint32_t    numFrames;                

    };


}

#endif  //qcv_KCFTracker_h_
