//
//
/**=============================================================================

@file
NCCTemplateClassifierTraining.h

@brief
Training methods for NCCTemplateClassifier

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

#ifndef qcv_ncc_template_classifier_training_h_
#define qcv_ncc_template_classifier_training_h_

#include <cstddef>
#include <vector>
#include <cassert>

#include "ClassifierTraining.h"
#include "NCCTemplateClassifier.h"

namespace qcv
{

/********************************************************************
Training methods for NCCTemplateClassifier 
********************************************************************/
template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
class NCCTemplateClassifierTraining : public ClassifierTraining<float32_t, NCCTemplateClassifierModel<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL> >
{

    typedef NCCTemplateClassifierModel<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL> NCCClassifierModel;

public:

    NCCTemplateClassifierTraining() {    }
    
    //public member functions
    uint32_t getNumClass() const {return nccModel.getNumClass();}
    uint32_t getFeatureLength() const {return nccModel.getFeatureLength();} 


    //adding training sampels only
    //each implmented class will decide how to accumulate the samples, but should not update the model until the update() function is called    
    //return false if the sample is not added
    bool addSample(TrainingSample<float32_t> &sample)
    {
        assert(sample.featureView.getLength() == getFeatureLength() && sample.classIndex < 2);
        //add the positive or negative template directly to the Incremental model 
        return nccModelInc.addTemplate(sample.featureView.getData(), sample.classIndex==1);
    }
    
    //update the classifier model using the added samples, returned the number of samples learned
    uint32_t update()
    {
        uint32_t totalSample = 0;
        //add the templates from the incremental model to the model 
        for (uint32_t i=0; i< nccModelInc.sizePositiveModelPatches; i++)
            if (nccModel.addTemplate(nccModelInc.PositiveModelPatches[i]))
                totalSample++;
        for (uint32_t i=0; i< nccModelInc.sizeNegativeModelPatches; i++)
            if (nccModel.addTemplate(nccModelInc.NegativeModelPatches[i]))
                totalSample++;
        //reset the incremental 
        nccModelInc.reset();
        
        return totalSample;
    }

    //get the classifier model
    NCCClassifierModel & getClassifierModel(void) {return nccModel;}
    void setClassifierModel(NCCClassifierModel &model) 
    {nccModel = model;}

    //reset the classifier model 
    void resetClassifierModel(void)
    {
        nccModel.reset();
    }

private:
    //member variables 
    NCCClassifierModel nccModel;
    NCCClassifierModel nccModelInc; //the incremental model for updates
        
};



} //namespace qcvlib

#endif  //qcv_ncc_template_classifier_training_h_

