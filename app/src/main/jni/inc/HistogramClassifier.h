/**=============================================================================

@file
HistogramClassifier.h

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

#ifndef qcv_histogramclassifier_h_
#define qcv_histogramclassifier_h_


#include <cstddef>
#include <vector>
#include <cassert>

#include "qcvTypes.h"
#include "Classifier.h"
#include "ClassifierTraining.h"


namespace qcv
{

/* ==================================================================
class HistgraomClassifier1D
Descritpion: 1D histogram-based classifier acceptiong only unsigned integer 
as input, the bin width is now set to 1. So this has the simplest impelentation 
of an histogram. 
Future, we could use a more complete histogram class to hold the distribution 
=================================================================== **/

class HistogramClassifier1DModel
{
public:

    HistogramClassifier1DModel(): numClass(0),histLen(0), classPosterior(NULL), classBias(NULL) { }

    HistogramClassifier1DModel(uint32_t nClass, uint32_t hLen) : classPosterior(NULL), classBias(NULL) 
    { initialize(nClass, hLen); }

    virtual ~HistogramClassifier1DModel()     { uninitialize();  }
    
    //public functions 
    bool initialize(uint32_t nClass, uint32_t numHistBins)
    {
        if (!uninitialize())
            return false;

        numClass = nClass;
        histLen = numHistBins;

        //allocate memory for class posteriors    
        float32_t *membuff = (float32_t *)MEMALLOC(numClass*histLen*sizeof(float32_t), 16);
        //std::memset(membuff, 0, nClass*histLen*sizeof(float32_t));
        //preset the class posterior histgram is 1/NumClass
        for (uint32_t i=0; i< numClass*histLen; i++)
            membuff[i] = 1.0f/numClass;
        //classPosterior
        classPosterior = (float32_t **) MEMALLOC(numClass*sizeof(float32_t *), 16);
        for (uint32_t i=0;i<numClass;i++)
        {
            classPosterior[i] = membuff;
            membuff += histLen; 
        }

        classBias = (float32_t *)MEMALLOC(numClass*sizeof(float32_t), 16);
        memset(classBias, 0, numClass*sizeof(float32_t));

        return true;
    }

    bool uninitialize( )
    {
        numClass = 0;
        histLen = 0;
        if (classPosterior)
        {
            MEMFREE(classPosterior[0]);
            MEMFREE(classPosterior);
            classPosterior = NULL;
        }

        if (classBias)
        {
            MEMFREE(classBias);
            classBias = NULL;
        }
        return true;
    }

    //reset model values
    void resetModel(void) 
    {
        //std::memset(classPosterior[0], 0, numClass*histLen*sizeof(float32_t));
        float32_t *membuff = classPosterior[0];
        for (uint32_t i=0; i< numClass*histLen; i++)
            membuff[i] = 1.0f/numClass;
        std::memset(classBias, 0, numClass*sizeof(float32_t));
    }


    //copy from another model 
    bool copyFrom(const HistogramClassifier1DModel &other)
    {
        
        if (other.getNumClass() !=numClass || other.getHistogramLength() !=histLen)
        {
            if (!initialize(other.getNumClass(), other.getHistogramLength()) )
                return false;
        }
        
        //copy data
        setClassPosterior(other.classPosterior[0]);
        setClassBias(other.classBias);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    //set data
    void setClassPosterior(float32_t *post)
    {
        std::memcpy(classPosterior[0], post, numClass*histLen*sizeof(float32_t));    
    }

    void setPosteriorValue(uint32_t classIndex, uint32_t binIndex, const float32_t value)
    {
        assert(classIndex< numClass && binIndex < histLen);
        classPosterior[classIndex][binIndex] = value;
    }
        
    void setClassBias(const float32_t *bias)
    {
        std::memcpy(classBias, bias, numClass*sizeof(float32_t));    
    }
    void setClassBias(const uint32_t classIndex, const float32_t bias)
    {
        assert(classIndex < numClass);
        classBias[classIndex] = bias;
    }

    //////////////////////////////////////////////////////////////
    ////get data
    ////////////////////////////////////////////////////////////
    uint32_t getNumClass(void) const {return numClass;}
    uint32_t getHistogramLength(void) const {return histLen;}
    
    float32_t *getClassPosterior(uint32_t classIndex)
    {
        assert(classIndex<numClass);
        return classPosterior[classIndex];
    }

    const float32_t *getClassPosterior(uint32_t classIndex) const 
    {
        assert(classIndex<numClass);
        return classPosterior[classIndex];
    }

    float32_t evalutePosterior(uint32_t classIndex, uint32_t binIndex) const
    {
        assert(classIndex < numClass);
        assert(binIndex < histLen);
        return classPosterior[classIndex][binIndex];
    }

    float32_t getClassBias(uint32_t classIndex) const
    {
        assert(classIndex<numClass);
        return classBias[classIndex];
    }

    void writeClassPosterior(const char *filename)
    {    
        std::ofstream outfile;
        outfile.open(filename, std::ios::out);
        for (uint32_t bin=0;bin<histLen;bin++)
        {
            outfile << bin << "\t\t"; 
            for (uint32_t c=0;c<numClass;c++)
            {
                outfile << evalutePosterior(c, bin)<< "\t\t\t\t" ; //posterior 
            }
            outfile << "\n";
        }
        outfile.close();
    }


private:    
    uint32_t numClass;
    uint32_t histLen;
    //posterior histogram    
    float32_t** classPosterior; //posterior for each class    
    float32_t* classBias; //the bias for each class to offset the posterior 

};

class HistogramClassifier1D : public Classifier<uint32_t, HistogramClassifier1DModel> 
{    

    typedef FeatureVectorView<uint32_t> FeatureVectorViewType;

public :
    HistogramClassifier1D(HistogramClassifier1DModel &histogramModel, bool copyModel);
    virtual ~HistogramClassifier1D();
        
    //public functions
    uint32_t getNumClass( ) const { return histModel->getNumClass(); }
    
    //for histogram 1D the feature length is always 1
    uint32_t getFeatureLength( ) const {return (uint32_t)1;}

    //descrete classification decision
    //return either a class index from 0 to numClass-1, 
    //      or -1 for no decision     
    classifyDiscreteResult classifyDiscrete(const FeatureVectorViewType &fv) const;

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
    
    //set classifier model
    bool setClassifierModel(HistogramClassifier1DModel &model);    

    //get the classifier model (will return the constant reference)
    HistogramClassifier1DModel & getClassifierModel(void); 

private:
    HistogramClassifier1DModel *histModel;

};


}   // namespace qcv

#endif // qcv_histogramclassifier_h_
//




