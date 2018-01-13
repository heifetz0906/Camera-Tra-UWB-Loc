/**=============================================================================

@file
LOMO.h

@brief
Person Signature model, this can include full body, half-body, facial features. 

Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

#ifndef qcv_person_signature_model_h_
#define qcv_person_signature_model_h_


#include <cstddef>
#include <vector>

#include "qcvTypes.h"
#include "ClassSample.h"

namespace qcv
{

/*********************************************
Human signature models 
can include a full set of human body parts models: 
e.g. full-body, half-body, face features, etc. 
*****************************************************************/
enum PersonBodyParts
{
    FULL_BODY = 0,
    UPPER_BODY = 1
};


/*********************************************
//Human Body Color and Shape model
can include a full set of human body parts models:
e.g. full-body, half-body, face features, etc.
*****************************************************************/
template<class _DataType>
class PersonBodyModel
{

public:
    //person body model, mostly color and texture based. 
    //basically holds a set of samples for the same body/parts
    PersonBodyModel(uint32_t personID, uint32_t featureLength, uint32_t initReserveSample, uint32_t maxNumSample) :
        samples(personID, featureLength, 0, initReserveSample, maxNumSample) //contains only the original samples, not the pre-computed ones. the pre-computation only associated with the classifier and metric
    {

    }
    virtual ~PersonBodyModel() { };

    //public functions
    //the length of the feature
    uint32_t getFeatureLength(void) const
    {
        return samples.getFeatureLength();
    }

    uint32_t getNumSamples(void) const
    {
        return samples.getNumSamples();
    }

    //add a sample 
    bool addSample(const float32_t *sample)
    {
        //contains only the original samples, not the pre-computed ones. 
        return samples.addSample(sample, NULL);
    }

    LabeledSampleEnsemble<float32_t> *getSample()
    {
        return &samples;
    }

private:
    LabeledSampleEnsemble<float32_t> samples;
};


class PersonSignatureModel
{

public:
    //person body model, mostly color and texture based. 
    //basically holds a set of samples for the same body/parts
    PersonSignatureModel(uint32_t personID, uint32_t reserveSamplePerModel, uint32_t maxNumSamplePerModel);
    virtual ~PersonSignatureModel();

    //public functions

    //get the body model
    PersonBodyModel<float32_t> *getBodyModel(PersonBodyParts partID);
   
    //add body sample 
    bool addBodySample(const float32_t *sample, uint32_t sampleLength, PersonBodyParts partID);
    
private:
    //create a body Model 
    bool initBodyModel(uint32_t featureLength, uint32_t reserveNumSample, uint32_t maxNumSample, PersonBodyParts partID);

public: 
    uint32_t id;
    
private:    
    uint32_t reserveSample;
    uint32_t maxSample;
    PersonBodyModel<float32_t> *fullBodyModel;
    PersonBodyModel<float32_t> *upperBodyModel;  
};



}   // namespace qcv

#endif // qcv_person_signature_model_h_
//


