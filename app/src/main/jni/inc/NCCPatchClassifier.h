//
//
/**=============================================================================

@file
nccPatchClassifier->h

@brief
Implementation of an NCC template based image patch classifier and it's PN learner

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

#ifndef qcv_nccpatchclassifier_h_
#define qcv_nccpatchclassifier_h_

#include <cstddef>
#include <vector>
#include <cassert>

#include "qcvImageExtension.h"
#include "ImagePatchClassifier.h"
#include "ImagePatchClassifierSimple.h"
#include "ImagePatchClassifierSimplePNTraining.h"
#include "ClassifierPNTraining.h"
#include "NCCTemplateClassifier.h"
#include "NCCTemplateClassifierTraining.h"
#include "NormalizedPatch.h"
#include "qcvMemory.h"

namespace qcv
{

/********************************************
the NCC patch template-based classification, which implements a image patch classifier simple 
*********************************************/
template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
class NCCImagePatchClassifier: public ImagePatchClassifier <float32_t>
{
public:
    NCCImagePatchClassifier(NCCTemplateClassifierModel<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL> &  nccClassifierModel, bool copyModel )    : 
        nccPatchExtra(Size2D<float32_t>(_PATCH_WIDTH, _PATCH_HEIGHT), true),
        nccClassifier(nccClassifierModel, copyModel),
        detectorImpl(nccPatchExtra, nccClassifier, 1)
    {
        //set the temporary buffer
        //uint32_t buffSize = NormalizedPatch<_NCC_PATCH_SIZE>::getTempBufferSize(480, 270, 480)*12/10;
        //sBuffer.setSize(buffSize + 200, 16);
        //uint8_t* tempbuff = sBuffer.releaseMem(buffSize);
        //assert(tempbuff);                
        //nccPatchExtra.setTempBuffer(tempbuff);
                
        //set the detector with extractor and the classifier
        //detectorImpl.setFeatureExtractor(nccPatchExtra);
        //detectorImpl.setClassifier(nccClassifier, 1); //for positive class
    }

    ~NCCImagePatchClassifier()
    {

    }

    //Given an image patch, extract an image feature, and classify the patch         
    //imgExt contains pointer to the beginning of the image 
    //locXY gives the upper-left conrer of the patch     
    //Output: decision: 0 for negative response, 1 for positive response, -1 for no decision
    classifyDiscreteResult classifyPatch(ImageExtension &imgExt, const Point2D<float32_t> &locXY)
    {
        return detectorImpl.classifyPatch(imgExt, locXY);
    }
          
    void setPatchSize(const Size2D<float32_t> &patchSize)
    {
        detectorImpl.setPatchSize(patchSize);
    }

    Size2D<float32_t> getPatchSize(void) const    
    {
        return detectorImpl.getPatchSize();
    }

    Size2D<uint32_t> getNCCTemplateSize() const {return nccPatchExtra.getNormalizedPatchSize();}

    NCCTemplateClassifier<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL> 
    *getClassifier(void) {return &nccClassifier;}

    bool setClassifierModel(NCCTemplateClassifierModel<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>  &model)
    {
        return nccClassifier.setClassifierModel(model);
    }

    ImagePatchClassifierSimple<float32_t, NCCTemplateClassifierModel<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>, float32_t>  
        & getImagePatchClassifierSimple() {return detectorImpl;}

private: 

    //feature extractor 
    NormalizedPatch<_PATCH_WIDTH, _PATCH_HEIGHT>     nccPatchExtra;
    //internal classifier 
    NCCTemplateClassifier<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL> nccClassifier;
    
    //image patch classifier simple
    ImagePatchClassifierSimple<float32_t, NCCTemplateClassifierModel<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>, float32_t>  detectorImpl;
    
};



/************************************************************
NCC Template based patch classifier learning
************************************************************/
template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
class NCCImagePatchClassifierPNTraining  
{
public:
    NCCImagePatchClassifierPNTraining( )                    
    {       

        //internal learner for PN Learning
        nccLearner = new NCCTemplateClassifierTraining<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>( );

        //the ncc classifier 
        nccPatchClassifier = new NCCImagePatchClassifier<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>(nccLearner->getClassifierModel(), false);

        //image patch classifier simple PN learning 
        pnLearner = new ImagePatchClassifierSimplePNTraining<float32_t, NCCTemplateClassifierModel<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>, float32_t >
                        (nccPatchClassifier->getImagePatchClassifierSimple(), *nccLearner);
    }

    ~NCCImagePatchClassifierPNTraining()
    {
        if (pnLearner)
            delete pnLearner;
        if (nccPatchClassifier)
            delete nccPatchClassifier;
        if (nccLearner)
            delete nccLearner;
        
    }
    
    void setThreshConfLearn(float32_t thPos, float32_t thNeg) {   pnLearner->setThreshConfLearn(thPos, thNeg); }

    Size2D<uint32_t> getNCCTemplateSize() const {return nccPatchClassifier->getNCCTemplateSize();}

    //NCC Patch's own learning function which is different from the class's API
    bool trainImagePatch(ImageExtension &imgExt, const Point2D<float32_t> &locXY,
                        Size2D<float32_t> imagePatchSize, uint32_t trueClassIndex, bool forceToLearn, bool incrementalLearn)
    {        
        //set the image patch size to the feature extractor
        pnLearner->setPatchSize(imagePatchSize);
        pnLearner->setFlagForceToLearn(forceToLearn);
        pnLearner->setFlagIncrementalLearning(incrementalLearn);
        return pnLearner->trainImagePatch(imgExt, locXY, trueClassIndex);
    }

    uint32_t updateLearner()
    {
        return pnLearner->updateLearner();
    }


    NCCTemplateClassifierModel<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>
        & getClassifierModel() 
    {
        return nccLearner->getClassifierModel();
    }
    
private: 
    //image patch classifier simple PN learning 
    ImagePatchClassifierSimplePNTraining<float32_t, NCCTemplateClassifierModel<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>, float32_t > *pnLearner;
    
    //the ncc classifier 
    NCCImagePatchClassifier<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL> *nccPatchClassifier;
        
    //internal learner for PN Learning
    NCCTemplateClassifierTraining<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL> *nccLearner;
       
};



} //namespace qcv

#endif  //qcv_nccobjectverification_h_

