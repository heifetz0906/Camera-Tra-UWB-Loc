/**=============================================================================

@file
DecisionStumpBinaryClassifier1D.h

@brief
Definition of a decision stump, which is a binary classifier with 1D feature input using one simple threshold 

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

#ifndef qcv_decision_stump_binary_classifier_1d_h_
#define qcv_decision_stump_binary_classifier_1d_h_


#include <cstddef>
#include <cassert>
#include <vector>

#include "fastcv.h"

#include "Classifier.h"

namespace qcv
{

template<class _DataType> 
class DecisionStumpBinaryClassifier1DModel
{
public: 
    DecisionStumpBinaryClassifier1DModel()
    { 
        setDefault();
    
    }
    DecisionStumpBinaryClassifier1DModel(_DataType threshold, uint32_t featureSign): th(threshold), sign(featureSign)    { }

    void setDefault()
    {
        th=0;
        sign=1;
    }

    DecisionStumpBinaryClassifier1DModel<_DataType> *clone(void) const 
    {        
        return new DecisionStumpBinaryClassifier1DModel<_DataType> (*this);
    }

    //~DecisionStumpBinaryClassifier1DModel() { }
public:
    _DataType th;
    uint32_t sign; //-1 or 1. 1 means the postive class is greater than threahold, -1 otherwise 
};

//a decision stump, which is a binary classifier with 1D feature input using one simple threshold 
//return 1 for feature value greater than the threshold 
//return 0 for less than the threshold 
template< class _DataType> 
class DecisionStumpBinaryClassifier1D : public Classifier<_DataType, DecisionStumpBinaryClassifier1DModel<_DataType> >
{
    
    typedef FeatureVectorView<_DataType> FeatureVectorViewType;

public :
    DecisionStumpBinaryClassifier1D(DecisionStumpBinaryClassifier1DModel<_DataType> &model, bool copyModel)  
        : Classifier<_DataType, DecisionStumpBinaryClassifier1DModel<_DataType> >(copyModel)
    { 
        if (this->hasModelCopy())
        {
            dsModel = model.clone();
        }
        else
            dsModel = &model; 
    }

    ~DecisionStumpBinaryClassifier1D() 
    {
        if (this->hasModelCopy() && dsModel)
        {
            delete dsModel;
            dsModel = NULL;
        }            
    }
        
    //binary class
    uint32_t getNumClass( ) const {return 2;}
    //single feature input
    uint32_t getFeatureLength( ) const {return 1;}
   
    //public member functions 
        
    //descrete classification decision
    //return either a class index from 0 to numClass-1, 
    //      or -1 for no decision 
    //return 1 for feature value greater than the threshold 
    //return 0 for less than the threshold 
    classifyDiscreteResult classifyDiscrete(const FeatureVectorViewType &fv) const     
    {         
        assert(fv.getLength()==1);

        classifyDiscreteResult rslt;
        rslt.confidence = (float32_t) (fv[0] - dsModel->th) * dsModel->sign;
        if (rslt.confidence  > 0)
            rslt.decision = 1;
        else if (rslt.confidence  < 0)
            rslt.decision = 0;
        else 
            rslt.decision = -1;    
        return rslt;
    }

    //descrete classification decision for a given class index 
    //return either 0 for negative and 1 for positive decision
    //      or -1 for no decision   
    classifyDiscreteResult classifyDiscrete(const FeatureVectorViewType &fv, const uint32_t classIndex) const 
    { 
        assert(classIndex<2 && fv.getLength()==1);
        classifyDiscreteResult rslt = classifyDiscrete(fv);
        if (rslt.decision!=-1)
            rslt.decision = (int32_t) (rslt.decision == classIndex);
        return rslt;
    }

    //continuous classification decision 
    //will return a real number (e.g. posterior) for each class     
    void classifyContinuous(const FeatureVectorViewType &fv, std::vector<float32_t> & output) const 
    {
        assert(fv.getLength()==1);
        output.resize(2);
        float32_t conf = (float32_t) (fv[0] - dsModel->th) * dsModel->sign;
        output.push_back(conf);
        output.push_back(conf);
    }

    //continuous classification decision 
    //will return a real number (e.g. posterior) for the given class index
    float32_t classifyContinuous(const FeatureVectorViewType &fv, const uint32_t classIndex) const 
    {
        assert(fv.getLength()==1);
        assert(classIndex<2);
        float32_t conf = (float32_t) (fv[0] - dsModel->th) * dsModel->sign; 
        /*if (classIndex == 0)
            return -conf;
        else
            return conf;
        */
        return conf;
    }

    //set classifier model
    bool setClassifierModel(DecisionStumpBinaryClassifier1DModel<_DataType> &model) 
    {
        if (this->hasModelCopy())            
            *dsModel = model;
        else
            dsModel = &model;
        return this->hasModelCopy();
    }

    DecisionStumpBinaryClassifier1DModel<_DataType> & getClassifierModel(void) 
    {
        return *dsModel;
    }

private:    
    DecisionStumpBinaryClassifier1DModel<_DataType> *dsModel;

};




}   // namespace qcv

#endif // qcv_decision_stump_binary_classifier_1d_h_
//
