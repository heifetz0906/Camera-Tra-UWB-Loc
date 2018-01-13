/*--------------------------------------------------------------------*/
/*  Copyright(C) 2011 by OMRON Corporation                            */
/*  All Rights Reserved.                                              */
/*                                                                    */
/*     This software is the Confidential and Proprietary product      */
/*     of OMRON Corporation. Any unauthorized use, reproduction       */
/*     or transfer of this software is strictly prohibited.           */
/*                                                                    */
/*--------------------------------------------------------------------*/

#ifndef OKAOCTAPI_H__
#define OKAOCTAPI_H__

#define OKAO_API
#include "OkaoDef.h"

#ifndef OKAO_DEF_HCONTOUR
#define OKAO_DEF_HCONTOUR
    typedef void *      HCONTOUR;
#endif /* OKAO_DEF_HCONTOUR */

#ifndef OKAO_DEF_HCTRESULT
#define OKAO_DEF_HCTRESULT
    typedef void *      HCTRESULT;
#endif /* OKAO_DEF_HCTRESULT */

#ifndef OKAO_DEF_HPTRESULT
#define OKAO_DEF_HPTRESULT
    typedef void *      HPTRESULT;
#endif /* OKAO_DEF_HDTRESULT */

/* Face Contour Detection Mode */
#define CT_DETECTION_MODE_DEFAULT             0                   /* Normal Mode */
#define CT_DETECTION_MODE_EYE                 1                   /* Eye Mode    */


enum CT_PT_PARTS_POINT { /* Feature Points from Facial Parts Detection */
    CT_PTPOINT_LEFT_EYE = 0,            /* Left Eye Center    */
    CT_PTPOINT_RIGHT_EYE,               /* Right Eye Center   */
    CT_PTPOINT_MOUTH,                   /* Mouth Center       */
    CT_PTPOINT_LEFT_EYE_IN,             /* Left Eye In        */
    CT_PTPOINT_LEFT_EYE_OUT,            /* Left Eye Out       */
    CT_PTPOINT_RIGHT_EYE_IN,            /* Right Eye In       */
    CT_PTPOINT_RIGHT_EYE_OUT,           /* Right Eye Out      */
    CT_PTPOINT_MOUTH_LEFT,              /* Mouth Left         */
    CT_PTPOINT_MOUTH_RIGHT,             /* Mouth Right        */
    CT_PTPOINT_NOSE_LEFT,               /* Nose Left          */
    CT_PTPOINT_NOSE_RIGHT,              /* Nose Right         */
    CT_PTPOINT_MOUTH_UP,                /* Mouth Up           */
    CT_PTPOINT_KIND_MAX       /* The number of Feature Points */
};

enum CT_POINT {  /* Indices of Face Contour Points */
    CT_POINT_EYE_L_PUPIL = 0,
    CT_POINT_EYE_L_IN,
    CT_POINT_EYE_L_OUT,
    CT_POINT_EYE_L_UP,
    CT_POINT_EYE_L_DOWN,
    CT_POINT_EYE_R_PUPIL,
    CT_POINT_EYE_R_IN,
    CT_POINT_EYE_R_OUT,
    CT_POINT_EYE_R_UP,
    CT_POINT_EYE_R_DOWN,
    CT_POINT_FOREHEAD,
    CT_POINT_NOSE,
    CT_POINT_NOSE_TIP,
    CT_POINT_NOSE_L,
    CT_POINT_NOSE_R,
    CT_POINT_NOSE_L_0,
    CT_POINT_NOSE_R_0,
    CT_POINT_NOSE_L_1,
    CT_POINT_NOSE_R_1,
    CT_POINT_MOUTH_L,
    CT_POINT_MOUTH_R,
    CT_POINT_MOUTH_UP,
    CT_POINT_MOUTH_DOWN,
    CT_POINT_LIP_UP,
    CT_POINT_LIP_DOWN,
    CT_POINT_BROW_L_UP,
    CT_POINT_BROW_L_DOWN,
    CT_POINT_BROW_L_IN,
    CT_POINT_BROW_L_OUT,
    CT_POINT_BROW_R_UP,
    CT_POINT_BROW_R_DOWN,
    CT_POINT_BROW_R_IN,
    CT_POINT_BROW_R_OUT,
    CT_POINT_CHIN,
    CT_POINT_CHIN_L,
    CT_POINT_CHIN_R,
    CT_POINT_EAR_L_DOWN,
    CT_POINT_EAR_R_DOWN,
    CT_POINT_EAR_L_UP,
    CT_POINT_EAR_R_UP,
    CT_POINT_KIND_MAX                   /* Number of Indices */
};

#ifdef  __cplusplus
extern "C" {
#endif

/* Get Version Information */
OKAO_API INT32      OKAO_CT_GetVersion(UINT8 *pucMajor, UINT8 *pucMinor);

/* Create/Delete Face Contour Detection Handle */
OKAO_API HCONTOUR   OKAO_CT_CreateHandle(void);                         /* Create Face Contour Detection Handle */
OKAO_API INT32      OKAO_CT_DeleteHandle(HCONTOUR hCT);                 /* Delete Face Contour Detection Handle */
OKAO_API HCTRESULT  OKAO_CT_CreateResultHandle(void);                   /* Create Result Handle */
OKAO_API INT32      OKAO_CT_DeleteResultHandle(HCTRESULT hCtResult);    /* Delete Result Handle */

/* Set Feature Points */
OKAO_API INT32      OKAO_CT_SetPoint(HCONTOUR hCT, INT32 nPointNum, POINT aptPoint[], INT32 anConfidence[],
                                       INT32 nUpDown, INT32 nLeftRight, INT32 nRoll);
OKAO_API INT32      OKAO_CT_SetPointFromHandle(HCONTOUR hCT, HPTRESULT hPtResult);

/* Execute Face Contour Detection */
OKAO_API INT32      OKAO_CT_DetectContour(HCONTOUR hCT, RAWIMAGE *pImage, INT32 nWidth, INT32 nHeight, HCTRESULT hCtResult);

/* Get Result of Face Contour Detection */
OKAO_API INT32      OKAO_CT_GetResult(HCTRESULT hCtResult, INT32 nPointNum, POINT aptCtPoint[]);

/* Set/Get Face Contour Detection Mode */
OKAO_API INT32      OKAO_CT_SetDetectionMode(HCONTOUR hCT, INT32 nMode);     /* Set Mode */
OKAO_API INT32      OKAO_CT_GetDetectionMode(HCONTOUR hCT, INT32 *pnMode);   /* Get Mode */

#ifdef  __cplusplus
}
#endif

#endif    /* OKAOCTAPI_H__ */
