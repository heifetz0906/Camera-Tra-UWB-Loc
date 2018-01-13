/**=============================================================================

@file
qotTypes.h

@brief
Definition of qot common data types

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

#ifndef qcv_qottypes_h_
#define qcv_qottypes_h_


#include <cstddef>
//#include <stdexcept>
#include <vector>

#include "qcvMemory.h"
#include "qcvTypes.h"
#include "ImageDetector.h"
#include "qcvImageExtension.h" 

#include "scveQOTC_api.h"
#include "qotDefs.h"


namespace qcv
{
namespace qot
{

/*==============================================================================
Data Types
===============================================================================*/
class QOTResult
{
public:
    QOTResult() : bbVerified(false),bbReIDPos(false),bbReIDEnable(true),bbVerConf(0),bbReIDConf(0) {  }
    
    void setResult(const DetectionResult &result) {bb =result;}
    void setVerifiedFlag(bool flag) {bbVerified = flag;}

    void clear();    

    bool isValidRect() const ; 

    //bool isValid()const ; 

    void invalidate();

public: 
    DetectionResult      bb;
    bool bbVerified; //whether the confidence is high enough to pass the verification test 
    float32_t bbVerConf; //the confidence of verification 
    bool bbReIDPos; //whether the confidence is high enough for ReID
    float32_t bbReIDConf;  //the confidence for reID
    bool bbReIDEnable; //flag whether ReID is enabled, for small bb ReID is disabled
};



/*==========================================================================
QOT Image Frame Information 
===========================================================================*/
class QOTImageFrameInfo
{
public: 
    QOTImageFrameInfo(uint32_t imW, uint32_t imH, uint32_t imStride, QOTColorFormat colorFormat, 
                      bool fixedDownScaleSize, uint32_t fcvPyramidLevels, uint32_t gauPydOctaves, uint32_t gauPydSteps);
    ~QOTImageFrameInfo();

    //set current Frame 
    void setCurrentFrame(uint8_t* src); 
    //swap the current and previous images
    void swapCurrPrevImages();
    
    //compute the extension images, such as the fcvPyramid, GaussianPyramid, and integral images
    void computeExtentionImages( );

    //restore the tracking cooridiate to the original coordinate 
    Rect<float32_t> restoreOriginalCoord(const Rect<float32_t> &inRect, bool clamp);

private:
    void initialize();
    
public: 
    int32_t                 origImgWidth;        // Original image width
	int32_t                 origImgHeight;       // Original image height
    int32_t                 origImgStride;       //original image stride    
    QOTColorFormat          origImgFormat;       //original image color formate
    
    int32_t                 imgWidth;           //Image width
    int32_t                 imgHeight;          //Image height
    int32_t                 imgStride;          //Image stride

    bool                    dsUseFixedSize;       //flag for using fixed size image (default 320x240), or keep the same aspect ratio
    float32_t               dsFactor[2];          // Downsample factor in x and y
	
    uint32_t                fcvPydLevels;            //FastCV pyramid levels 
    
    uint32_t                gaussPydNumOctaves;     //number of octaves for the Gaussian pyrmid
    uint32_t                gaussPydStepsPerOctave; //number of steps per octave

    ///hold image frame data 
    ImageExtension          *prevFrameExt;
    ImageExtension          *currFrameExt;
    
    uint8_t                 *prevFrameData;
    uint8_t                 *currFrameData;

    bool                    init; //the flag for intialization 

private:
    qcvMemory           scratchBuff;
};


/*==========================================================================
QOT Object States Type
===========================================================================*/

struct QOTObjectState
{
    Size2D<float32_t>    origObjSize;
	Size2D<float32_t>  	 origObjZoomFactor;   //in case the input ROI is too large, we only tracked part of it, we will rescale it back when return

    float32_t           objAspectRatio;      //the aspect of the selected objects
    float32_t           min_bb_aspect_ratio; //min aspect ratio allowed
    float32_t           max_bb_aspect_ratio; //max aspect ratio allowed         

    int32_t             totalFrame;
    int32_t             learnedFrame;       //number of Frames that DF Learned 

    QOTResult           prevQOTBB;    
    QOTResult           currQOTBB;            
    bool                smoothBBFlag;     
    QOTResult           smoothedQOTBB;      //the smoothed BB

    uint32_t            numQOTBBSameLoc;    //number of tracked BB at the same location 

    bool                kcfTrackerReset; //flag for resetting the KCF tracker model 
    bool                kcfTrackerResetPrevBB; //flag for resetting the KCF tracker prevBB 
    bool                kcfTrackerTotalReset; //flag for totally resetting the KCF tracker 
    int32_t             kcfLearnedFrame; 

    Size2D<float32_t>   lastVerifiedSize;   //the scale of the last valid tracking
    
    DetectionResult     prevObjDetBB;       //the previous detection result from object detector + verification 
    int32_t             prevObjDetFrameNum; // the frame number that the previous detectio result    
    
    bool                objDetFullScan;     //flag for scanning the whole image for detection 
    bool                objDetFullScanPrev; //history of whether full scan was on the previous frame 
    uint32_t            numObjDetBB;        // number of bounding boxes returned by the object detector
    uint32_t            numObjVerificationBB; //number of bounding boxes returned by the object verification 

    //int32_t             numInvalidOF;         //number of consecutive frames with invalid OF BB
    //uint32_t             numDFHits;            //Number of consecutive frames with DF detection, it is used to confirm a recovery from OF miss
    uint8_t              numDFHitsHistory[NUM_DETECTION_HITS_HISTORY]; //array to store the detection hits history
    uint8_t              numDFHitsHistoryCurrPos; //the current position to add the hit information
    //int32_t             numDFHits_invalidOF;  //number of consecutive frames with DF detection after the number of invalid OF BBs reach the maximum    
    uint32_t             numMissedFrames;    //number of concecutive missed frames since last tracked position 
    uint32_t             numNonVerifiedFrames; //number of frames that were not verified

    uint32_t             numContinuousVerifiedFrames; //number of continuously verified frames 

    //ReID recover from detection related
    //uint8_t              numReIDHitsHistory[NUM_DETECTION_HITS_HISTORY]; //array to store the detection hits history
    //uint8_t              numReIDHitsHistoryCurrPos; //the current position to add the hit information
    //DetectionResult      prevObjDetReIDBB;       //the previous detection result from object detector + verification 
    //int32_t              prevObjDetReIDFrameNum; //the frame number that the previous detection results get verified on ReID



        
#ifdef QOT_Profiling                    //profile each processing steps
    float32_t QOTProfile;
    float32_t QOTProfileOF;
    float32_t QOTProfileOFNumFrames;
    float32_t QOTProfileKCF;
    float32_t QOTProfileKCFNumFrames;
    float32_t QOTProfileDF;
    float32_t QOTProfileDFNumFrames;
    float32_t QOTProfileNCC;
    float32_t QOTProfileNCCNumFrames;
    float32_t QOTProfileDFLearning;
    float32_t QOTProfileDFLearningNumFrames;
    float32_t QOTProfileReID;
    float32_t QOTProfileReIDNumFrames;
    float32_t QOTProfileReIDNumVerify;
#endif

public: 
    QOTObjectState( float32_t					roiX,
				    float32_t					roiY,
				    float32_t					roiWidth,
				    float32_t					roiHeight, 
                    bool                        smoothFlag);

};

}   //namespace qot

}   // namespace qcv

#endif // qcv_qottypes_h_
//


