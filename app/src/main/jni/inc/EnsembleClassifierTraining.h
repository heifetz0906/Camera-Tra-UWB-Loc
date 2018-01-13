/**=============================================================================

@file
EnsembleClassifierSimpleTraining.h

@brief
class for ensemble classifier training 

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

#ifndef qcv_ensemble_classifier_training_h_
#define qcv_ensemble_classifier_training_h_


#include <cstddef>
#include <vector>

#include "qcvTypes.h"

#include "ClassifierTraining.h"
#include "EnsembleClassifier.h"

namespace qcv
{

//class for ensemble classifier training 
template< class _DataType, class _WeakClassifierModel> 
class EnsembleClassifierSimpleTraining : public ClassifierTraining<_DataType, EnsembleClassifierSimpleModel<_WeakClassifierModel> > 
{     
public :
    typedef EnsembleClassifierSimpleModel<_WeakClassifierModel> EnsembleWeakModel;

    EnsembleClassifierSimpleTraining( );

    //~EnsembleClassifierSimpleTraining();
        
    //public member functions
    uint32_t getNumClass() const
    {
        if (learners.empty())
            return 0;
        else
            return learners[0]->getNumClass();
    }

    uint32_t getFeatureLength( ) const { return featureLength;}

    //adding training sampels only
    //each implmented class will decide how to accumulate the samples, but should not update the model until the update() function is called    
    //return false if the sample is not added
    bool addSample(TrainingSample<_DataType> &sample);
    
    //update the classifier model using the added samples, returned the number of samples learned
    uint32_t update();

    //get the classifier model (the user of classifier model cannot change the model)
    EnsembleWeakModel & getClassifierModel(void); 
    
    //set ClassifierModel for the learner only
    //will maintain local copy since the model will be updated
    void setClassifierModel(EnsembleWeakModel &model); 
    
    //reset the current learning model 
    void resetClassifierModel(void);
    
    ///////////////////////////////////////////////////////////////////////////
    void addTrainer(ClassifierTraining<_DataType, _WeakClassifierModel> & learner);
    void setTrainers(const std::vector< ClassifierTraining<_DataType, _WeakClassifierModel> *> &learnerVector);

private: 
    std::vector< ClassifierTraining<_DataType, _WeakClassifierModel> *> learners;    
    EnsembleWeakModel learnerModels;
    uint32_t featureLength; //total length of the feautre vector, i.e. sum of all feature length from all weak classifiers

};


///implementation of EnsembleClassifierSimpleTraining
template< class _DataType, class _WeakClassifierModel> 
EnsembleClassifierSimpleTraining<_DataType, _WeakClassifierModel>::EnsembleClassifierSimpleTraining() 
{ 
    featureLength = 0;
    learners.clear();
    learnerModels.clear();
}

template< class _DataType, class _WeakClassifierModel> 
void EnsembleClassifierSimpleTraining<_DataType, _WeakClassifierModel>::addTrainer( ClassifierTraining<_DataType, _WeakClassifierModel> & learner )
{
    if (!learners.empty())
        assert(learners[0]->getNumClass()== learner.getNumClass() );
    learners.push_back(&learner);
    learnerModels.addModel(learner.getClassifierModel());
    featureLength += learner.getFeatureLength();  
}


template< class _DataType, class _WeakClassifierModel> 
void EnsembleClassifierSimpleTraining<_DataType, _WeakClassifierModel>::setTrainers(const std::vector< ClassifierTraining<_DataType, _WeakClassifierModel> *> &learnerVector)
{
    featureLength = 0;
    learners = learnerVector;
    this->trainerModels.clear();
    for (uint32_t i=0;i<learners.size();i++)
    {
        assert(learners[0].getNumClass() == learners[i]->getNumClass());
        learnerModels.addModel(learners[i]->getClassifierModel());
        featureLength += learners[i]->getFeatureLength();        
    }
}



//get the classifier model
template< class _DataType, class _WeakClassifierModel> 
EnsembleClassifierSimpleModel<_WeakClassifierModel> & EnsembleClassifierSimpleTraining<_DataType, _WeakClassifierModel>::getClassifierModel(void) 
{
    return learnerModels;
}


//set ClassifierModel for the learner only
//will maintain local copy since the model will be updated
template< class _DataType, class _WeakClassifierModel> 
void EnsembleClassifierSimpleTraining<_DataType, _WeakClassifierModel>::setClassifierModel(EnsembleWeakModel &model)
{
    assert(learners.size()==model.size());    
    learnerModels.clear();
    for (uint32_t i=0; i< learners.size(); i++)
    {
        learners[i]->setClassifierModel( *(model.getModel(i)) );
        learnerModels.addModel(learners[i]->getClassifierModel());
    }
}


//reset the current learning model 
template< class _DataType, class _WeakClassifierModel> 
void EnsembleClassifierSimpleTraining<_DataType, _WeakClassifierModel>::resetClassifierModel(void)
{    
    for (uint32_t i=0; i< learners.size(); i++)
    {
        learners[i]->resetClassifierModel();        
    }
}


//adding training sampels only
//each implmented class will decide how to accumulate the samples, but should not update the model until the update() function is called    
//return false if the sample is not added
template< class _DataType, class _WeakClassifierModel> 
bool EnsembleClassifierSimpleTraining<_DataType, _WeakClassifierModel>::addSample(TrainingSample<_DataType> &sample)
{
    _DataType *fvPtr = sample.featureView.getData();
    TrainingSample<_DataType> subSample;
    subSample.classIndex = sample.classIndex;
    for (uint32_t i=0;i<learners.size();i++)
    {
        subSample.featureView.setData(fvPtr, learners[i]->getFeatureLength());
        learners[i]->addSample(subSample);
        fvPtr += learners[i]->getFeatureLength();        
    }
    return true;

}
    
//update the classifier model using the added samples, returned the number of samples learned
template< class _DataType, class _WeakClassifierModel> 
uint32_t  EnsembleClassifierSimpleTraining<_DataType, _WeakClassifierModel>::update()
{
    uint32_t numSamples;
    for (uint32_t i=0;i<learners.size();i++)
        numSamples = learners[i]->update();
    return numSamples;

}


}   // namespace qcv

#endif // qcv_ensemble_classifier_training_h_
//
