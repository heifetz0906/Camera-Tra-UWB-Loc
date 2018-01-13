/**=============================================================================

@file
ImagePatchClassifierSimple.h

@brief
Definition of an simple image patch classifier which contains only one feature 
extractor and one classifier. 

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

#ifndef qcv_image_patch_classifier_simple_h_
#define qcv_image_patch_classifier_simple_h_


#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cstring>

#include "qcvTypes.h"
#include "qcvImage.h"
#include "Feature.h"
#include "Classifier.h"
#include "qcvImageExtension.h"
#include "ImageFeatureExtractor.h"
#include "ImagePatchClassifier.h"


namespace qcv 
{

//an simple image patch classifier which contains only one feature extractor and one classifier. 
template< class _DataType, class _ClassifierModel, class _LocType> //= uint32_t>
class ImagePatchClassifierSimple : public ImagePatchClassifier <_LocType >
{
    
public :    
    ImagePatchClassifierSimple( ImageFeatureExtractor<_DataType, _LocType> &featureExtractor, 
            Classifier<_DataType, _ClassifierModel>  &classifier, uint32_t classIndex);

    //~ImagePatchClassifierSimple();
        
    //public member functions 

    //Given an image patch, extract an image feature, and classify the patch         
    //imgExt contains pointer to the beginning of the image 
    //locXY gives the upper-left conrer of the patch     
    //Output: decision: 0 for negative response, 1 for positive response, -1 for no decision
    classifyDiscreteResult classifyPatch(ImageExtension &imgExt, const Point2D<_LocType> &locXY);

    //set and get patch size for the classifier 
    //this is important since the sliding window will need such information to decide where to stop the loop
    
    void setPatchSize(const Size2D<_LocType> &patchSize)
    {
        featExtra.setWindowSize(patchSize);
    }


    Size2D<_LocType> getPatchSize(void) const    
    {
        return featExtra.getWindowSize();
    }

    ////////////////////////////////////
    //own public functions for setting up
    ////////////////////////////////////

    //set image feature extractor
    //void setFeatureExtractor(ImageFeatureExtractor<_DataType> &featureExtractor);    
    
    //set classifier
    //void setClassifier(Classifier<_DataType, _ClassifierModel> &classifier, const uint32_t classIndex)
    //{ classifier_ = classifier ; classIndex_ = classIndex; }
    
    Classifier<_DataType, _ClassifierModel> & getClassifier(void) const 
    {return classifier_;}    
    uint32_t getClassifierIndex(void) const {return classIndex_;}

    //access the extracted feature
    FeatureVectorView<_DataType> getFeatureView(void) {return feature.getView();}    

    //extract feature
    FeatureVectorView<_DataType> extractFeature(ImageExtension &imgExt, const Point2D<_LocType> &locXY);    
    
private:    
    ImageFeatureExtractor<_DataType, _LocType> &featExtra;        
    Classifier<_DataType, _ClassifierModel>  &classifier_;
    uint32_t classIndex_;

    //intenrally managed memory
    FeatureVector<_DataType> feature;    
};



template< class _DataType, class _ClassifierModel, class _LocType>
ImagePatchClassifierSimple<_DataType, _ClassifierModel, _LocType>::ImagePatchClassifierSimple 
    (ImageFeatureExtractor<_DataType, _LocType> &featureExtractor, Classifier<_DataType, _ClassifierModel>  &classifier, uint32_t classIndex) :
    featExtra(featureExtractor),
    classifier_(classifier),
    classIndex_(classIndex)
    
{ 
    //allocate memory for feature data
    feature.setLength(featureExtractor.getFeatureLength());    
}
    
//Given an image patch, extract an image feature, and classify the patch         
//imgExt contains pointer to the beginning of the image 
//locXY gives the upper-left conrer of the patch     
//Output: decision: 0 for negative response, 1 for positive response, -1 for no decision
template< class _DataType, class _ClassifierModel, class _LocType> 
classifyDiscreteResult ImagePatchClassifierSimple<_DataType, _ClassifierModel, _LocType>::
    classifyPatch(ImageExtension &imgExt, const Point2D<_LocType> &locXY)
{
    FeatureVectorView<_DataType> featureView = feature.getView();
    //extract feature
    featExtra.extractImageFeature(imgExt, locXY, featureView);
    //classify
    return classifier_.classifyDiscrete(featureView, classIndex_);    
}


//extract feature
template< class _DataType, class _ClassifierModel, class _LocType> 
FeatureVectorView<_DataType> ImagePatchClassifierSimple<_DataType, _ClassifierModel, _LocType>::
    extractFeature(ImageExtension &imgExt, const Point2D<_LocType> &locXY)
{
    FeatureVectorView<_DataType> featureView = feature.getView();
    //extract feature
    featExtra.extractImageFeature(imgExt, locXY, featureView);
    return featureView;
}


}   // namespace qcv

#endif // qcv_image_patch_classifier_simple_h_
//

