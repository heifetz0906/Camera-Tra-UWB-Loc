/**=============================================================================

@file
ImagePatchClassifierCascade.h

@brief
Definition of a standard cascade classifier  

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

#ifndef qcv_imagepatchclassifier_cascade_h_
#define qcv_imagepatchclassifier_cascade_h_


#include <cstddef>
#include <vector>

#include "qcvTypes.h"
#include "ImagePatchClassifier.h"


namespace qcv
{

//Definition of a standard cascade classifier  
template<class _LocType>
class ImagePatchClassifierCascade: public ImagePatchClassifier <_LocType>
{
    
public :
    ImagePatchClassifierCascade( )          
    { imClassifiers.clear(); }

    ~ImagePatchClassifierCascade() { imClassifiers.clear(); };
        
    //public member functions 

    void addImagePatchClassifier(ImagePatchClassifier<_LocType> &patchClassifier)
    {
        if (!imClassifiers.empty())
        {
            Size2D<_LocType> size, size_new;            
            size = imClassifiers[0]->getPatchSize();
            size_new = patchClassifier.getPatchSize();
            assert(size.equal(size_new));
        }
        imClassifiers.push_back(&patchClassifier);
    }

    //classify the patch    
    //Given an image patch, extract an image feature, and classify the patch         
    //imgExt contains pointer to the beginning of the image 
    //locXY gives the upper-left conrer of the patch     
    //Output: decision: 0 for negative response, 1 for positive response, -1 for no decision
    //the cascade return negatively as soon as one classifier rejects 
    //the confidence value will be set by either the classifier that rejected it, or the last classifier
    classifyDiscreteResult classifyPatch(ImageExtension &imgExt, const Point2D<_LocType> &locXY)
    {
        classifyDiscreteResult output;
        for (uint32_t i=0;i<imClassifiers.size();i++)
        {
            output = imClassifiers[i]->classifyPatch(imgExt, locXY);
            if (output.decision<=0)
            {
                break;
            }
        }
        return output;
    }
    
    void setPatchSize(const Size2D<_LocType> &patchSize) 
    {
        for (uint32_t i=0; i<imClassifiers.size();i++)
            imClassifiers[i]->setPatchSize(patchSize);

    }
    
    Size2D<_LocType> getPatchSize(void) const
    {
        Size2D<_LocType> size;
        if (imClassifiers.empty())
            size.setSize(0,0);
        else 
            size = imClassifiers[0]->getPatchSize();
        return size;
    }

    void clearClassifiers( ) {imClassifiers.clear();}

private:
    std::vector<ImagePatchClassifier<_LocType> *> imClassifiers;
};


}   // namespace qcv

#endif // qcv_imagepatchclassifier_cascade_h_
//

