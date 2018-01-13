/**=============================================================================

@file
SVMClassifier.h

@brief
Definition of a linear SVM classifier class. 

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

#ifndef qcv_svmclassifier_h_
#define qcv_svmclassifier_h_


#include <cstddef>
#include <vector>
#include <cassert>

#include "qcvTypes.h"
#include "Classifier.h"
#include "ClassifierTraining.h"


namespace qcv
{

/* ==================================================================
class SvmClassifier
Descritpion:  this is a linear SVM classfier. The model only saves the 
weighted support vectors. 
=================================================================== **/
template<class _DataType>
class SVMClassifierModelLinear
{
public:

    SVMClassifierModelLinear(uint32_t featureLength) : 
		feaLen(featureLength), 
        numClass(2), 
        maxFeatLen(featureLength)
    {  
        weights = (_DataType *)MEMALLOC(featureLength*sizeof(_DataType), 16);
		resetModel();
	}

    virtual ~SVMClassifierModelLinear() 
    {
        MEMFREE(weights);    
    }
    //public functions 
    
    //reset model values
    void resetModel(void) 
    {
		std::memset(weights, 0, maxFeatLen*sizeof(_DataType));
		bias = (_DataType)0;  
        posThreshold = 0.9f;
        negThreshold = -0.5f;
	}

    //////////////////////////////////////////////////////////////////////////
    ////get data
    ////////////////////////////////////////////////////////////
    uint32_t getNumClass(void) const {return numClass;}
    uint32_t getFeatureLength(void) const {return feaLen;}
    _DataType getBias() const    {return bias;}

	//set data
	//setWeights
    void setWeights(const _DataType* w, uint32_t length)
	{        
        if (maxFeatLen < length)
        {
            maxFeatLen = length;
            MEMFREE(weights);                        
            weights = (_DataType *)MEMALLOC(maxFeatLen*sizeof(_DataType), 16);
        }
        feaLen = length;
        std::memcpy(weights, w, length*sizeof(_DataType));
	}

	void setBias(_DataType b) 
	{
		bias = (_DataType)b;   
	}

    float32_t getPosThreshold() const {return posThreshold;}
    float32_t getNegThreshold() const {return negThreshold;}
    void setPosThreshold(float32_t th) {posThreshold =  th;}
    void setNegThreshold(float32_t th) {negThreshold =  th;}

    SVMClassifierModelLinear<_DataType> & operator= (const SVMClassifierModelLinear<_DataType> &m) 
    {
        numClass = m.numClass;
        setWeights(m.weights, m.feaLen);
        bias = m.bias;
        posThreshold = m.posThreshold;
        negThreshold = m.negThreshold;
        return *this;
    }

    SVMClassifierModelLinear<_DataType> * clone( ) const 
    {
        SVMClassifierModelLinear<_DataType> * model = new SVMClassifierModelLinear<_DataType>(maxFeatLen);
        *model = *this;     
        return model;
    }

    
public:    
    uint32_t numClass;
    uint32_t feaLen;	
    uint32_t maxFeatLen; 
	
    _DataType  *weights;    
	_DataType bias;
    
    float32_t posThreshold;
    float32_t negThreshold;
};

template<class _DataType>
class SvmClassifier : public Classifier<_DataType, SVMClassifierModelLinear<_DataType> >
{    

    typedef FeatureVectorView<_DataType> FeatureVectorViewType;

public :
    SvmClassifier(SVMClassifierModelLinear<_DataType> &model, bool copyModel):
		Classifier<_DataType, SVMClassifierModelLinear<_DataType> >(copyModel)
	{		
		if (copyModel)
        {
            svmModel = model.clone();
        }
        else
		    svmModel = &model;
	}

	// TODO(yang): if "model" already exists in the memory, and later 
	// a SvmClassifier is created from it. When you destroy the classifier,
	// "model" will also be destroyed. Is this the expected behavior?
    ~SvmClassifier()
	{
		if (this->hasModelCopy()) 
		{
			delete svmModel;
			svmModel = NULL;
		}
	}
        
    //public functions
    uint32_t getNumClass( ) const { return svmModel->getNumClass(); }
    
    //get feature length
    uint32_t getFeatureLength( ) const { return svmModel->getFeatureLength(); }

    //descrete classification decision
    //return either a class index from 0 to numClass-1, 
    //      or -1 for no decision     
    classifyDiscreteResult classifyDiscrete(const FeatureVectorViewType &fv) const
	{
		assert(fv.getLength()== getFeatureLength() );
		classifyDiscreteResult rslt;

		
        rslt.confidence = fcvDotProductf32(fv.getData(), svmModel->weights,svmModel->feaLen) + svmModel->getBias();
		
        if (rslt.confidence  > svmModel->posThreshold)
            rslt.decision = 1;
        else if (rslt.confidence  < svmModel->negThreshold)
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
		assert(classIndex<2 && (fv.getLength() == getFeatureLength()));
        classifyDiscreteResult rslt = classifyDiscrete(fv);
        if (rslt.decision!=-1)
            rslt.decision = (int32_t) (rslt.decision == classIndex);
        return rslt;
	}

    //continuous classification decision 
    //will return a real number (e.g. posterior) for each class 
    // TODO(yang): change it to:
	// float32_t classifyContinuous(const FeatureVectorViewType &fv) const
	void classifyContinuous(const FeatureVectorViewType &fv, std::vector<float32_t> & output) const
     {
        assert(fv.getLength() == getFeatureLength());
        output.clear();
        float32_t conf = fcvDotProductf32(fv.getData(), svmModel->weights,svmModel->feaLen) + svmModel->getBias();
		
        output.push_back(-conf);
        output.push_back(conf);
     }

    //continuous classification decision 
    //will return a real number (e.g. posterior) for the given class index
    float32_t classifyContinuous(const FeatureVectorViewType &fv, const uint32_t classIndex) const
    {
        assert(fv.getLength()== getFeatureLength() );
        assert(classIndex<2);
        float32_t conf = fcvDotProductf32(fv.getData(), svmModel->weights,svmModel->feaLen) + svmModel->getBias();
		
        if (classIndex == 0)
            return -conf;
        else
            return conf;        
        return conf;
    }
    //set classifier model

    bool setClassifierModel(SVMClassifierModelLinear<_DataType> &model)
	{
		if(this->hasModelCopy())
            *svmModel = model;
        else
            svmModel = &model;
        return this->hasModelCopy();
	}

    //get the classifier model (will return the constant reference)
    // TODO(yang): unsafe. using const?
	SVMClassifierModelLinear<_DataType> & getClassifierModel(void)
	{
		return *svmModel;
	}

private:
    SVMClassifierModelLinear<_DataType> *svmModel;
};


}   // namespace qcv

#endif // qcv_svmclassifier_h_
//
