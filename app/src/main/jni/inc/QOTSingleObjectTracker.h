/**=============================================================================

@file
QOT.h

@brief
Qualcomm Object Tracker (QOT)

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

#ifndef qcv_qot_h_
#define qcv_qot_h_


#include <cstddef>
#include <stdexcept>
#include <vector>

#include "qcvTypes.h"
#include "qotDefs.h"
#include "qotTypes.h"
#include "OFFBRobustTracker.h"
//#include "Tracker/OFFBTracker.h"
#include "qotObjectDetection.h"
#include "qotObjectVerification.h"
#include "qotObjectReID.h"
#include "qotObjectLearning.h"
#include "KCFTracker.h"

namespace qcv
{
    namespace qot
    {

        //the QOT object tracker, which include OF Tracking, object detector, appearance verifier, and learning 
        //each object tracker tracks only one object, which possesses exclusively tracker, 
		class QOTSingleObjectTracker
		{

		public:
			QOTSingleObjectTracker(ImageExtension &img,
				float32_t					roiX,
				float32_t					roiY,
				float32_t					roiWidth,
				float32_t					roiHeight,
				float32_t					roiWidthZoomFactor,
				float32_t					roiHeightZoomFactor,
				uint32_t ID); //initiliaze and register object

			~QOTSingleObjectTracker();

			//track the object 
			QOTResult trackObject(ImageExtension &prevImg, ImageExtension &currImg);

			//init flag
			bool hasInit(void) const { return initTracker; }

			//set whether to smooth the output
			void setSmoothingFlag(const bool flag) { smoothBBFlag = flag; }
			bool getSmoothingFlag(void) const { return smoothBBFlag; }

			uint32_t getID(void) const { return trackerID; }

			//temporary functions for intermediate results
			QOTResult getMotResult() const { return motResult; }
			QOTResult getKcfResult() const { return kcfResult; }
			vector<DetectionScaleResult> getDetResults() const { return detResults; }
			//vector<DetectionScaleResult> getDetResultVerified() const { return detResultVerified; }
            vector<QOTResult> getDetResultVerified() const { return detResultNCCReIDVerified; }
			void getVerifierNCCImages(uint8_t *posImg, uint8_t *negImg, uint32_t width, uint32_t height, uint32_t stride, uint32_t &numPosTemplates, uint32_t &numNegTemplates) const;

			//getState 
			QOTObjectState getObjState(void) const {return objState;}

            //internal functiona and membmers 
        private: 
            //register the object
            void registerObject(ImageExtension &img, 
                float32_t					roiX,
                float32_t					roiY,
                float32_t					roiWidth,
				float32_t					roiHeight,
				float32_t					roiWidthZoomFactor,
				float32_t					roiHeightZoomFactor);


            //verification of detection results using NCC and ReID
            bool detVerificationNCCReID(ImageExtension &currImg); 

            void trackingFusion(const QOTResult &motResult, const QOTResult &kcfResult, const vector<DetectionScaleResult> & detResult, const vector<DetectionResult> &detReIDPos,
                QOTObjectState &objSt);

            void trackingFusion(const QOTResult &motResult, const QOTResult &kcfResult, const vector< QOTResult>  &detVerified, QOTObjectState &objSt);

            //return up to two clusters
            vector<DetectionResult> clusterDetResults(const vector<DetectionScaleResult> &detResult);
            vector<DetectionResult> clusterDetResults(const vector<DetectionResult> &detResult);            
            std::vector<QOTResult>  clusterDetResults(const std::vector<QOTResult> &detResult);

            //check the ospect ratio of the bounding box 
            void checkAspectRatio(Rect<float32_t> &rect, float32_t objAspectRatio, uint32_t imgWidth, uint32_t imgHeight);

            void smoothTracking(const Rect<float32_t> &currBB, Rect<float32_t> &smoothBB, bool currBBVerified, uint32_t imgWidth, uint32_t imgHeight);

            //////////////////////////////////////////////////////////////////////////////
            //inter members 

            QOTObjectState          objState; //the state of the current object being tracked 

            const uint32_t          trackerID;  //the ID for the curent tracker, unique for each tracker

            //OFFBTracker             *motTracker; //motion tracker 
            OFFBRobustTracker       *motTracker; 
            KCFTracker              *kcfTracker; 
            //bool                    kcfTrackerReset; //flag for resetting the KCF tracker - MOVED TO QOTObjectState


            qotObjectDetector       *objDetector; //a weak object detector 
            qotObjectVerifier       *objVerifier; //object signature verification 
            qotObjectReID           *objReID;     //color based 


            qotObjectLearning       *objLearning; //object learning including the decision forest and the signature template

            Size2D<uint32_t>        winSize;    //the size of the object for scanning window

            bool                    initTracker;             //flag for whether the tracker has been initilaized
            bool                    smoothBBFlag;            //flag for whether to smooth the tracked BBs

            //parameters
            //float32_t   thresholdLearnValid;      //Threshold of valid BB to be learned (0.6)
            //thresholds for detection and learning 
            //float32_t   thresholdDetHitsOverlap;   //threshold on accepting consecutive DF detections
            //thread for valid full image variance 
#ifdef USE_VARIANCE_CHECK
            float32_t   thresholdFullImage; 
#endif

            //temporary intermediate results
            QOTResult motResult;
            QOTResult kcfResult;            
            std::vector<DetectionScaleResult> detResults;

            //std::vector<DetectionScaleResult> detResultVerified;
            //std::vector<DetectionResult>      detResultReIDPos;

            std::vector<QOTResult>         detResultNCCReIDVerified;
        };

    }   //namespace qot

}   // namespace qcv

#endif // qcv_qot_h_
//


