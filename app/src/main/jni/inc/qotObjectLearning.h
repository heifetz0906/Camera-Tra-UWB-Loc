/**=============================================================================

@file
qotObjectLearning.h

@brief
qot object learning, including both the detector and the verifier

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

#ifndef qcv_qot_object_learning_h_
#define qcv_qot_object_learning_h_


#include <cstddef>
#include <vector>

#include "qcvTypes.h"
#include "qcvRect.h"
#include "qcvImageExtension.h"
#include "ImagePatchClassifier.h"
#include "ImagePatchClassifierSimplePNTraining.h"
#include "ImageDetector.h"
//#include "Feature/BriefBinaryFeature.h"
#include "BriefBinaryMultiChanFeature.h"
#include "SvmClassifier.h"
#include "SvmClassifierTraining.h"

#include "qotObjectVerification.h"
#include "qotObjectDetection.h"


namespace qcv
{
namespace qot
{

/*==========================================================================
Decision Forest Data Types
===========================================================================*/

//typedef struct _DFTrackerParams
//{
//    //thresholds for detection and learning 
//    float32_t   thresholdLearnP;          //Threshold of overlap with the obj BB as positive window bb
//    float32_t   thresholdLearnN;          //Threshold of overlap with the obj BB as negative window bb
//
//           
//    float32_t   thresholdLearnValid;      //Threshold of valid BB to be learned
//
//    float32_t   thresholdPatchVar;        //Threshold for patch variance
//
//} DFTrackerParams;





//qot object learning
//assume that detection is done for each frame, so input to the learning will be a set of positive responses over 
//the image, and the current object location 
//the learning will maintain its own model 
class qotObjectLearning
{
    typedef EnsembleClassifierSimpleModel<HistogramClassifier1DModel> HistogramClassifier1DEnsembleModel;
public: 

    qotObjectLearning(ImageExtension &imgExt, const Rect<float32_t> &initROI, const Size2D<uint32_t> &winSize);
    ~qotObjectLearning( );
    
    //learning with only the image and the object box
    //bool learn(ImageExtension &imgExt, const QOTObjectState &qotState);

    //given an input image, current object state and a vector of object detection results, update the classifiers
    bool learn(ImageExtension &imgExt, const QOTObjectState &qotState, const std::vector<DetectionScaleResult> &objDetResults);//, const std::vector<DetectionScaleResult> &objDetResultsVerified);
    
    //get object detector model, set the flags for updated model components
    qotObjectDetectionModel getObjectDetectorModel( ) const;
    
    //get object verification model 
    NCCTemplateClassifierModel<NCC_PATCH_SIZE,NCC_PATCH_SIZE, NCC_MAX_POSITIVE_PATCHES, NCC_MAX_NEGATIVE_PATCHES> 
        &getObjectVerifierModel( );

    //adjust the threshold for the obejct detector 
    bool updateDetectorBias(const QOTObjectState &objState);
        
private:     
    //given an input image, current object roi, for initial learning
    bool initialLearn(ImageExtension &imgExt, Rect<float32_t> initROI);

    bool updateDetector(ImageExtension &imgExt, const Rect<float32_t> &currBB, uint32_t numLearntFrame);
    
    //given an input image and a vector of object detection results, train the classifier 
    bool updateDetector(ImageExtension &imgExt, const Rect<float32_t> &currBB, const std::vector<DetectionScaleResult> &negBB, uint32_t numLearntFrame);

    bool detLearnPositiveSample(ImageExtension &imgExt, const Rect<float32_t> &currBB, uint32_t numLearntFrame);
    
    //update the object verifier
    bool updateVerifier(ImageExtension &imgExt, const Rect<float32_t> &currBB, const std::vector<DetectionScaleResult> &negBB);

private: 

    uint32_t learnedFrames; 

    Size2D<uint32_t> scanWinSize;
    ///////////////////////////////////////////////////////////////
    //variance classifier (initialization only for initialization) 
    /////////////////////////////////////////////////////////////
    float32_t imVarThreshold;

    ///////////////////////////////////////////////////////////////////////
    // sliding window object detector is the same as the qot object detector
    /////////////////////////////////////////////////////////////////////
    SlidingWindowDetector   *sldWinDet;             //the sliding window detector 
    ImagePatchClassifierCascade<uint32_t> patchClsfCascade;  //image patch classifier (variance + ensemble classifier)

    //specific features and classifiers
#ifdef USE_VARIANCE_CHECK
    //1. image variance classifier
    ImageVarianceFeature    *varFeat;
    DecisionStumpBinaryClassifier1D<float32_t> *varClassifier;
    ImagePatchClassifierSimple<float32_t, DecisionStumpBinaryClassifier1DModel<float32_t>, uint32_t> *varPatchClassifier;
#endif 

#ifndef USE_SVM_CLASSIFIER
    //2. brief feature based deciison forest classifier learning (P-N learning)
    ImagePatchClassifierSimplePNTraining<uint32_t, HistogramClassifier1DEnsembleModel > *objDetPNLearning;
    //including:
    //image patch classifier: feature extractor and classifier 
    BriefFeature    *briefFeat;
    HistogramClassifier1DEnsemble *histClassifier;
    float32_t histClassifierBias;
    ImagePatchClassifierSimple<uint32_t, HistogramClassifier1DEnsembleModel>    *briefPatchClassifier;    
    //classifier learner
    HistogramClassifier1DEnsembleTraining *histClassifierLearning;
#else 
    //2. brief binary feature based SVM classifier learning (P-N learning)
    ImagePatchClassifierSimplePNTraining<float32_t, SVMClassifierModelLinear<float32_t>, uint32_t> *objDetPNLearning;
    //including:
    //image patch classifier: feature extractor and classifier 
    //BriefBinaryFeature<float32_t>    *briefFeat;
    BriefBinaryMultiChanFeature<float32_t> *briefFeat; //for multiple channel feature, also works for Y only    
    SvmClassifier<float32_t>        *detClassifier;  
    ImagePatchClassifierSimple<float32_t, SVMClassifierModelLinear<float32_t>, uint32_t>    *briefPatchClassifier;    
    //classifier learner
    SvmClassifierTraining<float32_t> *detClassifierLearning;
#endif
    //for adjusting classification threshold 
    int32_t         too_few_detection_cnt;      //num of continuous frames that have too few detections
    int32_t         too_many_detection_cnt;     //num of continuous frames that have too many detections

    ////////////////////////////////////////////////////////////////////////
    ////  Object verification 
    //////////////////////////////////////////////////////////////////////////
    //learning for NCC patch classification, it includes both patch classifier and learner
    NCCImagePatchClassifierPNTraining<NCC_PATCH_SIZE, NCC_PATCH_SIZE, NCC_MAX_POSITIVE_PATCHES, NCC_MAX_NEGATIVE_PATCHES> *objVerPNLearning;
    //parameters
    float32_t   thresholdValid;      //Threshold of verify BB
    
    ///////////////////////////////////////////////////////////
    //other state variables
    ///////////////////////////////////////////////////////////
    uint32_t nFrameLearned;
    qcvMemory   sBuffer;
    std::vector<DetectionScaleResult> negSamplesForVerifier;


};


}   //namespace qot

}   // namespace qcv

#endif // qcv_qot_object_learning_h_
//


