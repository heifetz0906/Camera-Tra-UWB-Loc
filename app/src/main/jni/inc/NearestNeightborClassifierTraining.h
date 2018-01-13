/**=============================================================================

@file
NearestNeighborClassifier.h

@brief
Definition of a training class for nearest neighbor classifier.

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

#ifndef qcv_nearest_neighbor_classifier_training_h_
#define qcv_nearest_neighbor_classifier_training_h_

#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cassert>
#include <memory>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <cfloat>

#include "qcvMath.h"
#include "qcvTypes.h"
#include "ClassifierTraining.h"
#include "qcvMemory.h"
#include "NearestNeightborModel.h"

namespace qcv
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //Nearest Neighbor Classifier 
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class _DataType>
    class NearestNeighborClassifierTraining : public ClassifierTraining <_DataType, NearestNeighborModel<_DataType> >
    {        
    public:
        //the training class always own its own model 
        NearestNeighborClassifierTraining(uint32_t featureLength, uint32_t reserveNumClass, uint32_t reserveNumSamplePerClass, uint32_t maxNumSamplePerClass, 
            uint32_t reserveNumSamplePerClassIncModel, uint32_t maxNumSamplePerClassIncModel, Metric<_DataType> &distanceMeasure, bool distPreCompute, 
            float32_t distanceThreshold = FLT_MAX, float32_t classScoreThreshold = 0) :
            nnModel(featureLength, reserveNumClass, reserveNumSamplePerClass, maxNumSamplePerClass, distanceMeasure, distPreCompute, distanceThreshold, classScoreThreshold),
            nnModelInc(featureLength, reserveNumClass, reserveNumSamplePerClassIncModel, maxNumSamplePerClassIncModel, distanceMeasure, distPreCompute, distanceThreshold, classScoreThreshold)
        {

        }

        ~NearestNeighborClassifierTraining()
        {

        }

        //public member functions
        uint32_t getNumClass() const { return nnModel.getNumClass(); }
        uint32_t getFeatureLength() const { return nnModel.getFeatureLength(); }


        //adding training sampels only (use classID in sample)
        //each implmented class will decide how to accumulate the samples, but should not update the model 
        //until the update() function is called (in consideration of async learning using multi-threading)    
        //return false if the sample is not added
        bool addSample(TrainingSample<float32_t> &sample)
        {
            assert(sample.featureView.getLength() == getFeatureLength());
            return nnModelInc.addSample(sample.featureView.getData(), sample.classIndex);
        }

        //update the classifier model using the added samples, returned the number of samples learned
        uint32_t update()
        {            
            //add the templates from the incremental model to the model 
            uint32_t totalSample = nnModel.addSample(nnModelInc);            
            //reset the incremental 
            nnModelInc.resetClassModel();//only reset number of samples in each class. 
            return totalSample;
        }

        //get the classifier model
        NearestNeighborModel<_DataType> & getClassifierModel(void) { return nnModel; }
        void setClassifierModel(NearestNeighborModel<_DataType>  &model)
        {
            //copy the model
            nnModel = model;
        }

        //reset the classifier model 
        void resetClassifierModel(void)
        {
            //fully reset all the models. 
            nnModel.resetFull();
            nnModelInc.resetFull();
        }

    private:
        //member variables 
        NearestNeighborModel<_DataType> nnModel;
        NearestNeighborModel<_DataType> nnModelInc; //the incretmental model for updates
    };
    

} //namespace qcvlib

#endif  //qcv_nearest_neighbor_classifier_training_h_

