/**=============================================================================

@file
ClassifierTraining.h

@brief
base class for classifier training 

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

#ifndef qcv_classifier_training_h_
#define qcv_classifier_training_h_


#include <cstddef>
#include <vector>

#include "Feature.h"

namespace qcv
{

template<class _DataType>
struct TrainingSample
{
    uint32_t classIndex;
    FeatureVectorView<_DataType>  featureView;
};



//Base class for an classifier online learner algorithm which has the ability 
//to update the classifier with new feature samples
//the class will maintain its own classifier model data (i.e. local copy)
template< class _DataType, class _ClassifierModel> 
class ClassifierTraining
{    
public :
    virtual ~ClassifierTraining() { }
        
    //public member functions
    virtual uint32_t getNumClass() const =0; 
    virtual uint32_t getFeatureLength() const =0; 
    
    //adding training sampels only
    //each implmented class will decide how to accumulate the samples, but should not update the model until the update() function is called    
    //return false if the sample is not added
    virtual bool addSample(TrainingSample<_DataType> &sample) = 0;
    
    //update the classifier model using the added samples, returned the number of samples learned
    virtual uint32_t update()=0;
    
    /*
    //batch training of a classifier
    //return true if the classifier model is learned sucessfully
    //return false if the classifier model is learned unsuccessfully    
    virtual uint32_t trainBatch(std::vector< TrainingSample<_DataType>> &samples )
    {         
        for (uint32_t i=0; i<samples.size();i++)        
            addSample(sample);
        return update();       
    }

    //update the classifier learner with a sample
    //return true if the classifier is updated with the given sample
    //return false if the classifier is not updated             
    virtual bool trainIncremental(TrainingSample<_DataType> &sample)
    { 
        addSample(sample);        
        return (update()==1);
    }
    */

    //get the classifier model (the user of classifier model cannot change the model)
    virtual _ClassifierModel& getClassifierModel(void) =0; 
    
    //set ClassifierModel
    //will maintain local copy 
    virtual void setClassifierModel(_ClassifierModel &model) =0; 

    //reset the classifier model 
    virtual void resetClassifierModel(void) = 0;

};


}   // namespace qcv

#endif // qcv_classifier_training_h_
//
