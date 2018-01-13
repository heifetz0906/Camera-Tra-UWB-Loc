/**=============================================================================

@file
EnsembleClassifier.h

@brief
Definition of a simple ensemble classifier, which fuse the decision of multiple classifiers. 
/class for an ensemble classifier, which combines the output of multiple weak classifiers for a decision
//the feature length of the ensembled classifier is the sum of those of the weak classifiers
//all the weak classifiers should have the same type classifier model

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

#ifndef qcv_ensembleclassifier_simple_h_
#define qcv_ensembleclassifier_simple_h_


#include <cstddef>
#include <vector>
#include <cassert>

#include "Classifier.h"

namespace qcv
{

template< class _WeakClassifierModel > 
class EnsembleClassifierSimpleModel
{
public:

    EnsembleClassifierSimpleModel() 
    {
        weakModels.clear();
    }

    ~EnsembleClassifierSimpleModel() { }

    uint32_t size() const {return weakModels.size();}
    
    void addModel(_WeakClassifierModel &model)
    {
        weakModels.push_back(&model);
    }
    
    _WeakClassifierModel *getModel(uint32_t index)  
    {
        assert(index<weakModels.size());
        return weakModels[index];
    }
        
    void clear(void) {weakModels.clear();}
    
private:
    std::vector<_WeakClassifierModel *> weakModels;
};

//class for an ensemble classifier, which combines the output of multiple weak classifiers for a decision
//the feature length of the ensembled classifier is the sum of those of the weak classifiers
//all the weak classifiers should have the same type classifier model
template< class _DataType, class _WeakClassifierModel > 
class EnsembleClassifierSimple : public Classifier<_DataType, EnsembleClassifierSimpleModel<_WeakClassifierModel> >
{
    typedef FeatureVectorView<_DataType> FeatureVectorViewType;
    typedef EnsembleClassifierSimpleModel<_WeakClassifierModel> EnsembleWeakModel;

public :
    //nClass: number of classes
    EnsembleClassifierSimple();

    virtual ~EnsembleClassifierSimple(){ };
        
    /////////////////////////////////////////////////////////////////////
    //member functions required by the base class
    /////////////////////////////////////////////////////////////////////

    //public member functions
    uint32_t getNumClass( ) const
    {
        if (classifiers.empty())
            return 0;
        else
            return classifiers[0]->getNumClass();
    }

    uint32_t getFeatureLength( ) const { return featureLength;}

    //descrete classification decision
    //return either a class index from 0 to numClass-1, 
    //      or -1 for no decision    
    //fv: feature vector that concatenates the feature vectors of all classifiers 
    classifyDiscreteResult classifyDiscrete(const FeatureVectorViewType &fv) const;

    //descrete classification decision for a given class index 
    //return either 0 for negative and 1 for positive decision
    //      or -1 for no decision 
    //fv: feature vector that concatenates the feature vectors of all classifiers         
    classifyDiscreteResult classifyDiscrete(const FeatureVectorViewType &fv, const uint32_t classindex) const;

    //continuous classification decision 
    //will return a real number (e.g. posterior) for each class 
    //fv: feature vector that concatenates the feature vectors of all classifiers 
    void classifyContinuous(const FeatureVectorViewType &fv, std::vector<float32_t> & output) const;

    //continuous classification decision 
    //will return a real number (e.g. posterior) for the given class index
    //fv: feature vector that concatenates the feature vectors of all classifiers 
    //fLen: total length of the feautre vector, i.e. sum of all feature length from all weak classifiers
    float32_t classifyContinuous(const FeatureVectorViewType &fv, const uint32_t classindex) const;
    
    bool setClassifierModel(EnsembleWeakModel &model);   
    EnsembleWeakModel & getClassifierModel(void);


    ////////////////////////////////////////////////////////////////////////////////////
    //own public functions
    ///////////////////////////////////////////////////////////////////////////////////
    //set class variables 
    void setClassBias(const std::vector<float32_t> &bias); 
    void setClassBias(const uint32_t classindex, const float32_t bias); //set by class index
    

    void addClassifier(Classifier<_DataType, _WeakClassifierModel> &clf);   
    void setClassifiers(const std::vector< Classifier<_DataType, _WeakClassifierModel> *> &clfs);

    
private:     
    //fusion the output of each classifier using the equal weight (i.e. average)
    void NaiveBayesFusion(const FeatureVectorViewType &fv, std::vector<float32_t> & output) const; //for all classes
    float32_t NaiveBayesFusion(const FeatureVectorViewType &fv,  const uint32_t classindex) const; //for a given class

    //majority vote for descrete decision
//    classifyDiscreteResult MajorityVote(const FeatureVectorViewType &fv) const;
//    classifyDiscreteResult MajorityVote(const FeatureVectorViewType &fv, const uint32_t classindex) const;

private:
    uint32_t featureLength; //total length of the feautre vector, i.e. sum of all feature length from all weak classifiers
    std::vector< Classifier<_DataType, _WeakClassifierModel> * > classifiers;    
    EnsembleWeakModel classifierModels;
    std::vector<float32_t> classBias; //the bias (threshold) for each class to make discrete decision 

};



//Implementation of EnsembleClassifierSimple
template< class _DataType, class _WeakClassifierModel > 
EnsembleClassifierSimple<_DataType, _WeakClassifierModel >::EnsembleClassifierSimple( ) : 
    Classifier<_DataType, EnsembleClassifierSimpleModel<_WeakClassifierModel> > (true)
{ 
    featureLength = 0;
    classBias.clear();    
    classifiers.clear();
    classifierModels.clear();
}


template< class _DataType, class _WeakClassifierModel > 
void EnsembleClassifierSimple<_DataType, _WeakClassifierModel > ::setClassBias(const std::vector<float32_t> &bias)
{
    assert(classBias.size()==bias.size());
    classBias = bias;    
}

//set by class index
template< class _DataType, class _WeakClassifierModel > 
void EnsembleClassifierSimple<_DataType, _WeakClassifierModel > ::setClassBias(const uint32_t classindex, const float32_t bias)
{   
    assert(classindex < classBias.size());
    classBias[classindex] = bias;
}

template< class _DataType, class _WeakClassifierModel > 
void EnsembleClassifierSimple<_DataType, _WeakClassifierModel > ::addClassifier(Classifier<_DataType, _WeakClassifierModel> &classifier)
{
    if (classifiers.empty())
        classBias.resize(classifier.getNumClass(),0);
    else
        assert(classifiers[0]->getNumClass()== classifier.getNumClass());

    classifiers.push_back(&(classifier));
    classifierModels.addModel(classifier.getClassifierModel());
    featureLength += classifier.getFeatureLength();
}

template< class _DataType, class _WeakClassifierModel > 
void EnsembleClassifierSimple<_DataType, _WeakClassifierModel > ::setClassifiers(const std::vector< Classifier<_DataType, _WeakClassifierModel> *> &classifierVector)
{   
    classifiers = classifierVector;

    classifierModels.clear();    
    uint32_t numClass = classifiers[0]->getNumClass();    
    featureLength = 0;
    for (uint32_t i=0;i<classifierVector.size();i++)
    {
        assert(numClass == classifiers[i]->getNumClass());
        featureLength += classifiers[i]->getFeatureLength();
        classifierModels.addModel(classifiers[i]->getClassifierModel());
    }    
}

///////////////////////////////////////////////////////////////////////////////
template< class _DataType, class _WeakClassifierModel > 
bool EnsembleClassifierSimple<_DataType, _WeakClassifierModel >::
    setClassifierModel(EnsembleWeakModel &model)
{
    assert(classifiers.size()== model.size());    
    classifierModels.clear();
    for (uint32_t i=0;i<classifiers.size();i++)
    {
        classifiers[i]->setClassifierModel(*model.getModel(i));
        classifierModels.addModel(classifiers[i]->getClassifierModel());
    }
    return this->hasModelCopy(); 
}

template< class _DataType, class _WeakClassifierModel > 
EnsembleClassifierSimpleModel<_WeakClassifierModel> & EnsembleClassifierSimple<_DataType, _WeakClassifierModel >::getClassifierModel(void)
{
    return classifierModels;
}

//descrete classification decision
//return either a class index from 0 to numClass-1, 
//      or -1 for no decision 
//fv: feature vector that concatenates the feature vectors of all classifiers 
template< class _DataType, class _WeakClassifierModel > 
classifyDiscreteResult  EnsembleClassifierSimple<_DataType, _WeakClassifierModel >::classifyDiscrete(const FeatureVectorViewType &fv) const
{    
    classifyDiscreteResult output;
    std::vector<float32_t> confidents;
    NaiveBayesFusion(fv, confidents);
    if (confidents.empty())
    {
        output.confidence = 0;
        output.decision = -1;
    }
    else
    {        
        float32_t maxC =confidents[0];
        int32_t classIndex = 0;  
        for (unsigned i=1; i<confidents.size(); i++)
        {            
            if (confidents[i]> maxC)
            {            
                maxC = confidents[i];
                classIndex = i;
            }
            else if (confidents[i] == maxC)
                classIndex = -1;
        }

        output.decision = classIndex;
        if (classIndex!=-1)
            output.confidence = maxC;
        else 
            output.confidence = 0;
    }
    return output;    
}


//descrete classification decision for a given class index 
//return either 0 for negative and 1 for positive decision
//      or -1 for no decision 
template< class _DataType, class _WeakClassifierModel > 
classifyDiscreteResult EnsembleClassifierSimple<_DataType, _WeakClassifierModel > ::classifyDiscrete(const FeatureVectorViewType &fv, const uint32_t classindex) const 
{
    classifyDiscreteResult output;  
    if (classifiers.empty())
    {
        output.confidence = 0;
        output.decision = -1;
    }
    else
    {
        output.confidence = NaiveBayesFusion(fv, classindex) - classBias[classindex];
        if (output.confidence > 0 )
            output.decision = 1;
        else if (output.confidence==0)
            output.decision = -1;
        else 
            output.decision = 0;

        output.confidence +=  classBias[classindex];
    }
    return output;    
}

//continuous classification decision 
//will return a real number (e.g. posterior) for each class 
template< class _DataType, class _WeakClassifierModel > 
void EnsembleClassifierSimple<_DataType, _WeakClassifierModel >::classifyContinuous(const FeatureVectorViewType &fv, std::vector<float32_t> & output) const 
{
    //use the naive bayesian fusion method
    NaiveBayesFusion(fv, output);
}


//continuous classification decision 
//will return a real number (e.g. posterior) for the given class index
template< class _DataType, class _WeakClassifierModel > 
float32_t EnsembleClassifierSimple<_DataType, _WeakClassifierModel >::classifyContinuous(const FeatureVectorViewType &fv, const uint32_t classindex) const 
{
    //use the naive bayesian fusion method
    return NaiveBayesFusion(fv, classindex);

}

//fusion the output of each classifier using the equal weight (i.e. average)
//for all classes
template< class _DataType, class _WeakClassifierModel > 
void EnsembleClassifierSimple<_DataType, _WeakClassifierModel >::NaiveBayesFusion(const FeatureVectorViewType &fv, std::vector<float32_t> & output) const
{
    uint32_t numClass;
    if (classifiers.empty())
    {
        output.clear();
        return;
    }
    else
    {
        numClass = getNumClass();
        output.resize(numClass, 0);        
    }

    std::vector<float32_t> clsoutput(numClass,0);    
    _DataType *fvPtr = fv.getData();    
    for(uint32_t c = 0; c < classifiers.size(); c++)
    {                
        FeatureVectorViewType subFV(fvPtr, classifiers[c]->getFeatureLength());
        classifiers[c]->classifyContinuous(subFV, clsoutput);
        for (uint32_t i = 0; i < numClass; i++)
            output[i] += clsoutput[i];
        fvPtr += classifiers[c]->getFeatureLength();
    }
    for (uint32_t i = 0; i < numClass; i++)
        output[i] = output[i]/(float32_t)numClass; // + classBias[i];    
}


//for a given class
template< class _DataType, class _WeakClassifierModel > 
float32_t EnsembleClassifierSimple<_DataType, _WeakClassifierModel >::
    NaiveBayesFusion(const FeatureVectorViewType &fv,  const uint32_t classindex) const
{
    if (classifiers.empty())
        return 0;
  
    float32_t output = 0;    
    _DataType *fvPtr = fv.getData();    
    for(uint32_t c = 0; c < classifiers.size(); c++)
    {                        
        FeatureVectorViewType subFV(fvPtr, classifiers[c]->getFeatureLength());
        float32_t clsoutput = classifiers[c]->classifyContinuous(subFV, classindex);        
        output += clsoutput;
        fvPtr += classifiers[c]->getFeatureLength();
    }
    output = output / (float32_t) classifiers.size(); // + classBias[classindex];
    
    return output;
}
/*
//majority vote for descrete decision
//for all classes
template< class _DataType, class _WeakClassifierModel > 
classifyDiscreteResult EnsembleClassifierSimple<_DataType, _WeakClassifierModel >::MajorityVote(const FeatureVectorViewType &fv) const
{
    uint32_t numClass = getNumClass();
    std::vector<uint32_t> clsCounter(numClass, 0);
    _DataType *fvPtr = fv.getData();
    for (uint32_t c=0; c < classifiers.size(); c++)
    {
        FeatureVectorViewType subFV(fvPtr, classifiers[c]->getFeatureLength());
        classifyDiscreteResult clsrslt = classifiers[c]->classifyDiscrete(subFV);
        uint32_t clslabel = clsrslt.decision;
        if (clslabel >=0 && clslabel < (int32_t)numClass)
        {
            clsCounter[clslabel]++;
        }
        fvPtr += classifiers[c]->getFeatureLength();        
    }
    //find the class with the maximum vote
    uint32_t maxCount=clsCounter[0];
    int32_t maxIdx=0;
    for (uint32_t i=1;i< numClass; i++)
    {
        if (clsCounter[i]>maxCount)
        {
            maxCount = clsCounter[i];            
            maxIdx = i;            
        }
        else if (clsCounter[i] == maxCount)
        {
            maxIdx = -1;
        }
    }        
    classifyDiscreteResult output;    
    if (maxIdx!=-1)
        output.confidence = (float32_t)maxCount / (float32_t)classifiers.size(); //clsConf[maxIdx]/maxCount;
    else
        output.confidence = 0;
    output.decision = maxIdx;
    return output;

}

//for a given class
template< class _DataType, class _WeakClassifierModel > 
classifyDiscreteResult EnsembleClassifierSimple<_DataType, _WeakClassifierModel >::MajorityVote(const FeatureVectorViewType &fv, const uint32_t classindex) const
{
    uint32_t posCount = 0;
    uint32_t negCount = 0;
    _DataType *fvPtr = fv.getData();
    for (uint32_t c=0; c < classifiers.size(); c++)
    {
        FeatureVectorViewType subFV(fvPtr, classifiers[c]->getFeatureLength());
        classifyDiscreteResult clsrslt = classifiers[c]->classifyDiscrete(subFV, classindex);
        int32_t clslabel = clsrslt.decision;
        if (clslabel ==0 )
            negCount++;
        else if (clslabel ==1)
            posCount++;
        fvPtr += classifiers[c]->getFeatureLength();
    }
    classifyDiscreteResult output;
    if (negCount > posCount)
    {
        output.confidence = (float32_t)negCount / (float32_t)classifiers.size(); 
        output.decision = 0;
    }
    else if (negCount < posCount)
    {
        output.confidence = (float32_t)posCount / (float32_t)classifiers.size();
        output.decision = 1;
    }
    else 
    {
        output.confidence = 0;
        output.decision = -1;
    }
    return output;
}
*/


}   // namespace qcv

#endif // qcv_ensembleclassifier_simple_h_
//

