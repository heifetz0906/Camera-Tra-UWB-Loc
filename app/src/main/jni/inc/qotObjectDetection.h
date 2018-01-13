//
/**=============================================================================

@file
qotObjectDetection.h

@brief
includes an object detetector using sliding window, and detection logics

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

#ifndef qcv_dfobjectdetection_h_
#define qcv_dfobjectdetection_h_


#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cstring>

#include "qcvTypes.h"
#include "qcvImageExtension.h"

#include "SlidingWindowDetector.h"
#include "ImagePatchClassifierCascade.h"
#include "ImagePatchClassifierSimple.h"
#include "BriefFeature.h"
//#include "Feature/BriefBinaryFeature.h"
#include "BriefBinaryMultiChanFeature.h"
#include "ImageVariance.h"
#include "HistogramClassifierEnsemble.h"
#include "DecisionStumpBinaryClassifier1D.h"
#include "SvmClassifier.h"

#include "qcvMemory.h"

#include "qotTypes.h"
#include "qotDefs.h"

namespace qcv
{

namespace qot
{

/////////////////////////////////////////////////////////////////
// detector related Definitions
///////////////////////////////////////////////////////////////


//object detection model 
struct qotObjectDetectionModel
{

#ifdef USE_VARIANCE_CHECK    
    float32_t patchVarThreshold; //patch variance threshold
#endif

#ifndef USE_SVM_CLASSIFIER
    //Brief ensemble classifier 
    EnsembleClassifierSimpleModel<HistogramClassifier1DModel> briefModel;        
    //classifier bias 
    float32_t briefClassifierBias;
#else
    //svm classifier model
    SVMClassifierModelLinear<float32_t> *linearModel;
    
#endif
};



class qotObjectDetector 
{
public: 
    qotObjectDetector(ImageExtension &img, 
                      const Size2D<uint32_t> &winSize, 
                      qotObjectDetectionModel &objDetModel);

    ~qotObjectDetector();
    
    //detection object from an image return the detection results. 
    //return false if the detection was not run 
    bool detect(ImageExtension &ImgSrc, const QOTObjectState &objState, std::vector<DetectionScaleResult> &result);

    void setObjDetModel(const qotObjectDetectionModel &model);

private:
    //initialization of the detectors 
    bool initialize( ImageExtension &img, 
                    const Size2D<uint32_t> &winSize,
                    qotObjectDetectionModel &objDetModel);

    void uninitialize();
    
    //feature selection 
    bool featureSelection(SVMClassifierModelLinear<float32_t> *svmModel, uint32_t numFeatureRetain, bool *removedFeatureList);


public: 
    bool            initialized;                         //Initialization flag 
    
    ImageExtension  *currFrame;

    //Sliding window Detector 
    SlidingWindowDetector   *sldWinDet;             //the sliding window detector 
    ImagePatchClassifierCascade<uint32_t> patchClsfCascade;  //image patch classifier (variance + ensemble classifier)

    //specific features and classifiers
#ifdef USE_VARIANCE_CHECK
    //1. image variance classifier
    ImageVarianceFeature    *varFeat;
    DecisionStumpBinaryClassifier1D<float32_t> *varClassifier;
    ImagePatchClassifierSimple<float32_t, DecisionStumpBinaryClassifier1DModel<float32_t>, uint32_t> *varPatchClassifier;
#endif

    //2. brief feature based classifier
#ifndef USE_SVM_CLASSIFIER
    BriefFeature    *briefFeat;
    HistogramClassifier1DEnsemble *histClassifier;
    ImagePatchClassifierSimple<uint32_t, EnsembleClassifierSimpleModel<HistogramClassifier1DModel>>    *briefPatchClassifier;    
#else
    //BriefBinaryFeature<float32_t> *briefFeat;
    BriefBinaryMultiChanFeature<float32_t> *briefFeat;
    SvmClassifier<float32_t> *detClassifier;
    ImagePatchClassifierSimple<float32_t, SVMClassifierModelLinear<float32_t>, uint32_t>    *briefPatchClassifier;
#endif

    //internal states and parameters            
    uint32_t            detectionStep; //the number of windows to jump for each sliding window scan
    uint32_t            detectionStartOffest; //the counter that indicate the current starting offset

    
    //scratch buffer 
    qcvMemory       scratchBuffer;
    

};


} //namespace qot

} //namespace qcv






#endif  //qcv_dfobjectdetection_h_




