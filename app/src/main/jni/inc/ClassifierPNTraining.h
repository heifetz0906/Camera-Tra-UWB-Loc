/**=============================================================================

@file
ClassifierPNTraining.h

@brief
A classifier training class that only updates the model with false classified samples


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

#ifndef qcv_classifier_pntraining_h_
#define qcv_classifier_pntraining_h_


#include <cstddef>
#include <vector>

#include "Classifier.h"
#include "ClassifierTraining.h"

namespace qcv
{

//A classifier training class that only updates the model with false classified samples
//the ClassifierPNTraining assumes that the classifier and the learner are initialize from outside 
//they may share the same model, or different, but the training doesn't assumes either. 
//only the learner's model will be updated during learning (if they are different). 
//the syncClassifierModel() will update the classifier's model
template< class _DataType, class _ClassifierModel> 
class ClassifierPNTraining : public ClassifierTraining<_DataType, _ClassifierModel>
{    
public :
    ClassifierPNTraining(ClassifierTraining<_DataType, _ClassifierModel> &learner, Classifier<_DataType, _ClassifierModel> &classifier) :
        learner_(learner), 
        classifier_(classifier), 
        forceToLearn(false)
    { 
        assert(learner.getNumClass()==classifier.getNumClass());
        //if (learner.getNumClass()==classifier.getNumClass() )
        //    throw std::runtime_error("ClassifierPNLearner::ClassifierPNLearner(): Different number of Classes");
    }

    //~ClassifierPNTraining() { }
        
    //public member functions
    
    uint32_t getNumClass() const { return learner_.getNumClass(); }
    uint32_t getFeatureLength() const { return learner_.getFeatureLength(); }
    
    //adding training sampels only
    //each implmented class will decide how to accumulate the samples, but should not update the model until the update() function is called    
    bool addSample(TrainingSample<_DataType> &sample)
    {
        if (forceToLearn)
        {
            learner_.addSample(sample);
            return true;
        }
        else
        {
            //first classify the feature, then add the template
            classifyDiscreteResult rlst = classifier_.classifyDiscrete(sample.featureView, sample.classIndex);
            if (rlst.decision != 1)
            {
                return learner_.addSample(sample);
            }
            else
                return false;
        }       
    }
    
    //update the classifier model using the added samples, returned the number of samples learned
    uint32_t update()
    {
        uint32_t numLearned = learner_.update();
        syncClassifierModel();
        return numLearned;
    }

    //get the classifier model from the learner
    _ClassifierModel& getClassifierModel(void) 
    {
        return learner_.getClassifierModel(); 
    }    

    //set ClassifierModel for learner only 
    //will maintain local copy since the model will be updated
    void setClassifierModel(_ClassifierModel &model)  
    {         
        learner_.setClassifierModel(model);        
    } 

    //clear the classifier model of the learner 
    void resetlassifierModel(void) 
    {
        learner_.resetlassifierModel();
    }

    //synchronize the classifier model of the classifier to that of the learner_
    void syncClassifierModel(void)
    {
        classifier_.setClassifierModel(learner_.getClassifierModel());
    }

    Classifier<_DataType, _ClassifierModel> & getClassifier(void) {return classifier_;}
    
    ClassifierTraining<_DataType, _ClassifierModel> & getLearner(void) {return learner_;}

    //reset the classifier model 
    void resetClassifierModel(void)
    {
        learner_.resetClassifierModel();
        syncClassifierModel();
    }


    //set for force to learn flag
    void setForceToLearn(bool flag) { forceToLearn = flag; }
    bool getForceToLearn(void) const {  return forceToLearn; }

private: 
    ClassifierTraining<_DataType, _ClassifierModel> &learner_;
    Classifier<_DataType, _ClassifierModel> &classifier_;
    bool forceToLearn;
    
};



}   // namespace qcv

#endif // qcv_classifier_pntraining_h_
//
