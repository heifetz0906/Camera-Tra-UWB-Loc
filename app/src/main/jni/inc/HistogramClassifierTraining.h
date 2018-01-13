/**=============================================================================

@file
HistogramClassifierTraining.h

@brief
Definition of a histogram based classifier class. 

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

#ifndef qcv_histogramclassifier_training_h_
#define qcv_histogramclassifier_training_h_


#include <cstddef>
#include <vector>
#include <cassert>

#include "qcvTypes.h"
#include "ClassifierTraining.h"
#include "HistogramClassifier.h"


namespace qcv
{


/* ==================================================================
class HistogramClassifier1DTraining
Descritpion: 1D histogram-based Training with online learning  
=================================================================== **/
class HistogramClassifier1DTraining : public ClassifierTraining<uint32_t, HistogramClassifier1DModel> 
{    
public :
    HistogramClassifier1DTraining(uint32_t numClass, uint32_t numHistBins, const bool useLikelihood, const bool useEqualPrior);
    virtual ~HistogramClassifier1DTraining();
        
    //public functions
    uint32_t getNumClass() const { return histModel.getNumClass(); }
    
    //for histogram 1D the feature length is always 1
    uint32_t getFeatureLength() const {return (uint32_t)1; }


    //adding training sampels only
    //each implmented class will decide how to accumulate the samples, but should not update the model until the update() function is called    
    //return false if the sample is not added
    bool addSample(TrainingSample<uint32_t> &sample);
    
    //update the classifier model using the added samples, returned the number of samples learned
    uint32_t update();

    //update classifier with a sample
    //return true if the classifier is updated with the given sample
    //return false if the classifier is not updated     
    //bool trainIncremental(TrainingSample<uint32_t> &sample);

    //get the classifier model (the user of classifier model cannot change the model)
    HistogramClassifier1DModel& getClassifierModel(void); 
    
    //set ClassifierModel
    //will maintain local copy since the model will be updated
    void setClassifierModel(HistogramClassifier1DModel &model);     

    //reset the classifier model 
    void resetClassifierModel(void);

    //////////////////////////////////////////////////////////////
    //set variables 
    void setHistograms(const uint32_t *hist);
    void setClassPrior(const float32_t *prior);
    void setClassSample(const uint32_t *nsample);

    void writeClassHistograms(const char *filename);

private:    
    //reset likelihood histogram and the number of samples accumulated
    void resetHistogram();

private:
    HistogramClassifier1DModel histModel;

    //histogram of likelihoods (managed locally)
    uint32_t **classHistogram; //histogram (or likelihood) for each class       
    float32_t *classPrior; //prior for each class     
    uint32_t *classSample; //number of samples for each class

    //for learning incremental accumulation
    uint32_t **classHistogramInc; //incremental histogram for each class
    uint32_t *classSampleInc; //number of samples for each class
    
    bool flagLikelihood; //wether the histograms are likelihoods (i.e. normalized)
    bool flagEqualPrior; //wether each class has the same prior (for computing posteriors)
    
};


}   // namespace qcv

#endif // qcv_histogramclassifier_training_h_
//




