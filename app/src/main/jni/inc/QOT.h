/*========================================================================

*//** @file scveQOT.h

@par DESCRIPTION:      
      Qualcomm object tracker (a.k.a. touch to track tracker), the highest level 
      which wraps the individual trackers 

@par EXTERNALIZED FUNCTIONS:
      See below.

    Copyright (c) 2012 QUALCOMM, Incorporated.  All Rights Reserved.
    QUALCOMM Proprietary. Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.

*//*====================================================================== */

#ifndef SCVEQOT_H_
#define SCVEQOT_H_


#include <cstddef>
#include <stdexcept>
#include <vector>
#include <map>

#include "fastcv.h"
#include "fastcv_.h"

#include "scveQOTC_api.h"

#include "QOTSingleObjectTracker.h"

namespace qcv
{
namespace qot
{


//the highest class of QOT object tracker, which wraps the individual QOTSingleObjectTracker
//the class contains all image frame data, and other memories
//It is directly interfaced with the outside user, e.g. SCVE
//The structure of this class implementation will enable the multi-threading and graph representation 
class QOT
{
public:

    QOT(uint32_t srcWidth, uint32_t srcHeight, uint32_t srcStride, TrackerParamModes qotMode, QOTColorFormat colorFormat); 

    ~QOT();     
   
    void setFrame(uint8_t *__restrict src);

    //register the object, return the index of the registered 
    //object (or object tracker)
    uint32_t registerObject(uint8_t* __restrict	src1, 
					uint32_t					roiX,
					uint32_t					roiY,
					uint32_t					roiWidth,
					uint32_t					roiHeight);

    //unregister the object 
    void unregisterObject(uint32_t objID);

    //track object
    void trackObject(uint8_t* __restrict		src,
		            std::vector<uint32_t>           objIDs,
					uint32_t* __restrict 			roiX,
					uint32_t* __restrict			roiY,
					uint32_t* __restrict			roiWidth,
					uint32_t* __restrict			roiHeight,
					uint32_t* __restrict			confidence);
    
    bool hasInit() const {return frameData.init;}


#ifdef T2T_DEBUG
    //get interdiate results, for debugging     
    void getTrackingResults(uint32_t                        objID,
					        QOTResult                       &motResult, 
                            QOTResult                       &kcfResult,
                            vector<DetectionScaleResult>    &detResults, 
                            vector<QOTResult>         &detResultVerified );    

    void getVerifierNCCImages(uint32_t objID, uint8_t *posImg, uint8_t *negImg, uint32_t width, uint32_t height, uint32_t stride, uint32_t &numPosTemplates, uint32_t &numNegTemplates);

#endif

private: 
    //member variable 
    QOTImageFrameInfo                   frameData; //image frame data (including previous and current image)

    //vector<QOTSingleObjectTracker *>    objectTrackers;
    std::map<uint32_t, QOTSingleObjectTracker *>    objectTrackerMap;
    uint32_t                            trackerCount; //total number of trackers that has been used in the past
    
    TrackerParamModes qotParaMode; 
    
    
};



}   //namespace qot

}   // namespace qcv




#endif //SCVEQOT_H_
