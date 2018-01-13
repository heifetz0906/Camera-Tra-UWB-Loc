/*--------------------------------------------------------------------*/
/*  Copyright(C) 2010 by OMRON Corporation                            */
/*  All Rights Reserved.                                              */
/*--------------------------------------------------------------------*/

#ifndef DETECTIONINFO_H__
#define DETECTIONINFO_H__

#include "OkaoDef.h"


/* Detector Type */
#define DET_SOFT    (0x00000000)        /* Software */
#define DET_IP      (0x00010000)        /* IP       */

#define DET_DT      (0x00000100)        /* Face Detector       */
#define DET_BD      (0x00000200)        /* Human Body Detector */
#define DET_PD      (0x00000300)        /* Pet Detector        */
#define DET_OT      (0x00000400)        /* Object Tracking     */

#define DET_V1      (0x00000010)
#define DET_V2      (0x00000020)
#define DET_V3      (0x00000030)
#define DET_V4      (0x00000040)
#define DET_V5      (0x00000050)
#define DET_V6      (0x00000060)
#define DET_V7      (0x00000070)
#define DET_V8      (0x00000080)
#define DET_V9      (0x00000090)


#define DETECTOR_TYPE_SOFT_DT_V4   (DET_SOFT|DET_DT|DET_V4)  /* Face Detection V4          */
#define DETECTOR_TYPE_SOFT_DT_V5   (DET_SOFT|DET_DT|DET_V5)  /* Face Detection V5          */
#define DETECTOR_TYPE_SOFT_BD_V1   (DET_SOFT|DET_BD|DET_V1)  /* Human Body Detection V1    */
#define DETECTOR_TYPE_SOFT_PD_V1   (DET_SOFT|DET_PD|DET_V1)  /* Pet Detection V1           */
#define DETECTOR_TYPE_SOFT_OT_V1   (DET_SOFT|DET_OT|DET_V1)  /* Object Tracking V1         */

#define DETECTOR_TYPE_DT_IP_V1     (DET_IP|DET_DT|DET_V1)    /* Face Detection IP V1       */
#define DETECTOR_TYPE_DT_IP_V2     (DET_IP|DET_DT|DET_V2)    /* Face Detection IP V2       */
#define DETECTOR_TYPE_BD_IP_V1     (DET_IP|DET_BD|DET_V1)    /* Human Body Detection IP V1 */

#define DETECTOR_TYPE_OTHER        (0x00000000)              /* Third-party */


/* Object Types */
#define OBJ_TYPE_UNKNOWN  (0x00000000)
#define OBJ_TYPE_FACE     (0x00010000)
#define OBJ_TYPE_HUMAN    (0x00020000)
#define OBJ_TYPE_OBJECT   (0x00040000)
#define OBJ_TYPE_DOG      (0x00080000)
#define OBJ_TYPE_CAT      (0x00100000)


/* Pose(Angle of Yaw direction) */
#define POSE_YAW_LF_PROFILE   (-90)     /* Left Profile        */
#define POSE_YAW_LH_PROFILE   (-45)     /* Left Half Profile   */
#define POSE_YAW_FRONT          (0)     /* Front               */
#define POSE_YAW_RH_PROFILE    (45)     /* Right Half Profile  */
#define POSE_YAW_RF_PROFILE    (90)     /* Right Profile       */
#define POSE_YAW_HEAD        (-180)     /* Head                */
#define POSE_YAW_UNKOWN         (0)     /* Unkown              */


typedef struct {
    INT32             nDetectorType;    /* Detector Type   */
    INT32             nObjectType;      /* Object Type     */

  /* Detection Result */
    INT32             nID;              /* ID Number       */
    INT32             nConfidence;      /* Confidence      */
    POINT             ptCenter;         /* Center Position */
    INT32             nWidth;           /* Width           */
    INT32             nHeight;          /* Height          */
    INT32             nAngle;           /* Angle(Roll)     */
    INT32             nReserved[5];
} DETECTION_INFO;

#endif /* DETECTIONINFO_H__ */
