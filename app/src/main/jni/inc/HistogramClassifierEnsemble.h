/**=============================================================================

@file
HistogramClassifierEnsemble.h

@brief
Definition of an ensemble of histogram classifiers, it is an impmentation of the ensemble classifier. 

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

#ifndef qcv_histogramclassifierensemble_h_
#define qcv_histogramclassifierensemble_h_

#include <cstddef>
#include <vector>

#include "qcvTypes.h"
#include "EnsembleClassifier.h"
#include "HistogramClassifier.h"

#include "EnsembleClassifierTraining.h"
#include "HistogramClassifierTraining.h"

namespace qcv
{


//Definition of an ensemble of histogram classifiers, it is an impmentation of the ensemble classifier. 
class HistogramClassifier1DEnsemble : public Classifier<uint32_t, EnsembleClassifierSimpleModel<HistogramClassifier1DModel> >
{
public :
    typedef FeatureVectorView<uint32_t> FeatureVectorViewType;
    typedef EnsembleClassifierSimpleModel<HistogramClassifier1DModel> EnsembleHistModel;
    
    HistogramClassifier1DEnsemble(EnsembleHistModel &histModels, bool copyHistModel) :
        Classifier<uint32_t, EnsembleHistModel >(copyHistModel)
    {    
        uint32_t numClassifiers = histModels.size();

        //initialization the histogram classifiers and add to the ensemble classifier
        for (uint32_t h=0;h<numClassifiers;h++)
        {       
            HistogramClassifier1D *hCls= new HistogramClassifier1D (*(histModels.getModel(h)), copyHistModel);      
            histClassifiers.push_back(hCls);
            ensembleHistClassifier.addClassifier(*hCls);
        }    
    }


    ~HistogramClassifier1DEnsemble()
    {
        releaseClassifiers();
    }
    
    void releaseClassifiers()
    {
        for (uint32_t i=0;i<histClassifiers.size();i++)
            delete histClassifiers[i];
        histClassifiers.clear();
    }

    //public member functions
    uint32_t getNumClass( ) const {return ensembleHistClassifier.getNumClass();}
    uint32_t getFeatureLength( ) const {return ensembleHistClassifier.getFeatureLength();}

    //descrete classification decision
    //return either a class index from 0 to numClass-1, 
    //      or -1 for no decision 
    classifyDiscreteResult classifyDiscrete(const FeatureVectorViewType &fv) const 
    { return ensembleHistClassifier.classifyDiscrete(fv);}

    //descrete classification decision for a given class index 
    //return either 0 for negative and 1 for positive decision
    //      or -1 for no decision 
    classifyDiscreteResult classifyDiscrete(const FeatureVectorViewType &fv, const uint32_t classindex) const 
    { return ensembleHistClassifier.classifyDiscrete(fv, classindex); }

    //continuous classification decision 
    //will return a real number (e.g. posterior) for each class 
    void classifyContinuous(const FeatureVectorViewType &fv, std::vector<float32_t> & output) const 
    { ensembleHistClassifier.classifyContinuous(fv, output);}

    //continuous classification decision 
    //will return a real number (e.g. posterior) for the given class index
    float32_t classifyContinuous(const FeatureVectorViewType &fv, const uint32_t classindex) const 
    { return ensembleHistClassifier.classifyContinuous(fv, classindex); }

    //set classifier model (only refer to the model, no local copy maintained)
    bool setClassifierModel(EnsembleHistModel &model) 
    {  ensembleHistClassifier.setClassifierModel(model);  return hasModelCopy(); }

    //get the classifier model (will return the constant reference)
    EnsembleHistModel & getClassifierModel(void) 
    {   return ensembleHistClassifier.getClassifierModel();    }

    //set the bias for the ensemble classifier set by class index
    void setClassBias(const std::vector<float32_t> &bias)
    {
        ensembleHistClassifier.setClassBias(bias);
    }

    void setClassBias(const uint32_t classindex, const float32_t bias)
    {
        ensembleHistClassifier.setClassBias(classindex, bias);
    }




private:
    EnsembleClassifierSimple<uint32_t, HistogramClassifier1DModel> ensembleHistClassifier;
    std::vector< HistogramClassifier1D *> histClassifiers;
        
};

/*****************************************************************************
//Definition of the training of an ensemble of histogram classifiers, it is an impmentation of the ensemble classifier training
********************************************************************************/
class HistogramClassifier1DEnsembleTraining : public ClassifierTraining<uint32_t, EnsembleClassifierSimpleModel<HistogramClassifier1DModel> > 
{           
public :
    typedef EnsembleClassifierSimpleModel<HistogramClassifier1DModel> EnsembleHistModel;

    HistogramClassifier1DEnsembleTraining(const uint32_t numHistogramClassifier, const uint32_t numClass, const uint32_t numBin, const bool useLikelihood, const bool useEqualPrior) 
    {    
        for (uint32_t h=0;h<numHistogramClassifier;h++)
        { 
            HistogramClassifier1DTraining *hTrainer = new HistogramClassifier1DTraining(numClass, numBin, useLikelihood, useEqualPrior);   
            histTrainers.push_back(hTrainer);
            ensembleHistTraining.addTrainer(*hTrainer);        
        }

    }

    ~HistogramClassifier1DEnsembleTraining()
    {
        for (uint32_t i=0;i<histTrainers.size();i++)
            delete histTrainers[i];
        histTrainers.clear();
    }


    //public member functions
    uint32_t getNumClass() const {return ensembleHistTraining.getNumClass(); }
    uint32_t getFeatureLength() const {return ensembleHistTraining.getFeatureLength(); }

    //adding training sampels only
    //each implmented class will decide how to accumulate the samples, but should not update the model until the update() function is called    
    //return false if the sample is not added
    bool addSample(TrainingSample<uint32_t> &sample) {return ensembleHistTraining.addSample(sample);}
    
    //update the classifier model using the added samples, returned the number of samples learned
    virtual uint32_t update() {return ensembleHistTraining.update();}

    //get the classifier model (the user of classifier model cannot change the model)
    EnsembleHistModel& getClassifierModel(void) {return ensembleHistTraining.getClassifierModel();}
    
    //set ClassifierModel
    //will maintain local copy since the model will be updated
    void setClassifierModel(EnsembleHistModel &model) {ensembleHistTraining.setClassifierModel(model);}

    //reset the classifier model 
    void resetClassifierModel(void)    {ensembleHistTraining.resetClassifierModel();}

    std::vector<HistogramClassifier1DTraining *> getHistTrainers( ) { return histTrainers;}

private: 
    EnsembleClassifierSimpleTraining<uint32_t, HistogramClassifier1DModel> ensembleHistTraining;
    std::vector<HistogramClassifier1DTraining *> histTrainers;

};





}   // namespace qcv

#endif // qcv_histogramclassifierensemble_h_
//

