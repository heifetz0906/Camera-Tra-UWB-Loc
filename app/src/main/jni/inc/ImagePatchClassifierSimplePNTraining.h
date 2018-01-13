/**=============================================================================

@file
ImagePatchClassifierSimplePNTraining.h

@brief
Learning of a simple image patch classifier. It contains a simple image patch classifier and a learner

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

#ifndef qcv_image_patch_classifier_simple_pntraining_h_
#define qcv_image_patch_classifier_simple_pntraining_h_


#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cstring>

#include "qcvTypes.h"
#include "qcvImage.h"
#include "Feature.h"
#include "ClassifierTraining.h"
#include "qcvImageExtension.h"

#include "ImagePatchClassifierSimple.h"


namespace qcv 
{

//PN learning of a simple image patch classifier, which learns only the false classification 
template< class _DataType, class _ClassifierModel, class _LocType> 
class ImagePatchClassifierSimplePNTraining : public ImagePatchClassifier<_LocType>
{
    
public :
    ImagePatchClassifierSimplePNTraining( ImagePatchClassifierSimple<_DataType, _ClassifierModel, _LocType> &classifier,
                    ClassifierTraining<_DataType, _ClassifierModel> &trainer) : 
    patchClassifier(classifier),
    learner(trainer), 
    forceToLearn(false), 
    incrementalLearning(true),
    threshMinConfLearnPos(-10000), // the threshold for minimum confidence to learn the false negative samples (for positive set)
    threshMaxConfLearnNeg(10000) // the threshold for maximum confidence to learn the false positive samples (for negative set)
    {    }
       
    //public member functions 

    //reloaded functions for setting patch width and height
    void setPatchSize(const Size2D<_LocType> &size) 
    {
        patchClassifier.setPatchSize(size);
    }

    Size2D<_LocType> getPatchSize(void) const {return patchClassifier.getPatchSize();}
    
    void setFlagForceToLearn(bool flag) {forceToLearn = flag;}

    void setFlagIncrementalLearning(bool flag) {incrementalLearning = flag;}

    void setThreshConfLearn(float32_t thPos, float32_t thNeg) 
    {   threshMinConfLearnPos = thPos; threshMaxConfLearnNeg = thNeg; }
    

    //this function enables that learning to be called by the Sliding window for learning 
    //assuming that the location is pointing to a negative patch 
    //imgExt contains pointer to the beginning of the image 
    //locXY gives the upper-left conrer of the patch         
    classifyDiscreteResult classifyPatch(ImageExtension &imgExt, const Point2D<_LocType> &locXY)
    {        
        trainImagePatch(imgExt, locXY, 0);            
        return result;
    }

    //Given an image patch, extract an image feature, and learns the patch if it is a false classification     
    //imgExt contains pointer to the beginning of the image, patchOffset indicate the starting position of the patch    
    //if forceToLearn is true, then always learn 
    //return true if the sample is learned, false otherwise.
    bool trainImagePatch(ImageExtension &imgExt, const Point2D<_LocType> &locXY, uint32_t trueClassIndex)
    {
        uint32_t posClassIdx = patchClassifier.getClassifierIndex();
        bool posLabel = (posClassIdx==trueClassIndex);        
        bool posDecision = false;
        
        TrainingSample<_DataType> sample;        
        if (forceToLearn)
        {            
            sample.classIndex = trueClassIndex;
            sample.featureView = patchClassifier.extractFeature(imgExt, locXY);
            result.decision = -1;
            bool sampleAdd = learner.addSample(sample);
            if (incrementalLearning)
                updateLearner();
            return sampleAdd;
        }
        else 
        {
            //Output: decision: 0 for negative response, 1 for positive response, -1 for no decision
            result = patchClassifier.classifyPatch(imgExt, locXY);             
            posDecision = (result.decision==1);
            if (result.decision==-1 || posDecision != posLabel) 
            {                
                //don't learn false positive or false negative samples if their confidence values are too exetrem 
                //      so that we avoid some false detections that distroy our templates                 
                if ( (result.decision==-1) || (posLabel && result.confidence > threshMinConfLearnPos) || (!posLabel && result.confidence < threshMaxConfLearnNeg) )                
                {    
                    //false classification, need to learn                
                    sample.classIndex = trueClassIndex;
                    sample.featureView = patchClassifier.getFeatureView( );   
                    if(posLabel)
                        result.decision = 0; //return false detections for patch classifier 
                    else
                        result.decision = 1; //resturn true for negative sample
                    bool sampleAdd = learner.addSample(sample);
                    if (incrementalLearning)
                        updateLearner();
                    return sampleAdd;
                }
            }
        }
        return false;
    }

    uint32_t updateLearner()
    {
        return learner.update();
    }


    //synchronize the classifier model of the classifier to that of the learner_
    void syncClassifierModel(void)
    {
        patchClassifier.setClassifierModel(learner.getClassifierModel());
    }
    
protected:
    ImagePatchClassifierSimple<_DataType, _ClassifierModel, _LocType> &patchClassifier;
    ClassifierTraining<_DataType, _ClassifierModel> &learner;    

    bool forceToLearn;   //whether force the classifier to learn
    bool incrementalLearning; //whether updating the learning after each trained patch (default is true)
    classifyDiscreteResult result;

    float32_t threshMinConfLearnPos; // the threshold for minimum confidence to learn the false negative samples (for positive set)
    float32_t threshMaxConfLearnNeg; // the threshold for maximum confidence to learn the false positive samples (for negative set)
};



}   // namespace qcv

#endif // qcv_image_patch_classifier_simple_pntraining_h_
//

