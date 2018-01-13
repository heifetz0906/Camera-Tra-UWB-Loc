/**=============================================================================

@file
Classifier.h

@brief
Definition of Classifier class, which will be a base class for different kind of classifiers. 
A clasisifer holds its own model, and given a feature, it'll provide either a descrete or 
a set of continuous deicsions. 

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

#ifndef qcv_classifier_h_
#define qcv_classifier_h_


#include <cstddef>

#include "Feature.h"

namespace qcv
{

//output structure for descrete classification 
struct classifyDiscreteResult
{
    int32_t decision;
    float32_t confidence;
};



//Base class for a standard classifier
//it has two classification modes: descrete and continuous
//to ensure easier support of multi-threading and qcvGraph implementation
//the classifier does not have to own its local copy of the classifier model, 
//which is indicated by a flag: localModelCopy;
//
template< class _DataType>
class ClassifierGeneral
{
public:

	//typedef typename FeatureVectorView<_DataType> FeatureVectorViewType;
	typedef FeatureVectorView<_DataType> FeatureVectorViewType;

	
	virtual ~ClassifierGeneral() { }

	virtual uint32_t getNumClass() const = 0; //{return numClass;}
	virtual uint32_t getFeatureLength() const = 0;

	//descrete classification decision
	//return either a class index from 0 to numClass-1, 
	//      or -1 for no decision 
	virtual classifyDiscreteResult classifyDiscrete(const FeatureVectorViewType &fv) const
	{
		classifyDiscreteResult output; output.confidence = 0.0f; output.decision = -1; return output;
	}

	//descrete classification decision for a given class index 
	//return either 0 for negative and 1 for positive decision
	//      or -1 for no decision 
	virtual classifyDiscreteResult classifyDiscrete(const FeatureVectorViewType &fv, const uint32_t classindex) const
	{
		classifyDiscreteResult output; output.confidence = 0.0f; output.decision = -1; return output;
	}

	//continuous classification decision 
	//will return a real number (e.g. posterior) for each class 
	virtual void classifyContinuous(const FeatureVectorViewType &fv, std::vector<float32_t> & output) const
	{
		output.clear();
	}

	//continuous classification decision 
	//will return a real number (e.g. posterior) for the given class index
	virtual float32_t classifyContinuous(const FeatureVectorViewType &fv, const uint32_t classindex) const
	{
		return 0.0f;
	}

};



//Base class for a standard classifier
//it has two classification modes: descrete and continuous
//to ensure easier support of multi-threading and qcvGraph implementation
//the classifier does not have to own its local copy of the classifier model, 
//which is indicated by a flag: localModelCopy;
//
template< class _DataType, class _ClassifierModel > 
class Classifier : public ClassifierGeneral<_DataType>
{
public :

    //typedef typename FeatureVectorView<_DataType> FeatureVectorViewType;
    //typedef FeatureVectorView<_DataType> FeatureVectorViewType;

    Classifier(bool copyModel) : modelCopy(copyModel) { }

    virtual ~Classifier() { }
        
    //public member functions
    bool hasModelCopy(void) const {return modelCopy;}
	/*
    virtual uint32_t getNumClass( ) const = 0; //{return numClass;}
    virtual uint32_t getFeatureLength( ) const =0;

    //descrete classification decision
    //return either a class index from 0 to numClass-1, 
    //      or -1 for no decision 
    virtual classifyDiscreteResult classifyDiscrete(const FeatureVectorViewType &fv) const 
    { classifyDiscreteResult output; output.confidence=0.0f; output.decision = -1; return output;}

    //descrete classification decision for a given class index 
    //return either 0 for negative and 1 for positive decision
    //      or -1 for no decision 
    virtual classifyDiscreteResult classifyDiscrete(const FeatureVectorViewType &fv, const uint32_t classindex) const 
    { classifyDiscreteResult output; output.confidence=0.0f; output.decision = -1; return output;}

    //continuous classification decision 
    //will return a real number (e.g. posterior) for each class 
    virtual void classifyContinuous(const FeatureVectorViewType &fv, std::vector<float32_t> & output) const 
    { output.clear();}

    //continuous classification decision 
    //will return a real number (e.g. posterior) for the given class index
    virtual float32_t classifyContinuous(const FeatureVectorViewType &fv, const uint32_t classindex) const 
    { return 0.0f; }
	*/
    //set classifier model, the classifier will make a copy if modelCopy is true. 
    //return true if the model has been copied, false otherwise
    virtual bool setClassifierModel(_ClassifierModel &model) = 0;  
    //get the classifier model (will return the constant reference)
    virtual _ClassifierModel & getClassifierModel(void) = 0; 

private:
    bool modelCopy; 

};




}   // namespace qcv

#endif // qcv_classifier_h_
//
