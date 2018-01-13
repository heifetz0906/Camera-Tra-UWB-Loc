/**=============================================================================

@file
ImagePatchClassifier.h

@brief
Definition of a base class for the image patch classifiers. 
The class extract features from an image patch, and then classify the patch using 
its classifier. 

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

#ifndef qcv_imagepatchclassifier_h_
#define qcv_imagepatchclassifier_h_

#include <cstddef>

#include "Classifier.h"
#include "qcvImageExtension.h"
#include "qcvTypes.h"

namespace qcv
{

//Base class for an image patch classifier (abstract)
//give an image patch (located on a given image), extract the feature and classify the patch whether the patch contains an object
template<class _LocType = uint32_t>
class ImagePatchClassifier
{
    
public :
    virtual ~ImagePatchClassifier() {}
        
    //public member functions 

    //Given an image patch, extract an image feature, and classify the patch         
    //imgExt contains pointer to the beginning of the image 
    //locXY gives the upper-left conrer of the patch     
    //Output: decision: 0 for negative response, 1 for positive response, -1 for no decision
    virtual classifyDiscreteResult classifyPatch(ImageExtension &imgExt, const Point2D<_LocType> &locXY) = 0;
           
    //set and get patch size for the classifier 
	//this is important since the sliding window will need such information to decide where to stop the loop
    virtual void setPatchSize(const Size2D<_LocType> &patchSize) =0; 
	virtual Size2D<_LocType> getPatchSize(void) const =0; 

};


}   // namespace qcv

#endif // qcv_imagepatchclassifier_h_
//

