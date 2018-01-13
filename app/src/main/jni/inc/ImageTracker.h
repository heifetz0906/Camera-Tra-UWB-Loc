/**=============================================================================

@file
ImageTracker.h

@brief
Definition of a base class for an image tracker. 
The class provide a bounding box of an image region on a give frame after initialization 

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

#ifndef qcv_imagetracker_h_
#define qcv_imagetracker_h_


#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cstring>

#include "qcvTypes.h"
#include "qcvRect.h"
#include "qcvImage.h"
#include "qcvImageExtension.h"

namespace qcv
{


//Base class (abstract) for an image tracker  
//The class finds candidate locations of an object on an image.
template <class _LocType>   
class ImageTracker
{
    
public :
    virtual ~ImageTracker() { }
        
    //public member functions      
   
    //track image patch (object) at prevBB in prevImg to BB1 in currImg. currBB and confidence are the output.     
    //return true if the tracking is performed, otherwise return false   
    virtual bool track(ImageExtension &prevImg, ImageExtension &currImg, const BoundingBox<_LocType> &prevBB, BoundingBox<_LocType> &currBB, float32_t &confidence) = 0;

    virtual bool track(ImageExtension &prevImg, ImageExtension &currImg, const Rect<_LocType> &prevBB, Rect<_LocType> &currBB, float32_t &confidence) = 0;
};

}   // namespace qcv

#endif // qcv_imagetracker_h_
//


