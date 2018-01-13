/**=============================================================================

@file
qotDefs.h

@brief
qot #define 

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

#ifndef qcv_qotdefs_h_
#define qcv_qotdefs_h_


#define QOT_Profiling
#define T2T_DEBUG
#define QOT_DEBUG_VB_PRINT
//#define QOT_DEBUG_VB_WRITE

namespace qcv
{
namespace qot
{

//////////////////////////////////////////////////////////////////////
//some definitions for image frame related information
//////////////////////////////////////////////////////////////////////

#define RANDOM_SEED 4 //100

#define USE_ORIGINAL_SIZE       false
#define DOWN_SCALE_TO_720       true
#define USE_FIXED_DOWNSAMPLE_SIZE false 
#define FIXED_DOWNSCALE_WIDTH    1280
#define FIXED_DOWNSCALE_HEIGHT   720

/////////////////////////////////////
// QOT parameters 
///////////////////////////////////
//min bounding box side for scanning window
#define MIN_BB_SIZE 16
#define USE_SMOOTH_BB  false

////////////////////////////////////
// OF Tracking
////////////////////////////////
#define ENABLE_OF_TRACKER      true //false // 
#define OF_PYRAMID_LEVELS 4
#define MIN_ASPECT_RATIO_CHANGE 0.5 //0.75 //0.8 //must smaller than 1
#define MAX_ASPECT_RATIO_CHANGE 2 //1.3 //1.25 //must greater than 1

/////////////////////////////////////////////////////////
//  For DF Detection
///////////////////////////////////////////////////////
//#define USE_VARIANCE_CHECK
#define USE_SVM_CLASSIFIER

//the parameters are mostly for accuracy, but more intensive computation
/////////////////////////////////////////
// Sliding Window setup
/////////////////////////////////////////////
#define WIN_NUM_OCTAVE  3
#define WIN_NUM_STEPS_PER_OCVTAVE  3 //
#define SHIFT_BB 0.1    //original: 0.12,  0.1 (TLD)
#define MIN_SHIFT_BB  2 //2 (original) //

/////////////////////////////////////////////////
// Object Detector settings 
///////////////////////////////////////////////

#define SLIDING_WIN_BALANCE_BELOW_OCTAVE_INDEX   1 //2 //0 //1 //2 

#define DETECTOR_INCREMENTAL_LEARN false 
#define DETECTOR_LEARN_UPDATE_INTERVAL     5 //1 // 5 // the frequency of update detector learning with samples collected 

/////////// SVM /////////////
#define SVM_MAX_CACHED_SAMPLES          5000 //20000 
#define SVM_TRAIN_PRUNE_SIZE            1000
#define SVM_TRAIN_COST_C                1
#define SVM_TRAIN_COST_POSITIVE         1
#define SVM_TRAIN_TOL                   0.05
#define SVM_TRAIN_MAX_ITERATION_INIT    500
#define SVM_TRAIN_MAX_ITERATION         10 //50 //10 //500 //10 
    
//////INIT Learning
//Warping parameters 
//for initialization
#define WARP_ANGLE_RANGE_INIT    10  //degree
#define WARP_ANGLE_STEP_INIT     5   //degree
#define WARP_SCALE_RANGE_INIT    0.1 //0.1  // in scale factor 
#define WARP_SCALE_STEP_INIT     0.05 // in scale factor 
#define WARP_SHIFT_RANGE_INIT    2   //pixels
#define WARP_SHIFT_STEP_INIT    1 //1   //pixels
//for later
#define WARP_ANGLE_RANGE   5 //5 //10 //5 //10  //degree
#define WARP_ANGLE_STEP    5 //5   //degree
#define WARP_SCALE_RANGE   0.05 //0.1  // in scale factor 
#define WARP_SCALE_STEP    0.05 //0.05 // in scale factor 
#define WARP_SHIFT_RANGE   1 //1 //1 //pixels
#define WARP_SHIFT_STEP    1 //1   //pixels


//BRIEF Feature related 
#define BRIEF_PIXEL_PAIRS_PER_CHANNEL  64 //100 //number of pixel pairs for each channel
// for feature selection in detection 
#define FEATURE_SELECTION_ENABLE    true
#define BRIEF_PIXEL_PAIRS_FOR_DETECTION   128  //144 (good) //  192 // 128  //number of pixel pairs remained for all channels (after feature selection) for detection 

//////For object detection Detection (for accuracy)
//for detection sampling to accelorate speed
//#define DF_SEARCH_ROI_SZ_FACTOR     0.5 //0.25 (good) //enlarge the current BB for ROI search 
//#define DF_DETECT_SKIP_RATIO_ROI_FOR_ACCURACY  2 // 1 //2 // 3 (good)
//#define DF_DETECT_SKIP_RATIO_OUTSIDE_ROI_FOR_ACCURACY 1 // 5 (good)
//full frame scan
#define DF_DETECT_SKIP_RATIO_FULL_FOR_ACCURACY 2 // 1 //2 //3 (good)
//for learning, only used initial learning
#define DF_DETECT_SKIP_RATIO_INIT_LEARNING_FOR_ACCURACY  2 //1 //2 // 5 //3 // for scanning negative examples in initial learning
#define DF_DETECT_SKIP_RATIO_LEARNING_FOR_ACCURACY 4 //2 //1 //2 // 5 //not currently used

    
//search object scale range 
//#define DF_MIN_BB_SEARCH_SCALE_FACTOR_ROI 0.5 //0.5 (good) //the minimum scale to search the tracked object from the scale found last time 
//#define DF_MAX_BB_SEARCH_SCALE_FACTOR_ROI 2.0 //4.0 //4.0 (good)  //the maximum scale to search the tracked object from the scale found last time 
#define DF_MIN_BB_SEARCH_SCALE_FACTOR_FULL 0.5 // 0.5 //0.5 //0.3 (good) //the minimum scale to search for whole scale 
#define DF_MAX_BB_SEARCH_SCALE_FACTOR_FULL 2.0 //2.0 //4.0 //4.0 (good)//the maximum scale to search for whole scale
//learn object scale reange    
#define DF_MIN_BB_LEARN_SCALE_FACTOR 0.5 //0.3 //0.3 (good) //the minimum scale to learn 
#define DF_MAX_BB_LEARN_SCALE_FACTOR 2.0 //3.0 //3.0 (good) //the maximum scale to learn

//for tracking output    
#define NUM_NONVERIFIED_FRAMES_RESET     90 //60// 3000// 300 //150 //15 //the number of non-varified frame to reset the bounding box
//for smoothing output
#define NUM_MISSED_FRAMES_FOR_RESET_SMOOTHING 150 //the number of missed frames for resetting the smoothing regions

//for detection results clustering 
#define CLUSTER_CENTER_RATIO 1.5    //the ratio between confidend of the first cluster and that of the second cluster

//for deteciton without OFBB
#define NUM_DETECTION_TO_COMFIRM    2 //3 // 2 //1 //2 //number of continuous detection to confirm a recovery from no detection 
#define NUM_DETECTION_HITS_HISTORY  4 //5 //number of historical frames to consider a new detection
#define NUM_DETECTION_HITS_INTERVAL_TH 30 //maximum detection hit gap between the current and previous one

//for detection threshold adjustment
#define MAX_DETECTION_POSTERIOR_NO_VERIFY 120 
#define MAX_DETECTION_POSTERIOR 100 //30 (good) //50 (good) 
#define MIN_DETECTION_POSTERIOR 20 //30 //30 (good) //minimum detections each frame before NCC
#define MIN_DETECTION_POSTERIOR_NCC_RATIO 2 //5 //2 (good) //minimum ration between detection using only posterior and after NCC
#define MAX_FRAMES_TOO_FEW_DETECTIONS 30 // 10  //maximum # of frames with too few detections
#define MAX_FRAMES_TOO_MANY_DETECTIONS 30 //10 //maximum # of frames with too many detections
//SVM threshold after each training
#define DETECTION_TH           0.5  //0.9
//values to increase or decrease threhsold 
#define DF_DETECTION_ADJUST_FACTOR_ADDITIVE   0.08 //0.03 //0.1 //
#define DF_DETECTION_ADJUST_FACTOR_SUBTRACTIVE 0.1 //0.05 //0.1 //
#define DF_DETECTION_MIN_TH -0.5 
//#define DF_DETECTION_MIN_PP_TH 0.30

//////for  learning 
#define MINIMUM_SVM_LEARN   false //learning SVM only for the first MAX_FRAME_TO_LEARN frames
#define MAX_FRAME_TO_LEARN  90 //1000 (original) //  maximum number of frames to learn DF
#define MIN_LEARNED_FRAME 10 // minimum number for frames learn for DF to be run for invalid OF BB
#define NUM_FRAMES_FORCE_TO_LEARN   20 //30 //20 (good) //number of frames force to learn at the beginning of the tracking
#define NUM_VERIFIED_FRAME_TO_START_LEARN 1 //5 //5 (original)//number of continuously verified frames to start learning 
#define NUM_LEARN_FRAME_SKIP    1 //5 (original)  number of learning frames to skip

//maximum and minimum size changes from the registered size to learn
#define MIN_SIZE_RATIO_TO_LEARN  0.3 // 0.5  
#define MAX_SIZE_RATIO_TO_LEARN  3.0 //2.0

//////////////////////////////////////////////////////////
// NCC patches settings 
//////////////////////////////////////////////////////////

//maximum number of samples for NCC verification 
#define MAX_SAMPLES_FOR_NCC_VERIFICATION 60 //120 // 100 (TLD)

//Negative examples used to learn NCC model
#define MAX_NEGATIVE_EXAMPLES_FOR_NCC_LEARNING 60 //120 //

#define NCC_PATCH_SIZE 16 
//maximum number of patches can be in the NCC lists
#define NCC_MAX_POSITIVE_PATCHES 200 // 100 (good)//10000 //50
#define NCC_MAX_NEGATIVE_PATCHES 400 // 200 (good) //50000 //50

//for learning 
//#define NCC_POS_VERIFICATION_HIGH_TH    0.80 //0.75 //0.80 (good)
//#define NCC_POS_VERIFICATION_REINIT_KCF_TH   0.75 //0.70 // 0.75 (good)
//#define NCC_POS_VERIFICATION_TH    0.7 //0.65 //0.7 (good)
//#define NCC_POS_VERIFICATION_LOW_TH  0.57 // 0.6 //the threhsold for non-verified object 
//#define NCC_POS_VERIFICATION_MIN_TH  0.35 //the minimum threshold for accepte tracking by NCC (?)
//#define NCC_LEARNING_TH_CHANGE_POS  0.10 //0.05 //0.10 //0.10 (good)  //ncc threshold increase for learning positive samples
//#define NCC_LEARNING_TH_CHANGE_NEG  0.10 // 0.05 //0.10 (good) //ncc threshold increase for learning negatives samples
////for verification 
//#define NCC_MOTION_TH_CHANGE       0.0 //the larger the less verified
#define NCC_POS_VERIFICATION_HIGH_TH    0.78 //0.75 (original)
#define NCC_POS_VERIFICATION_REINIT_KCF_TH   0.70 // 0.75 (good)
#define NCC_POS_VERIFICATION_TH    0.65 //0.7 (good)
#define NCC_POS_VERIFICATION_LOW_TH  0.53 //0.57 // 0.6 //the threhsold for non-verified object 
#define NCC_POS_VERIFICATION_MIN_TH  0.38 // 0.35 (original) //the minimum threshold for accepte tracking by NCC 
#define NCC_POS_VERIFICATION_MIN_SPLIT_TH  0.45 //0.40 (original) the minimum threshold for accepte tracking by NCC (?)
#define NCC_LEARNING_TH_CHANGE_POS  0.10 //0.05 //0.10 //0.10 (good)  //ncc threshold increase for learning positive samples
#define NCC_LEARNING_TH_CHANGE_NEG  0.10 // 0.05 //0.10 (good) //ncc threshold increase for learning negatives samples
//for verification 
#define NCC_MOTION_TH_CHANGE       0.0 //the larger the less verified

//maximum overlap for false posive samples to learn
#define MAX_OVERLAP_FOR_FALSE_POSITIVE  0.2 //0.0 // 0.2 // 0.2 (good)


//threshold for maximum NCC value to learn a negative (false positive sample)
#define NCC_THRESH_MIN_CONF_LEARN_POS      -0.3 // -0.8
#define NCC_THRESH_MAX_CONF_LEARN_NEG      0.15


//Warping parameters for learning positive samples for Verifier
//#define VERIFIER_WARP_ANGLE_RANGE    3 //5  //degree
//#define VERIFIER_WARP_ANGLE_STEP     6 //10   //degree
//#define VERIFIER_WARP_SCALE_RANGE    0 //0.05  // in scale factor ///2   //pixels
//#define VERIFIER_WARP_SCALE_STEP     0.1 // in scale factor ///1   //pixels
//#define VERIFIER_WARP_SHIFT_RANGE    0 // 1 //2   //pixels
//#define VERIFIER_WARP_SHIFT_STEP     2 //4  //pixels


////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
//KCF Tracking parameters
//////////////////////////////////////////////////////////////
#define KCF_TRACKER_CONF_GOOD_TH          0.40  //the threshold for good KCF tracking results 
#define KCF_TRACKER_CONF_MED_TH           0.30  //0.3 //the threshold for mediocre 
#define KCF_TRACKER_CONF_MIN_TH           0.20 //0.22 // 0.20  //the threshold for trusting the KCF tracker in fusion 
#define KCF_TRACKER_CONF_TOTAL_RESET_TH   0.16 //0.14 //0.15 //the threshold for totally killing the KCF Tracker  
#define KCF_TRACKER_CONF_RESET_BB         0.14 //0.16 //0.15 //threshold for resetting the bounding box 
//// KCF model updating and restarting 
#define KCF_TRACKER_CONF_STOP_MODEL_UPDATE 0.23 //0.23 (best)//threshold for stopping KCF model updating 
#define KCF_TRACKER_RESTART_MIN_CONF       0.30 // the minimum confidence score for restarting KCF model due to size change 
// KCF to control learning 
#define KCF_TRACKER_CONF_LEARN_TH         0.35 //0.40 // 0.45  //0.40  //the threshold for triggering detection learning (qotDeetector and qotVerifier)
#define KCF_TRACKER_NUM_FRAME_START_LEARN   20 //10 //5   //the number of frames after init the model to starting the learning 
#define KCF_TRACKER_NUM_FRAME_START_USE   10 //5   //the number of frames after init the model to starting to use
//KCF multiscale search 
#define KCF_TRACKER_NUM_FRAME_PER_MULTISCALE   1 // number of frames per multiscale search 

////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Person ReID parameters
/////////////////////////////////////////////////////////////////////////////
#define MAX_DETECTION_SAMPLES_FOR_REID    50 //100
//Negative examples used to learn RID model
#define MAX_NEGATIVE_EXAMPLES_FOR_REID_LEARNING 25 //120 //
//maximum number of samples for each model
#define MAX_SAMPLES_FOR_EACH_MODEL        200 //0 //100
//ReID thresholds
#define REID_XQDA_DISTANCE_TH   10 //7 //6 //5 
#define REID_CONF_TH         0.58 //0.55 //0.60 //0.65 // 
#define REID_CONF_LEARN_TH   0.63 //0.60 //0.65 //0.70 // 
#define REID_DELETE_TRACK_LOW_CONF_TH  -0.01 //-0.001
#define REID_KCF_REINIT_CONF_TH  0.72 // 0.75 
#define REID_LEARN_DETECTION_HIGH_CONF_TH  0.75 //0.74 //
#define REID_NO_LEARNING_LOW_CONF_TH    -0.02 //-0.05 
#define REID_MIN_CONF_TH                -0.15
#define REID_MIN_LEARN_BB_SIZE      18 //16 //the minimum bounding box size to learn 
#define REID_MIN_BB_SIZE            24 // 18 // the minimum boundding box size for ReID to be considered 

//for deteciton recover
//#define NUM_REID_TO_COMFIRM    2 //1 //2 //number of continuous detection to confirm a recovery from no detection 
//#define NUM_REID_HITS_HISTORY  6 //4 //5 //number of historical frames to consider a new detection
//#define NUM_REID_HITS_INTERVAL_TH 30 //maximum detection hit gap between the current and previous one



///////////////////////////////////////////////////////////////
//Fusion parameters
//////////////////////////////////////////////////////////////
#define FUSION_TH_OVERLAP_TO_MERGE  0.5 //0.9 (original) //the overlap threshold to merge Motion tracking and Detection results
#define FUSION_TH_OVERLAP_TO_SPLIT  0.4 //0.3 //0.2 (original)//0.5  //the overlap threshold to consider DF without merging

#define MAX_SAME_LOC_TO_LEARN       5 //10 //20 //maximum number of frame to learn at the same location 

#define MIN_AREA_WITHIN_IMAGE_TH      0.5 //0.25 //the threshold for the minimum portion of the tracked box inside the image boundary 




}   //namespace qot

}   // namespace qcv

#endif // qcv_qottypes_h_
//


