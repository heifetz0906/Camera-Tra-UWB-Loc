/**=============================================================================

@file
ClassSample.h

@brief
Definition of class samples

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

#ifndef qcv_class_sample_h_
#define qcv_class_sample_h_

#include <cstdlib>
#include <cstddef>
#include <stdexcept>
#include <cassert>
#include <memory>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#include "qcvTypes.h"


namespace qcv
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // Labeled samples for a class 
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class _DataType>
    class LabeledSampleEnsemble
    {

    private:
        uint32_t    classLabel; //can be any unsigned integer number 
        uint32_t    numSamples;
        _DataType   *sampleData;
        uint32_t    featLen;
        _DataType   *auxData; //the auxilary data for associated with each data
        uint32_t    auxLen;   //the length of the auxilary data 
        
        uint32_t    reserveSampleSz; //the sample size to reserve at initialization 
        uint32_t    incSamples; //the number of samples to increase each time when the reserved size is reached 
        uint32_t    maxSampleAllowed; //the hard maximum number of sample limit. if maxSampleAllowed==0, then unlimited sample can be added

    public:
        LabeledSampleEnsemble(uint32_t classID, uint32_t featureLength, uint32_t auxilaryLength, uint32_t reserveNumSample, uint32_t maxNumSampleAllowed) : //=0) : //default unlimited cap
            classLabel(classID),
            numSamples(0),
            featLen(featureLength),
            auxLen(auxilaryLength),
            reserveSampleSz(reserveNumSample),
            maxSampleAllowed(maxNumSampleAllowed),
            incSamples(reserveNumSample)
        { 
            sampleData = (_DataType *)MEMALLOC(reserveSampleSz * featLen * sizeof(_DataType), 16);
            assert(sampleData);
            if (auxLen > 0)
            {
                auxData = (_DataType *)MEMALLOC(reserveSampleSz * auxLen * sizeof(_DataType), 16);
                assert(auxData);
            }
            else
                auxData = NULL;            
        }

        LabeledSampleEnsemble(const LabeledSampleEnsemble<_DataType> &other)             
        {
            *this = other;            
        }

        
        ~LabeledSampleEnsemble() 
        { 
            if (sampleData)
                MEMFREE(sampleData);       
            if (auxData)
                MEMFREE(auxData);
        }
        
        //reset the model
        void reset()
        {
            numSamples = 0;
        }

        bool increaseReserveSample()
        {
            if (maxSampleAllowed > 0 && reserveSampleSz == maxSampleAllowed) //reached the cap
                return false;

            reserveSampleSz += incSamples;
            if (maxSampleAllowed > 0 && reserveSampleSz > maxSampleAllowed)
                reserveSampleSz = maxSampleAllowed;

            _DataType *newSampleData = (_DataType *)MEMALLOC(reserveSampleSz * featLen * sizeof(_DataType), 16);
            assert(newSampleData);
            if (!newSampleData)
                return false;
            _DataType *newAuxData = NULL;
            if (auxLen > 0)
            {
                newAuxData = (_DataType *)MEMALLOC(reserveSampleSz * auxLen * sizeof(_DataType), 16);
                assert(newAuxData);
                if (!newAuxData)
                {
                    MEMFREE(newSampleData);
                    return false;                    
                }                    
            }

            //copy old data to new data 
            memcpy(newSampleData, sampleData, numSamples * featLen * sizeof(_DataType));
            MEMFREE(sampleData);
            sampleData = newSampleData;
            
            if (auxLen > 0)
            {
                //copy auxilary data
                memcpy(newAuxData, auxData, numSamples * auxLen * sizeof(_DataType));
                MEMFREE(auxData);
                auxData = newAuxData;
            }
            return true;
        }

        bool addSample(const _DataType *sample, const _DataType *aux)
        {   
            if (!sample || (auxLen > 0 && !aux) || (auxLen==0 && aux))
                return false;

            if (maxSampleAllowed > 0 && numSamples == maxSampleAllowed) //reached the maximum cap
                return false;

            if (numSamples == reserveSampleSz && !increaseReserveSample())
                return false;
            
            //copy sample
            memcpy(sampleData + numSamples*featLen, sample, featLen * sizeof(_DataType));
            //copy auxilary data
            if (auxLen>0)
                memcpy(auxData + numSamples*auxLen, aux, auxLen * sizeof(_DataType));
            numSamples++;
            return true;
        }
        
        //return number of samples added
        uint32_t addSample(const LabeledSampleEnsemble<_DataType> &other)
        {
            
            if (other.numSamples == 0 || other.featLen!=featLen || other.auxLen!=auxLen)
                return 0;

            uint32_t samplesToAdd = other.numSamples;
            if (numSamples + other.numSamples > reserveSampleSz)
            {
                while (numSamples + other.numSamples > reserveSampleSz)
                {
                    if (!increaseReserveSample())
                        break;
                }
                samplesToAdd = MIN(other.numSamples, reserveSampleSz - numSamples);
            }

            if (samplesToAdd > 0)
            {
                //add all new samples            
                memcpy(sampleData + numSamples*featLen, other.sampleData, samplesToAdd * featLen * sizeof(_DataType));
                //add all new auxilary data
                memcpy(auxData + numSamples*auxLen, other.auxData, samplesToAdd * auxLen * sizeof(_DataType));
                numSamples += samplesToAdd;
                
            }
            return samplesToAdd;
        }

        uint32_t getFeatureLength(void) const { return featLen; }

        //get class label 
        uint32_t getClassID(void) const { return classLabel; }

        //get number of samples
        uint32_t getNumSamples(void) const { return numSamples; }

        //get data values 
        _DataType *getSampleData(uint32_t sampleIdx=0)  
        {             
            assert(sampleIdx < numSamples);
            return sampleData + sampleIdx * featLen;        
        }

        //get aux data
        _DataType *getAuxData(uint32_t sampleIdx=0)
        {
            assert(sampleIdx < numSamples);
            return auxData + sampleIdx * auxLen;
        }

        LabeledSampleEnsemble<_DataType> & operator= (const LabeledSampleEnsemble<_DataType> &other)
        {
            if (other.featLen * other.reserveSampleSz != reserveSampleSz * featLen)
            {   
                MEMFREE(sampleData);
                sampleData = (_DataType *)MEMALLOC(other.reserveSampleSz * other.featLen * sizeof(_DataType), 16);
                MEMFREE(auxData);
                auxData = (_DataType *)MEMALLOC(other.reserveSampleSz * other.auxLen * sizeof(_DataType), 16);
            }

            classLabel = other.classLabel; 
            numSamples = other.numSamples;            
            featLen = other.featLen;
            auxLen = other.auxLen;
            reserveSampleSz = other.reserveSampleSz;
            incSamples = other.incSamples;
            maxSampleAllowed = other.maxSampleAllowed;
            memcpy(sampleData, other.sampleData, reserveSampleSz * featLen * sizeof(_DataType));
            memcpy(auxData, other.auxData, reserveSampleSz * auxLen * sizeof(_DataType));
            return *this;
        }

        LabeledSampleEnsemble<_DataType> * clone() const
        {
            LabeledSampleEnsemble<_DataType> * model = new LabeledSampleEnsemble<_DataType>(*this);
            return model;
        }
        
        //save the template
        void write(std::ofstream &outfile) const
        {
            for (uint32_t sampleIdx = 0; sampleIdx < numSamples; sampleIdx++)
            {
                for (uint32_t i = 0; i < featLen; i++)
                {
#ifndef ANDROID
                    outfile << std::to_string((_DataType)sampleData[sampleIdx*featLen+i]) << "\t";
#else               
                    char fvString[100];
                    snprintf(fvString, 10, "%10.8f", (float32_t)sampleData[sampleIdx*featLen+i]);
                    outfile << fvString << "\t";
#endif
                }
                outfile << "\n";
                for (uint32_t i = 0; i < auxLen; i++)
                {
#ifndef ANDROID
                    outfile << std::to_string((_DataType)auxData[sampleIdx*auxLen + i]) << "\t";
#else               
                    char fvString[100];
                    snprintf(fvString, 10, "%10.8f", (float32_t)auxData[sampleIdx*auxLen + i]);
                    outfile << fvString << "\t";
#endif
                }
                outfile << "\n";
            }            
        }
    };
    
} //namespace qcvlib

#endif  //qcv_class_sample_h_

