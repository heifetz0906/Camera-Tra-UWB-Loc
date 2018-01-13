/**=============================================================================

@file
Feature.h

@brief
Definition of Feature class, which will be a base class for different kind of features. 
Written in C++ 

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

#ifndef qcv_feature_h_
#define qcv_feature_h_


#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <memory.h>
#include <iostream>
#include <fstream>
#include <string>

#include "qcvTypes.h"


namespace qcv
{

//feature vector view only doesn't maintain the data
template<class _DataType> 
class FeatureVectorView
{
public:

    FeatureVectorView( ) :
        fv(NULL),
        length(0)
    {     }
    
    FeatureVectorView(_DataType *featureVector, uint32_t featureLength) :
        fv(featureVector),
        length(featureLength)
    {

    }
    
    //public functions 
    uint32_t getLength(void) const {return length;}

    _DataType & operator[] (uint32_t index) 
    {
        assert(index<length);
        return fv[index];
    }

    const _DataType & operator[] (uint32_t index) const
    {
        assert(index<length);
        return fv[index];
    }

    _DataType *getData(void) const {return fv;}

    void setData(_DataType *featureVector, uint32_t featureLength)
    {
        fv = featureVector;
        length = featureLength;
    }
       
    void write(std::ofstream &outfile) const
    {                    
#ifdef WIN32
        for (uint32_t i=0; i<length; i++)
            outfile << std::to_string((_DataType)fv[i]) << " \t"; 
        outfile << "\n";
#else
        char fvString[100];
        for (uint32_t i=0; i<length; i++)
        {
            snprintf(fvString, 10, "%10.8f\t", (_DataType)fv[i]);
            outfile << fvString << " ";
        }
        outfile << "\n";
#endif
    }
    
    void write(const char *filename) const
    {
        std::ofstream outfile;
        outfile.open(filename);
        if (outfile.is_open())
            write(outfile);
        outfile.close();
    }

private:
    _DataType *fv;
    uint32_t length;
};


//feature vector contains and manage the data
template<class _DataType> 
class FeatureVector
{
public:
    
    FeatureVector( )
    {
        fv.clear();
    }

    FeatureVector(uint32_t featureLength)
    {
        fv.resize(featureLength);
    }

    FeatureVector(const _DataType *featureVector, uint32_t featureLength)
    {
        fv.resize(featureLength);
        memcpy(&(fv[0]), featureVector, featureLength*sizeof(_DataType));
    }

    FeatureVector(const std::vector<_DataType> &featureVector)
    {
        fv = featureVector;
    }
    
    //public functions     
    uint32_t getLength(void) const {return fv.size();}
    void setLength(uint32_t length) 
    {
        if (length!=fv.size())
            fv.resize(length);
    }

    void setData(const _DataType *featureVector, uint32_t featureLength)
    {
        fv.resize(featureLength);
        memcpy(&(fv[0]), featureVector, featureLength*sizeof(_DataType));
    }

    void setData(const std::vector<_DataType> &featureVector)
    {
        fv = featureVector;
    }      

    FeatureVectorView<_DataType> getView(void)  
    {
        FeatureVectorView<_DataType> fvv(fv.data(), fv.size());
        return fvv;
    }
    
private:
    std::vector<_DataType> fv;
};


}   // namespace qcv

#endif // qcv_classifier_h_
//

