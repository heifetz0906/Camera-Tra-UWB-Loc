/**=============================================================================

@file
NearestNeighborModel.h

@brief
Definition of models for nearest neighbor classifiers.

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

#ifndef qcv_nearest_neighbor_model_h_
#define qcv_nearest_neighbor_model_h_

#include <cstdlib>
#include <cstddef>
#include <stdexcept>
#include <cassert>
#include <memory>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <cfloat>

#include "Metric.h"
#include "qcvTypes.h"
#include "Feature.h"
#include "qcvMemory.h"

#include "ClassSample.h"

namespace qcv
{   
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Nearest Neighbor classifier model 
    //TODO: remove class by ID, this will need to add a flag to indicate which slot is being currenlty used 
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class _DataType>
    class NearestNeighborModel
    {
    //variables
    public:
        uint32_t numClass;
        uint32_t reserveClass;
        uint32_t incClass; //the number of classes memory increase each time
        uint32_t reserveSamplePerClass; //the number of sample to initialize with 
        uint32_t maxSamplePerClass; //the hard limit of maximum samples allowed each class 
        uint32_t featLen;        
        //for precomputation 
        bool     preCompute; //the flag for pre-computation in feature metric
        uint32_t featPCLen;
        uint32_t featAuxLen;
        _DataType *featPCBuffer;
        _DataType *featAuxBuffer;
        

        //////////////////////////////////////////////////////////////////////////////////////
        //Nearest Neighbor Class Models 
        LabeledSampleEnsemble<_DataType>  **classModels;

        //metric 
        Metric<_DataType> *distanceMetric;
        
        //metric threshold, set the minimum distance that can be accepted 
        float32_t distTh;

        //model threshold
        float32_t confTh;

        //qcvMemory scratchBuffer;


    public:
        NearestNeighborModel(uint32_t featureLength, uint32_t reserveNumClass, uint32_t reserveNumSamplePerClass, uint32_t maxNumSamplePerClass, Metric<_DataType> &distanceMeasure, 
                             bool metricPreCompute = false, float32_t distanceThreshold = FLT_MAX, float32_t classScoreThreshold = 0) :
        numClass(0),            
        featLen(featureLength),
        featPCLen(0),
        featAuxLen(0),
        featPCBuffer(NULL),
        featAuxBuffer(NULL),
        reserveSamplePerClass(reserveNumSamplePerClass),
        maxSamplePerClass(maxNumSamplePerClass),
        distanceMetric(&distanceMeasure),        
        distTh(distanceThreshold),
        confTh(classScoreThreshold),
        classModels(NULL), 
        reserveClass(reserveNumClass),
        incClass(reserveNumClass)
        {         
            classModels = (LabeledSampleEnsemble<_DataType> **) MEMALLOC(reserveClass * sizeof(LabeledSampleEnsemble<_DataType> *), 16);       
            //precompute 
            preCompute = (metricPreCompute && distanceMetric->getPrecomputeDim(featPCLen, featAuxLen));            
            if (preCompute)
            {
                featPCBuffer = (_DataType *)MEMALLOC(featPCLen * sizeof(_DataType), 16);
                featAuxBuffer = (_DataType *)MEMALLOC(featAuxLen * sizeof(_DataType), 16);
            }

        }


        NearestNeighborModel(const NearestNeighborModel<_DataType> &other)
        {
            *this = other;
        }
        
        ~NearestNeighborModel(void) 
        { 
            if (classModels)
            {
                for (uint32_t i = 0; i < numClass; i++)
                    delete classModels[i];
                MEMFREE(classModels);
            }        
            if (featPCBuffer)
                MEMFREE(featPCBuffer);
            if (featAuxBuffer)
                MEMFREE(featAuxBuffer);            
        }

        void increaseReserveClass()
        {            
            //increase the buffer
            reserveClass += incClass;
            LabeledSampleEnsemble<_DataType> ** temp = (LabeledSampleEnsemble<_DataType> **) MEMALLOC(reserveClass * sizeof(LabeledSampleEnsemble<_DataType> *), 16);
            memcpy(temp, classModels, numClass * sizeof(LabeledSampleEnsemble<_DataType> *));
            MEMFREE(classModels);
            classModels = temp;
        }

        uint32_t getNumClass() const { return numClass; }
        uint32_t getFeatureLength() const { return featLen; }

        //return the number of samples in the given classID
        uint32_t getNumClassSampleByClassID(uint32_t classID) const
        {
            int32_t idx = getClassIndex(classID);
            if (idx == -1)
                return 0;
            else
                return classModels[idx]->getNumSamples();
            
        }

        uint32_t getNumClassSampleByIndex(uint32_t classIndex) const
        {
            if (classIndex >= numClass)
                return 0;
            else
                return classModels[classIndex]->getNumSamples();
        }
        
        //if the found the classID, return its index, otherwise, return -1 
        int32_t getClassIndex(uint32_t classID) const
        {
            for (uint32_t i = 0; i < numClass; i++)
            {
                if (classModels[i]->getClassID() == classID)
                    return (int32_t)i;
            }
            return -1;
        }

        //if the index is out of range, then return -1
        int32_t getClassID(uint32_t index) const
        {
            if (index >= numClass)
                return -1;
            else
                return classModels[index]->getClassID();
        }

        //only reset the number of samples in each class model, but keep each class model
        void resetClassModel()
        {
            for (uint32_t i = 0; i < numClass; i++)
            {
                classModels[i]->reset();
            }

        }

        //reset the entire model by removing all class models
        void resetFull()
        {
            for (uint32_t i = 0; i < numClass; i++)
                delete classModels[i];
            numClass = 0;
        }

        bool addSample(const _DataType *sample, uint32_t classID)
        {
            
            int32_t classIdx = getClassIndex(classID);
            if (classIdx == -1)//need to add a new sample class
            {
                //Check the maximum number of classes 
                if (numClass == reserveClass)
                {
                    //increase the number of max classes
                    increaseReserveClass();
                }
                //for precompute, use different feature dimension for added samples 
                if (preCompute)
                    classModels[numClass] = new LabeledSampleEnsemble<_DataType>(classID, featPCLen, featAuxLen, reserveSamplePerClass, maxSamplePerClass);
                else 
                    //no auxFeat
                    classModels[numClass] = new LabeledSampleEnsemble<_DataType>(classID, featLen, 0, reserveSamplePerClass, maxSamplePerClass);
                classIdx = numClass;
                numClass++;
            }
            
            bool success;
            if (preCompute)
            {
                //pre-compute the metric, and only add the pre-computed vectors and auxilary feature                
                distanceMetric->preComputeFV(sample, featPCBuffer, featAuxBuffer);
                success = classModels[classIdx]->addSample(featPCBuffer, featAuxBuffer);
            }
            else
                success = classModels[classIdx]->addSample(sample, NULL);

            return success;
        }

        //return total samples added 
        uint32_t addSample(const NearestNeighborModel<_DataType> &other)
        {                   
            assert(featLen == other.featLen && preCompute == other.preCompute && featPCLen==other.featPCLen && featAuxLen==other.featAuxLen);
            if (!(featLen == other.featLen && preCompute == other.preCompute && featPCLen == other.featPCLen && featAuxLen == other.featAuxLen))
                return 0;

            uint32_t totalSamples = 0;

            for (uint32_t i = 0; i < other.numClass; i++)
            {
                if (other.getNumClassSampleByIndex(i) == 0)
                    continue;

                //look for corresponding class
                int32_t classIdx = getClassIndex(other.getClassID(i));

                if (classIdx == -1)//need to add a new sample class
                {
                    //Check the maximum number of classes 
                    if (numClass == reserveClass)
                    {
                        increaseReserveClass();
                    }
                    //for precompute, use different feature dimension for added samples 
                    if (preCompute)
                        classModels[numClass] = new LabeledSampleEnsemble<_DataType>(other.getClassID(i), featPCLen, featAuxLen, reserveSamplePerClass, maxSamplePerClass);
                    else
                        //no auxFeat
                        classModels[numClass] = new LabeledSampleEnsemble<_DataType>(other.getClassID(i), featLen, 0, reserveSamplePerClass, maxSamplePerClass);
                    classIdx = numClass;
                    numClass++;
                }
                totalSamples += classModels[classIdx]->addSample(*(other.classModels[i]));
            }
            return totalSamples;
        }

                
        void setDistanceThreshold(float32_t th)
        {
            distTh = th;
        }
        void setClassScoreThreshold(float32_t th)
        {
            confTh = th;
        }

        //functions to copy or clone the model 
        NearestNeighborModel<_DataType> & operator= (const NearestNeighborModel<_DataType> &other)
        {
            //first remove all extra data buffers 
            if (numClass > other.numClass)
            {
                for (uint32_t i = other.numClass; i < numClass; i++)
                    delete classModels[i];
                numClass = other.numClass;
            }
            
            //check reserveClass length
            if (other.reserveClass != reserveClass)
            {
                reserveClass = other.reserveClass;
                LabeledSampleEnsemble<_DataType> ** temp = (LabeledSampleEnsemble<_DataType> **) MEMALLOC(reserveClass * sizeof(LabeledSampleEnsemble<_DataType> *), 16);
                memcpy(temp, classModels, numClass * sizeof(LabeledSampleEnsemble<_DataType> *));
                MEMFREE(classModels);
                classModels = temp;
            }

            //copy all class models and data buffers 
            for (uint32_t i = 0; i < other.numClass; i++)
            {
                if (i < numClass)
                {
                    *(classModels[i]) = *(other.classModels[i]);
                }
                else
                {
                    //allocate new class models 
                    classModels[i] = new LabeledSampleEnsemble<_DataType>(*(other.classModels[i]));
                }                
            }
            //copy all variables 
            numClass = other.numClass;
            reserveClass = other.reserveClass;
            incClass = other.incClass;
            reserveSamplePerClass = other.reserveSamplePerClass;
            maxSamplePerClass = other.maxSamplePerClass;
            featLen = other.featLen;
            //precomputation
            if (other.preCompute)
            {
                MEMFREE(featPCBuffer);
                MEMFREE(featAuxBuffer);
                featPCBuffer = (_DataType *)MEMALLOC(other.featPCLen * sizeof(_DataType), 16);
                featAuxBuffer = (_DataType *)MEMALLOC(other.featAuxLen * sizeof(_DataType), 16);
            }
            preCompute = other.preCompute;
            featPCLen = other.featPCLen;
            featAuxLen = other.featAuxLen;            
            //metric 
            distanceMetric = other.distanceMetric;
            //metric threshold 
            distTh = other.distTh;
            //model threshold
            confTh = other.confTh;

            return *this;
        }

        NearestNeighborModel<_DataType> * clone() const
        {
            NearestNeighborModel<_DataType> * model = new NearestNeighborModel<_DataType>(*this);
            return model;
        }


        void write(const std::string &filename)
        {
#ifndef ANDROID
            for (uint32_t i = 0; i < numClass; i++)
            {
                //save positive templates
                std::string outName = filename + "_classID_" + std::to_string(classModels[i]->getClassID()) + ".txt";
                std::ofstream outfile;
                outfile.open(outName.c_str(), std::ios::out);
                classModels[i]->write(outfile);
                outfile.close();
            }
#endif
        }

        //evalute get the kNN measurements from a given class
        //for kNN = 0, return the measurement to all samples (sorted)
        //for kNN > 0, return the sorted largest kNN values
        //tatalMeasure - the actually number of measurement values returned. 
        bool getKNearestNeighborMeasureByClassID(const FeatureVectorView<_DataType> &fv, uint32_t classID, uint32_t kNN, float32_t *measure, uint32_t *totalMeasure)
        {
            int32_t classIdx = getClassIndex(classID);
            if (classIdx == -1)
            {
                *totalMeasure = 0;
                return false;
            }                                        
            return getKNearestNeighborMeasureByIndex(fv, classIdx, kNN, measure, totalMeasure);
        }



        //evalute get the kNN measurements from a given class
        //for kNN = 0, return the measurement to all samples (sorted)
        //for kNN > 0, return the sorted largest kNN values        
        //tatalMeasure - the actually number of measurement values returned. 
        bool getKNearestNeighborMeasureByIndex(const FeatureVectorView<_DataType> &fv, uint32_t classIndex, uint32_t kNN, float32_t *measure, uint32_t *totalMeasure) const
        {
            assert(classIndex < numClass);
            if (classIndex >= numClass)
            {
                *totalMeasure = 0;
                return false;
            }

            uint32_t nSample = classModels[classIndex]->getNumSamples();
            if (nSample == 0)
            {
                *totalMeasure = 0;
                return false; 
            }             

            ValueIndexPair<float32_t> *dist = (ValueIndexPair<float32_t> *)MEMALLOC(nSample * sizeof(ValueIndexPair<float32_t>), 16);
            if (preCompute)
            {
                //batch comptue all distances
                float32_t *distances = (float32_t *)MEMALLOC(nSample * sizeof(float32_t), 16);
                distanceMetric->distanceWithPreComBatch(fv, classModels[classIndex]->getSampleData(), classModels[classIndex]->getAuxData(),nSample, distances);
                for (uint32_t i = 0; i < nSample; i++)
                {
                    dist[i].value = distances[i];
                    dist[i].index = i;
                }
                MEMFREE(distances);
            }
            else
            {
                for (uint32_t i = 0; i < nSample; i++)
                {
                    _DataType *sample = classModels[classIndex]->getSampleData(i);
                    FeatureVectorView<_DataType> sampleView(sample, featLen);
                    dist[i].value = distanceMetric->distance(fv, sampleView);
                    dist[i].index = i;
                }
            }
            

            //sort 
            int32_t direction = distanceMetric->getSortDirection();
            if (direction == 1)
            {
                //direction = 1;
                std::qsort(dist, nSample, sizeof(ValueIndexPair<float32_t>), qsortValueIndexPair<float32_t, 1>);
            }
            else if (direction == -1)
            {
                //direction = -1;
                std::qsort(dist, nSample, sizeof(ValueIndexPair<float32_t>), qsortValueIndexPair<float32_t, -1>);
            }                
            else
            {
                *totalMeasure = 0;
                return false;
            }

            if (kNN == 0)
                *totalMeasure = nSample;
            else
                *totalMeasure = MIN(kNN, nSample);

            for (uint32_t i = 0; i < *totalMeasure; i++)
            {
                if (direction * dist[i].value > direction * distTh)
                {
                    *totalMeasure = i;
                    break;
                }
                measure[i] = dist[i].value;
            }             

            MEMFREE(dist);
            
            return (*totalMeasure > 0);                           
        }
            
    };

} //namespace qcvlib

#endif  //qcv_nearest_neighbor_model_h_

