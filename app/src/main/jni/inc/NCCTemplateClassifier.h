//
//
/**=============================================================================

@file
NCCTemplateClassifier.h

@brief
Definition of an NCC based template-matching classifier.

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

#ifndef qcv_ncc_template_classifier_h_
#define qcv_ncc_template_classifier_h_

#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cassert>
#include <memory>
#include <iostream>
#include <fstream>
#include <cmath>

#include "qcvMath.h"
#include "qcvTypes.h"
#include "qcvImage.h"
#include "Classifier.h"
#include "qcvMemory.h"


namespace qcv
{

/*==========================================================================
NCC-based Template Verification/classification
===========================================================================*/
    
////////////////////////////////////////////////////////////////////////////////////////////////////
//NCC Image Template
//////////////////////////////////////////////////////////////////////////////////////////////////////
template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT>
class NCCTemplate 
{   
public: 
    NCCTemplate()    { dataLength = _PATCH_WIDTH * _PATCH_HEIGHT;   }
    NCCTemplate(float32_t *TemplateData, bool classLabel)    
    {
        setValue(TemplateData, classLabel);
    }

    void setValue(const float32_t *TemplateData, const bool classLabel) 
    {
        label = classLabel;
        memcpy(values, TemplateData, dataLength * sizeof(float32_t));
        autocorr = fcvDotProductf32(values, values, dataLength); //autoCorrelation<float32_t>(values, dataLength);
    }

    Size2D<uint32_t> getPatchSize(void) const {return Size2D<uint32_t>(_PATCH_WIDTH, _PATCH_HEIGHT); }
    uint32_t getDataLength(void) const {return dataLength;}

    //get class label 
    bool getClassLabel(void) const {return label;}    

    //get data values 
    const float32_t *getData(void) const {return values;}

    //access to the data array directly 
    float32_t operator[] (uint32_t index) const
    {
        assert(index < dataLength);
        return values[index];
    }
    
    //get the auto correlation of the template 
    float32_t getAutoCorr(void) const {return autocorr;}

    //save the template
    void write(std::ofstream &outfile) const
    {                    
        for (uint32_t i=0; i<dataLength; i++)
            outfile << values[i] << "\t"; 
        outfile << "\n";
    }

    //write to an image
    void visualize( uint8_t *img, uint32_t stride) const
    {
        //normalize the template to fit 0 - 255
        float32_t min = 256, max = -256;
        for (uint32_t i=0;i<_PATCH_WIDTH*_PATCH_HEIGHT;i++)
        {
            if (values[i] < min)
                min = values[i];
            if (values[i] > max)
                max = values[i];
        }
        //write into the image
        uint8_t *line = img;
        const float32_t *tmpl  = values;
        for (uint32_t i=0;i<_PATCH_HEIGHT;i++)
        {
            for (uint32_t j=0; j<_PATCH_WIDTH; j++)
                line[j] = (uint8_t)((tmpl[j] - min)/(max-min)*255);
            line += stride;            
            tmpl += _PATCH_WIDTH;
        }
    }

private:
    bool label; //true for positive, and false for negative
	float32_t values[_PATCH_WIDTH*_PATCH_HEIGHT];
    uint32_t  dataLength;
    float32_t autocorr; //auto-correlation of the patch
    

};


////////////////////////////////////////////////////////////////////////////////////////////////////
//NCC Classifier model 
//////////////////////////////////////////////////////////////////////////////////////////////////////
template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
class NCCTemplateClassifierModel //model variales that changes during online learning
{

public: 
    NCCTemplateClassifierModel( ) : 
        sizePositiveModelPatches(0), sizeNegativeModelPatches(0)
    { 
        defaultThreshold();

        //reserve memory size for vectors 
        //PositiveModelPatches.reserve(300); 
        //NegativeModelPatches.reserve(300);    
    };

    uint32_t getNumClass() const {return 2;}
    uint32_t getFeatureLength() const {return _PATCH_WIDTH*_PATCH_HEIGHT;} 

    //~NCCTemplateClassifierModel() { }

