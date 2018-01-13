/**=============================================================================

@file
NearestNeighborClassifier.h

@brief
Definition of a nearest neighbor classifier.

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

#ifndef qcv_nearest_neighbor_classifier_h_
#define qcv_nearest_neighbor_classifier_h_

#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cassert>
#include <memory>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#include "qcvMath.h"
#include "Metric.h"
#include "qcvTypes.h"
#include "qcvImage.h"
#include "Classifier.h"
#include "qcvMemory.h"
#include "NearestNeightborModel.h"

namespace qcv
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //Nearest Neighbor Classifier 
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class _DataType>
    class NearestNeighborClassifier : public Classifier <_DataType, NearestNeighborModel<_DataType> >
    {        
    public:
        //default two classifiers 
        NearestNeighborClassifier(NearestNeighborModel<_DataType> &  classifierModel, bool copyModel, bool normalizedScore);

        ~NearestNeighborClassifier();

        //public member functions
        uint32_t getNumClass() const { return nnModel->getNumClass(); }
        uint32_t getFeatureLength() const { return nnModel->getFeatureLength();}

        //descrete classification decision
        //return either a class index from 0 to numClass-1, 
        //      or -1 for no decision 
        classifyDiscreteResult classifyDiscrete(const FeatureVectorView<_DataType> &fv) const;

        //descrete classification decision for a given class index 
        //return either 0 for negative and 1 for positive decision
        //      or -1 for no decision 
        classifyDiscreteResult classifyDiscrete(const FeatureVectorView<_DataType> &fv, const uint32_t classindex) const;

        //continuous classification decision 
        //will return a real number (e.g. posterior) for each class 
        void classifyContinuous(const FeatureVectorView<_DataType> &fv, std::vector<float32_t> & output) const;

        //continuous classification decision 
        //will return a real number (e.g. posterior) for the given class index
        float32_t classifyContinuous(const FeatureVectorView<_DataType> &fv, const uint32_t classindex) const;

        //set classifier model (only refer to the model, no local copy maintained)
        bool setClassifierModel(NearestNeighborModel<_DataType> &model);

        //get the classifier model (will return the constant reference)
        NearestNeighborModel<_DataType> & getClassifierModel(void);

    private:
        //measure the nn score for each class 
        //return false if no matching is made
        bool nnMatching(const FeatureVectorView<_DataType> &fv, float32_t *score, uint32_t *classIndex) const;        
        //nnMatching within each class 
        bool nnClassMatching(const FeatureVectorView<_DataType> &fv, float32_t *classScores) const;

    private:
        //member variables 
        NearestNeighborModel<_DataType> *nnModel;
        //normalization flag, use softmax to normalize the output
        bool                    normalize;

        //qcvMemory           scratchBuffer;
    };


    //////////////////////////////////////////////////////////////////
    // implementation
    ///////////////////////////////////////////////
    template<class _DataType>
    NearestNeighborClassifier<_DataType>::NearestNeighborClassifier(NearestNeighborModel<_DataType> &  classifierModel, bool copyModel, bool normalizedScore)
        : Classifier < _DataType, NearestNeighborModel<_DataType> >(copyModel),
        normalize(normalizedScore)
    {            
        if (copyModel)
            nnModel = new NearestNeighborModel<_DataType>(classifierModel);
        else
            nnModel = &classifierModel;

        //scratchBuffer.setSize(1024 * sizeof(float32_t), 16);
    }


    template<class _DataType>
    NearestNeighborClassifier<_DataType>::
        ~NearestNeighborClassifier()
    {
        if (this->hasModelCopy() && nnModel)
            delete nnModel;
    }


    //descrete classification decision
    //return either a class index from 0 to numClass-1, 
    //      or -1 for no decision 
    template<class _DataType>
    classifyDiscreteResult NearestNeighborClassifier<_DataType>::
        classifyDiscrete(const FeatureVectorView<_DataType> &fv) const
    {
            assert(fv.getLength() == getFeatureLength());

            //get the nn matching score 
            uint32_t bestClassID;
            float32_t conf;
            bool validMatch = nnMatching(fv, &conf, &bestClassID);


            classifyDiscreteResult output;
            if (validMatch && conf > nnModel->confTh)
            {
                output.decision = bestClassID;
                output.confidence = conf - nnModel->confTh;
            }
            else
            {
                output.decision = -1;
                output.confidence = 0;
            }
            return output;
        }


    //descrete classification decision for a given class index (class ID) 
    //return either 0 for negative and 1 for positive decision
    //      or -1 for no decision 
    template<class _DataType>
    classifyDiscreteResult NearestNeighborClassifier<_DataType>::
        classifyDiscrete(const FeatureVectorView<_DataType> &fv, const uint32_t classindex) const
    {        
        assert(fv.getLength() == getFeatureLength());
        //assert(classindex < getNumClass() ); //this line is removed because it is classID not index

        //get the nn matching score 
        uint32_t bestClassID;
        float32_t conf;
        bool validMatch = nnMatching(fv, &conf, &bestClassID);


        classifyDiscreteResult output;
        if (validMatch)
        {

            if (classindex == bestClassID && conf > nnModel->confTh)
            {
                output.decision = 1;
                output.confidence = conf - nnModel->confTh;
            }
            else
            {
                output.decision = 0;
                output.confidence = nnModel->confTh - conf;
            }
        }
        else
        {
            output.decision = -1;
            output.confidence = 0;
        }
        return output;
    }


    //continuous classification decision 
    //will return a real number (e.g. posterior) for each class 
    template<class _DataType>
    void NearestNeighborClassifier<_DataType>::
        classifyContinuous(const FeatureVectorView<_DataType> &fv, std::vector<float32_t> & output) const
    {
        assert(fv.getLength() == getFeatureLength());
        uint32_t numClass = getNumClass();

        output.resize(numClass);       
        bool validMatch = nnClassMatching(fv, output.data());        
    }


    //continuous classification decision 
    //will return a real number (e.g. posterior) for the given class index (class ID)
    template<class _DataType>
    float32_t NearestNeighborClassifier<_DataType>::
        classifyContinuous(const FeatureVectorView<_DataType> &fv, const uint32_t classindex) const
    {
        assert(fv.getLength() == getFeatureLength());
        uint32_t numClass = getNumClass();
        if (numClass == 0)
            return 0;
            
        float32_t *classConf = (float32_t *)MEMALLOC(numClass*sizeof(float32_t), 16);
        float32_t conf; 
        if (nnClassMatching(fv, classConf))
        {
            conf = classConf[nnModel->getClassIndex(classindex)];
        }
        else
            conf = 0;            

        MEMFREE(classConf);
        return conf;
    }


    //set classifier model
    template<class _DataType>
    bool NearestNeighborClassifier<_DataType>::
        setClassifierModel(NearestNeighborModel<_DataType> &model)
    {
        if (this->hasModelCopy())
            *nnModel = model;
        else
            nnModel = &model;
        return this->hasModelCopy();
    }


    //get the classifier model (will return the constant reference)
    template<class _DataType>
    NearestNeighborModel<_DataType> & NearestNeighborClassifier<_DataType>::getClassifierModel(void)
    {
        return *nnModel;
    }


    //matching for all 
    //compute nn match score between the given vector to each of the class
    template<class _DataType>
    bool NearestNeighborClassifier<_DataType>::nnClassMatching(const FeatureVectorView<_DataType> &fv, float32_t *classScores) const
    {
        uint32_t numClass = getNumClass();
        if (numClass == 0)
            return false;

        float32_t sum = 0; //use for normalize the scores
        float32_t dist;
        uint32_t totalmeasure = 0;
        bool validMatch = false;
        for (uint32_t i = 0; i < numClass; i++)
        {
            if (nnModel->getKNearestNeighborMeasureByIndex(fv, i, 1, &dist, &totalmeasure) && totalmeasure > 0)
            {
                classScores[i] = dist;
                validMatch = true;
            }
            else
                classScores[i] = 1e20f;
            sum += exp(-classScores[i]);
        }

        if (!validMatch)
            return false;

        //noramlize the output 
        if (normalize)
        {
            for (uint32_t i = 0; i < numClass; i++)
                classScores[i] = exp(-classScores[i]) / sum;
        }
        return true;
    }
        
    //compute nn match score between the given vector to each of the class, return only the best match 
    template<class _DataType>
    bool NearestNeighborClassifier<_DataType>::nnMatching(const FeatureVectorView<_DataType> &fv, float32_t *score, uint32_t *classID) const
    {        
        uint32_t numClass = getNumClass();
        if (numClass == 0)
            return false;

        float32_t sum = 0; //use for normalize the scores
        float32_t   minDist1 = 1e20f; 
        uint32_t    minIdx1 = 0;
        float32_t   minDist2 = 1e20f;
        uint32_t    minIdx2 = 0;
        float32_t dist;
        uint32_t totalmeasure=0;
        bool validMatch = false;
        for (uint32_t i = 0; i < numClass; i++)
        {
            if (nnModel->getKNearestNeighborMeasureByIndex(fv, i, 1, &dist, &totalmeasure) ) 
            {
                if (dist < minDist1)
                {
                    minDist1 = dist;
                    minIdx1 = i;
                }
                else if (dist < minDist2)
                {
                    minDist2 = dist;
                    minIdx2 = i;
                }                
                sum += exp(-dist);       
                validMatch = true;
            }
        }

        if (!validMatch)
            return false;

        //noramlize the output by softmax
        if (normalize)
        {
            //minDist1 = exp(-minDist1);
            //minDist2 = exp(-minDist2);
            //only comapre the frist largest and the 2nd largest 
            //*score = minDist1 / (minDist1 + minDist2);
            *score = exp(-minDist1) / sum;
        }
        else
        {
            //original distance 
            *score = minDist1; // exp(-minDist1) / sum;
        }
        //return the classID
        *classID = nnModel->getClassID(minIdx1);      

        return true;
    }




} //namespace qcvlib

#endif  //qcv_nearest_neighbor_classifier_h_

