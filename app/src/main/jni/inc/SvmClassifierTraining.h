/**=============================================================================

@file
SvmClassifierTraining.h

@brief
Definition of a linear svm classifier class. 

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

#ifndef qcv_svmclassifier_training_h_
#define qcv_svmclassifier_training_h_

#include <cassert>
#include <cstddef>
#include <vector>

#include "qcvTypes.h"
#include "ClassifierTraining.h"
#include "SvmClassifier.h"
#include "QpSolver.h"

namespace qcv
{

/* ==================================================================
class SvmClassifierOnlineTraining
Descritpion: linear SVM based Training with online learning  
=================================================================== **/
template<class _DataType>
class SvmClassifierTraining : public ClassifierTraining<_DataType, SVMClassifierModelLinear<_DataType> >
{    
public :
	// max_cached_samples: feat_len, 
    //svm_c:cost
    SvmClassifierTraining(uint32_t max_cached_samples, uint32_t feat_len, float32_t svm_c, float32_t Jpos, uint32_t pruneSize) :
        svmModel(feat_len)
	{
		qp = new QpSolver<_DataType>(max_cached_samples, feat_len, svm_c, Jpos, pruneSize);
	}
    
	virtual ~SvmClassifierTraining()
	{ 
		delete qp;
	} 

    //public functions
    uint32_t getNumClass() const { return svmModel.getNumClass(); }
        
    //get feature length
    uint32_t getFeatureLength() const { return svmModel.getFeatureLength(); }
	
    // add only one training sample
	bool addSample(TrainingSample<_DataType> &sample)
	{       
        return qp->addSample(sample);
	}

	//update classifier with a sample
	uint32_t update()
	{
        // qp -> qp_opt(svmModel, tol, iter); 
	    // TODO(yang): consider changing to:
        uint32_t numSample = qp->optimize(tol,iter);        
        //get the model 
        qp->getModel(svmModel); 
        //prune samples for next optimization
        qp->prune();
        return numSample;
	}

    //get the classifier model (the user of classifier model cannot change the model)
    SVMClassifierModelLinear<_DataType>& getClassifierModel(void)
	{
        return svmModel; 
	}

    //set ClassifierModel
    //will maintain local copy since the model will be updated
	// TODO(yang): the input is usually const reference.
    void setClassifierModel(SVMClassifierModelLinear<_DataType> &model)
	{
	   svmModel = model;       
	}

    //reset the classifier model 
    void resetClassifierModel(void)
	{
		svmModel.resetModel();
	}

    void setTolMaxiter(float32_t toler, uint32_t maxIter)
    {
        tol = toler;
        iter = maxIter;
    }
    //////////////////////////////////////////////////////////////
    //set variables 
    //void writeWeights(const char *filename);

private:
	// TODO(yang): can be dangerous without the implementation of 
	// copy constructor and assignment operator
	// (rule of three for c++).
    SVMClassifierModelLinear<_DataType> svmModel; 
	QpSolver<_DataType> *qp;
    float32_t tol;
    uint32_t iter;
};

}   // namespace qcv

#endif // qcv_svmclassifier_training_h_