    bool addTemplate(const NCCTemplate<_PATCH_WIDTH, _PATCH_HEIGHT> &tmpl)
    {
        bool success = true;
        if (tmpl.getClassLabel()) 
        {
            if (sizePositiveModelPatches < _MAX_NUM_POS_TMPL)
            {
                PositiveModelPatches[sizePositiveModelPatches] = tmpl;
                sizePositiveModelPatches++;
            }
        }
        else if (sizeNegativeModelPatches < _MAX_NUM_NEG_TMPL)
        {
            NegativeModelPatches[sizeNegativeModelPatches] = tmpl;
            sizeNegativeModelPatches++;
        }
        else 
            success = false;
        return success;
    }


    bool addTemplate(const float32_t *fv, const bool label)
    {
        bool success = true;
        if (label) 
        {
            if (sizePositiveModelPatches < _MAX_NUM_POS_TMPL)
            {
                PositiveModelPatches[sizePositiveModelPatches].setValue(fv, label);
                sizePositiveModelPatches++;
            }
        }
        else if (sizeNegativeModelPatches < _MAX_NUM_NEG_TMPL)
        {
            NegativeModelPatches[sizeNegativeModelPatches].setValue(fv, label);
            sizeNegativeModelPatches++;
        }
        else
            success=false;
        return success;
    }

    void reset()
    {
        sizePositiveModelPatches=0;
        sizeNegativeModelPatches=0;    
        defaultThreshold();
    }

    void defaultThreshold() 
    {
        thresholdPositiveClass = 0.50f; //0.60f;
        thresholdNegativeClass = 0.50f;
    }

    void setThreshold(float32_t thPos, float32_t thNeg)
    {
        thresholdPositiveClass = thPos;
        thresholdNegativeClass = thNeg;
    }

    void writeTemplates(const std::string &filename)
    {    
        //save positive templates
        std::string outName = filename + "_pos.txt";
        std::ofstream outfile;
        outfile.open(outName.c_str(), std::ios::out);
        for (uint32_t i=0; i< sizePositiveModelPatches; i++)
            PositiveModelPatches[i].write(outfile);
        outfile.close();

        //save negative templates
        outName = filename + "_neg.txt";        
        outfile.open(outName.c_str(), std::ios::out);
        for (uint32_t i=0; i< sizeNegativeModelPatches; i++)
            NegativeModelPatches[i].write(outfile);
        outfile.close();
        
    }

    //visualize negative and positive templates into images
    void visualize( uint8_t *posImg, uint8_t *negImg, uint32_t width, uint32_t height, uint32_t stride) const
    {
        //set the both image to zero
        std::memset(posImg, 0, height*stride);
        std::memset(negImg, 0, height*stride);
        //estimate number of templates per row and per columns
        uint32_t numRow = height / _PATCH_HEIGHT;
        uint32_t numCol = width / _PATCH_WIDTH;
        //for positive templates
        uint32_t numTempToViz = MIN(numRow * numCol, sizePositiveModelPatches);        
        uint8_t *imgPtrRow = posImg;
        uint8_t *imgPtr = imgPtrRow;
        for (uint32_t i = 0; i < numTempToViz; i++)
        {
            PositiveModelPatches[i].visualize(imgPtr, stride);
            imgPtr += _PATCH_WIDTH;
            if ( (i+1) % numCol == 0 ) 
            {
                imgPtrRow += _PATCH_HEIGHT * stride;
                imgPtr = imgPtrRow;
            }
        }
        //for negative templates
        numTempToViz = MIN(numRow * numCol, sizeNegativeModelPatches);        
        imgPtrRow = negImg;
        imgPtr = imgPtrRow;
        for (uint32_t i = 0; i < numTempToViz; i++)
        {
            NegativeModelPatches[i].visualize(imgPtr, stride);
            imgPtr += _PATCH_WIDTH;
            if ( (i+1) % numCol == 0 ) 
            {
                imgPtrRow += _PATCH_HEIGHT * stride;
                imgPtr = imgPtrRow;
            }
        }
    }


public: 
    //some thresholds 
    float32_t   thresholdPositiveClass;         //Threshold of positive patch classification (in NCC), for classification
    float32_t   thresholdNegativeClass;         //Threshold of negative patch classification (in NCC) 
    
