/**=============================================================================

@file
PersonReID.h

@brief
Class for Person Re-ID, which hold person models, extractors, and classification, and learning

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

#ifndef qcv_person_reid_h_
#define qcv_person_reid_h_


#include <cstddef>
#include <vector>
#include <cstdlib>

#include "qcvTypes.h"
#include "qcvImageExtension.h"

#include "PersonSignatureModel.h"
#include "PersonSignatureExtraction.h"
#include "NearestNeightborClassifier.h"
#include "NearestNeightborClassifierTraining.h"
#include "ClassifierPNTraining.h"
#include "QuadraticMetric.h"

#include "PersonReIDDefs.h"

namespace qcv
{

struct PersonReIDParams
{
    PersonReIDParams() : reserveClass(10), reserveSample(50),maxSample(0), 
                         xqdaDistTh((float32_t)XQDA_DIST_TH), nnConfTh((float32_t)NN_CONFIDENCE_TH), nnLearnConfTh((float32_t)NN_CONFIDENCE_TH_LEARN),
                         metricPreComp(false)
    { }

    uint32_t reserveClass; //number of classes to reserve
    uint32_t reserveSample; //number of sample to research for each model (for initialization), 0 means unlimited
    uint32_t maxSample; //maximum number of samples per model (hard limit)

    //thresholds 
    float32_t xqdaDistTh;
    float32_t nnConfTh;
    float32_t nnLearnConfTh;

    //precompute metric
    bool metricPreComp;

};
/**************************************************************
Person Re-ID class will be a stand-alone module that hold person models, 
extractors, re-identification and learning
*****************************************************************/
class PersonReID 
{
    
public :
    PersonReID(PersonReIDParams reIDParams);

    virtual ~PersonReID();

    //public functions   

    //add a new sample of a person with "personID", if the ID doesn't exist, a new model will be created
    //this only add samples, but will not update the model  
    bool addPersonSample(uint32_t personID, PersonBodyParts partID, ImageExtension &imgExt, const Rect<uint32_t> roi);

    //update the person models after adding samples 
    bool modelUpdate();

    //verification of a person with certain ID 
    bool reIDTest(PersonBodyParts partID, ImageExtension &imgExt, const Rect<uint32_t> roi, int32_t &outID, float32_t &confidence);   

    uint32_t getNumSamples(uint32_t personID, PersonBodyParts partID);

    PersonReIDParams getParameters(void) const;

private:
    //find person ID and return index, -1 means no person found
    int32_t personID2Index(uint32_t personID);
    

private:
    //member varialbes
    PersonReIDParams params;

    //person models
    std::vector<PersonSignatureModel *> personModels;
    
    //model feature extractor
    PersonSignatureExtraction signatureExtractor;
    
    //full body classifier and training     
    QuadraticMetric *fullBodyDistMetric;  //XQDA metric for distance meaurement
    NearestNeighborClassifier<float32_t> *fullBodyClassifier;
    NearestNeighborClassifierTraining<float32_t> *fullBodyClassifierTraining;    
    ClassifierPNTraining<float32_t, NearestNeighborModel<float32_t> > *fullBodyPNLearning;
    
    //upper body classifier and training 
    QuadraticMetric *upperBodyDistMetric;  //XQDA metric for distance meaurement
    NearestNeighborClassifier<float32_t> *upperBodyClassifier;
    NearestNeighborClassifierTraining<float32_t> *upperBodyClassifierTraining;    
    ClassifierPNTraining<float32_t, NearestNeighborModel<float32_t> > *upperBodyPNLearning;

    qcvMemory scratchBuffer;
};




}   // namespace qcv

#endif // qcv_person_reid_h_
//