    //////////////////////////////////////////////////////////////////////////////////////
    //NCC templates
    //std::vector< NCCTemplate<_PATCH_SIZE > >         PositiveModelPatches; //[_MAX_NUM_POS_TMPL];          //Store the positive patch templates for NCC 
    //std::vector< NCCTemplate<_PATCH_SIZE > >         NegativeModelPatches; //[_MAX_NUM_NEG_TMPL];          //Store the negative patch templates for NCC
    NCCTemplate<_PATCH_WIDTH, _PATCH_HEIGHT >   PositiveModelPatches[_MAX_NUM_POS_TMPL];          //Store the positive patch templates for NCC 
    NCCTemplate<_PATCH_WIDTH, _PATCH_HEIGHT >   NegativeModelPatches[_MAX_NUM_NEG_TMPL];          //Store the negative patch templates for NCC
    uint32_t         sizePositiveModelPatches;       //The size of positive patches
    uint32_t         sizeNegativeModelPatches;       //The size of negative patches

}; 



////////////////////////////////////////////////////////////////////////////////////////////////////
//NCC Classifier
//////////////////////////////////////////////////////////////////////////////////////////////////////

template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
class NCCTemplateClassifier : public Classifier < float32_t, NCCTemplateClassifierModel<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL> >
{
    typedef FeatureVectorView<float32_t> FeatureVectorViewType;
    typedef NCCTemplateClassifierModel<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL> NCCClassifierModel;
public: 
    //default two classifiers 
    NCCTemplateClassifier( NCCClassifierModel &  nccClassifierModel, bool copyModel); 

    ~NCCTemplateClassifier( ); 
    
    //public member functions
    uint32_t getNumClass( ) const {return 2;}
    uint32_t getFeatureLength( ) const {return featureLength;}

    //descrete classification decision
    //return either a class index from 0 to numClass-1, 
    //      or -1 for no decision 
    classifyDiscreteResult classifyDiscrete(const FeatureVectorViewType &fv) const ;

    //descrete classification decision for a given class index 
    //return either 0 for negative and 1 for positive decision
    //      or -1 for no decision 
    classifyDiscreteResult classifyDiscrete(const FeatureVectorViewType &fv, const uint32_t classindex) const;

    //continuous classification decision 
    //will return a real number (e.g. posterior) for each class 
    void classifyContinuous(const FeatureVectorViewType &fv, std::vector<float32_t> & output) const;

    //continuous classification decision 
    //will return a real number (e.g. posterior) for the given class index
    float32_t classifyContinuous(const FeatureVectorViewType &fv, const uint32_t classindex) const;

    //set classifier model (only refer to the model, no local copy maintained)
    bool setClassifierModel(NCCClassifierModel &model);   

    //get the classifier model (will return the constant reference)
    NCCTemplateClassifierModel<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL> & getClassifierModel(void);     

private: 

    //compute normalized cross correlation between two image patches b1 and b2
    //n1 and n2 are autocorrelation of b1 and b2
    float32_t ncc(const float32_t *b1, float32_t n1, const float32_t *b2, float32_t n2) const; 

    //compute ncc confidence using the templates
    float32_t templateMatching(const float32_t *fv) const;
    
private: 
    //member variables 
    NCCClassifierModel *nccModel;
    uint32_t        featureLength;

}; 


//default two classifiers 
template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
NCCTemplateClassifier<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>::
    NCCTemplateClassifier( NCCClassifierModel &  nccClassifierModel, bool copyModel)   
    : Classifier < float32_t, NCCTemplateClassifierModel<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL> >(copyModel)    
{
    featureLength = _PATCH_WIDTH*_PATCH_HEIGHT;
    if (copyModel)
        nccModel = new NCCClassifierModel();
    setClassifierModel(nccClassifierModel);    
}


template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
NCCTemplateClassifier<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>::
    ~NCCTemplateClassifier( )
{
    if (this->hasModelCopy() && nccModel)
        delete nccModel;
}


//descrete classification decision
//return either a class index from 0 to numClass-1, 
//      or -1 for no decision 
template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
classifyDiscreteResult NCCTemplateClassifier<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>::
    classifyDiscrete(const FeatureVectorViewType &fv) const
{
    assert(fv.getLength()==getFeatureLength()); 
    
    //check positive class 
    float32_t posMatching = templateMatching(fv.getData());
    float32_t posConf = posMatching - nccModel->thresholdPositiveClass;
    
    //check negative class
    float32_t negConf = 1-posMatching - nccModel->thresholdNegativeClass;
    

    classifyDiscreteResult output;
    if (negConf > posConf)
    {
        output.confidence = negConf;
        output.decision = 0;
    }
    else if (negConf < posConf)
    {
        output.confidence = posConf;
        output.decision = 1;
    }
    else
    {
        output.confidence = 0;
        output.decision = -1;    
    }
    return output;
}


//descrete classification decision for a given class index 
//return either 0 for negative and 1 for positive decision
//      or -1 for no decision 
template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
classifyDiscreteResult NCCTemplateClassifier<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>::
    classifyDiscrete(const FeatureVectorViewType &fv, const uint32_t classindex) const
{
    assert(fv.getLength()==getFeatureLength()); 
    assert(classindex < 2);
   
    classifyDiscreteResult output;

    if (nccModel->sizeNegativeModelPatches ==0 || nccModel->sizePositiveModelPatches==0)
    {
        output.confidence = 0;
        output.decision = -1;
    }
    else
    {    
        if (classindex==0) //for negative classification 
            output.confidence = 1-templateMatching(fv.getData()) - nccModel->thresholdNegativeClass;
        else 
            output.confidence = templateMatching(fv.getData()) - nccModel->thresholdPositiveClass;

        if (output.confidence > 0) 
            output.decision = 1;
        else if (output.confidence < 0)
            output.decision = 0;
        else
            output.decision = -1;
    }
    return output;
}


//continuous classification decision 
//will return a real number (e.g. posterior) for each class 
template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
void NCCTemplateClassifier<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>::
    classifyContinuous(const FeatureVectorViewType &fv, std::vector<float32_t> & output) const
{
    assert(fv.getLength()==getFeatureLength()); 

    output.resize(2);
    float32_t posMatching = templateMatching(fv.getData());
    output[0] = 1-posMatching - nccModel->thresholdNegativeClass;
    output[1] = posMatching - nccModel->thresholdPositiveClass;
}


//continuous classification decision 
//will return a real number (e.g. posterior) for the given class index
template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
float32_t NCCTemplateClassifier<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>::
    classifyContinuous(const FeatureVectorViewType &fv, const uint32_t classindex) const
{
    assert(fv.getLength()==getFeatureLength()); 
    assert(classindex < 2);

    if (classindex==0) 
        return 1-templateMatching(fv.getData()) - nccModel->thresholdNegativeClass;
    else 
        return templateMatching(fv.getData()) - nccModel->thresholdPositiveClass;
}


//set classifier model
template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
bool NCCTemplateClassifier<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>::
    setClassifierModel(NCCClassifierModel &model)
{
    if (this->hasModelCopy())
        memcpy(nccModel, &model, sizeof(NCCClassifierModel));
    else
        nccModel = &model;
    return this->hasModelCopy();
}


//get the classifier model (will return the constant reference)
template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
NCCTemplateClassifierModel<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL> &  
    NCCTemplateClassifier<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>::
    getClassifierModel(void)
{
    return *nccModel;
}


//compute normalized cross correlation between two image patches b1 and b2
//n1 and n2 are autocorrelation of b1 and b2
//for faster computation, the output value is not normalized, value range between -1 and 1
template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
float32_t NCCTemplateClassifier<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>::
    ncc(const float32_t *b1, float32_t n1, const float32_t *b2, float32_t n2) const
{
    float32_t ncc;

    float32_t ccorr = fcvDotProductf32(b1, b2, featureLength);

    //ncc = (ccorr / sqrt(n1 * n2) + 1) / 2.0f;
    if (ccorr>=0)
        //ncc = (ccorr*ccorr / (n1 * n2) + 1) / 2.0f;
        ncc = ccorr*ccorr / (n1 * n2);
    else
        //ncc = (-ccorr*ccorr / (n1 * n2) + 1) / 2.0f;
        ncc = -ccorr*ccorr / (n1 * n2);
    return ncc;
}


//compute ncc confidence using the templates, finding nearest neighbor from both positive and negative side
//and determine the ncc distance (normalized)
template<uint32_t _PATCH_WIDTH, uint32_t _PATCH_HEIGHT, uint32_t _MAX_NUM_POS_TMPL, uint32_t _MAX_NUM_NEG_TMPL>
float32_t NCCTemplateClassifier<_PATCH_WIDTH, _PATCH_HEIGHT, _MAX_NUM_POS_TMPL, _MAX_NUM_NEG_TMPL>::
    templateMatching(const float32_t *fv) const
{

    float32_t corr =    -1.0f;    
    float32_t corrP =   -1.0f;
    float32_t corrP2 =  -1.0f;
    float32_t corrN =    -1.0f;
    float32_t corrN2 =    -1.0f;
    float32_t distanceP =    0;
    float32_t distanceN =    0;
    float32_t distancePatch =    0;

    if(nccModel->sizePositiveModelPatches== 0)
        return 0.0;

    if(nccModel->sizeNegativeModelPatches == 0)
        return 1.0;

    //first compute the auto-correlation for the input vector 
    float32_t autocorr = fcvDotProductf32(fv, fv, featureLength);
    
#if 1
    //using 1-NN
    //Get the maximum ncc through all the positive patches
    for(uint32_t i = 0; i < nccModel->sizePositiveModelPatches; i++)
    {
        corr = ncc(nccModel->PositiveModelPatches[i].getData(), nccModel->PositiveModelPatches[i].getAutoCorr(), fv, autocorr);
        if(corr > corrP)
            corrP = corr;
    }

    //Get the maximum ncc through all the negative patches
    for(uint32_t i = 0; i < nccModel->sizeNegativeModelPatches; i++)
    {
        corr = ncc(nccModel->NegativeModelPatches[i].getData(), nccModel->NegativeModelPatches[i].getAutoCorr(), fv, autocorr);
        if(corr > corrN)
            corrN = corr;
    }

    //return ncc to the original value and range from 0 to 1
    if (corrP >= 0 )
        corrP = (sqrt(corrP) + 1) / 2.0f;
    else
        corrP = (-sqrt(-corrP) + 1) / 2.0f;

    if (corrN >= 0)
        corrN = (sqrt(corrN) + 1) / 2.0f;
    else
        corrN = (-sqrt(-corrN) + 1) / 2.0f;

#else 
    //using 2-NN
    //Get the maximum ncc through all the positive patches
    for(uint32_t i = 0; i < nccModel->sizePositiveModelPatches; i++)
    {
        corr = ncc(nccModel->PositiveModelPatches[i].getData(), nccModel->PositiveModelPatches[i].getAutoCorr(), fv, autocorr);
        if (corr > corrP)
        {
            corrP2 = corrP;
            corrP = corr;            
        }
        else if (corr > corrP2)
            corrP2 = corr;
    }    
    //return ncc to the original value and range from 0 to 1
    //corrP
    if (corrP >= 0 )
        corrP = (sqrt(corrP) + 1) / 2.0f;
    else
        corrP = (-sqrt(-corrP) + 1) / 2.0f;
    //corrP2
    if (nccModel->sizePositiveModelPatches > 1)
    {
        if (corrP2 >=0)
            corrP2 = (sqrt(corrP2) + 1) / 2.0f;
        else
            corrP2 = (-sqrt(-corrP2) + 1) / 2.0f;
        corrP = (corrP + corrP2) / 2;
    }
    
    //Get the maximum ncc through all the negative patches
    for(uint32_t i = 0; i < nccModel->sizeNegativeModelPatches; i++)
    {
        corr = ncc(nccModel->NegativeModelPatches[i].getData(), nccModel->NegativeModelPatches[i].getAutoCorr(), fv, autocorr);
        if(corr > corrN)
        {
            corrN2 = corrN;
            corrN = corr;
        }
        else if (corr > corrN2)
            corrN2 = corr;
    }      
    //return ncc to the original value and range from 0 to 1
    //corrN
    if (corrN >= 0)
        corrN = (sqrt(corrN) + 1) / 2.0f;
    else
        corrN = (-sqrt(-corrN) + 1) / 2.0f;
    //corrN2
    if (nccModel->sizeNegativeModelPatches > 1)
    {
        if (corrN2 >= 0)
            corrN2 = (sqrt(corrN2) + 1) / 2.0f;
        else
            corrN2 = (-sqrt(-corrN2) + 1) / 2.0f;
        corrN = (corrN + corrN2) / 2;
    }
#endif

    //threshold corrP and corrN
    //if (corrP > 0.2)
        distanceP = 1.0f - corrP;
    //else 
    //    distanceP = 1.0f;

    //if (corrN > 0.2)
        distanceN = 1.0f - corrN;
    //else
    //    distanceP = 1.0f;

    distancePatch = distanceN / (distanceP + distanceN);

    return distancePatch;
}


} //namespace qcvlib

#endif  //qcv_ncc_template_classifier_h_

