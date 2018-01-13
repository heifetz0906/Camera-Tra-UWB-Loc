/******************************************************************************/
/*  Copyright(C) 2010 by OMRON Corporation                                    */
/*  All Rights Reserved.                                                      */
/******************************************************************************/
/**
 *  Pet Detection Library API
 */
#ifndef OMCVPDAPI_H__
#define OMCVPDAPI_H__

#define OMCV_API
#include    "CommonDef.h"
#include    "OkaoDef.h"
#include    "DetectionInfo.h"


#ifndef OMCV_DEF_HPET
    #define OMCV_DEF_HPET
    typedef void * HPET;           /* Pet Detection handle  */
    typedef void * HPDRESULT;      /* Pet Detection Result handle */
#endif /* OMCV_DEF_HPET */

/** Disable direction skip */
#define PD_DIRECTION_SKIP_OFF    (-1)

#ifdef  __cplusplus
extern "C" {
#endif

/**********************************************************/
/* Get Version                                            */
/**********************************************************/
/* Get Version */
OMCV_API INT32     OMCV_PD_GetVersion ( UINT8 *pucMajor, UINT8 *pucMinor );

/**********************************************************/
/* Required Memory Calculation                            */
/**********************************************************/
/* Get the Required Memory size */
OMCV_API INT32     OMCV_PD_GetRequiredStillMemSize ( INT32 nWidth, INT32 nHeight,
                                                     INT32 nMinSize, INT32 nMaxDetectionCount,
                                                     RECT rcNonTrackingEdgeMask, INT32 nNonTrackingStep,
                                                     UINT32 * pnBackupMemSize,
                                                     UINT32 * pnMinWorkMemSize,
                                                     UINT32 * pnMaxWorkMemSize );

/* Get the Required Memory size */
OMCV_API INT32     OMCV_PD_GetRequiredMovieMemSize ( INT32 nWidth, INT32 nHeight,
                                                     INT32 nMinSize, INT32 nMaxDetectionCount,
                                                     RECT rcNonTrackingEdgeMask, INT32 nNonTrackingStep, 
                                                     RECT rcTrackingEdgeMask, INT32 nTrackingStep,
                                                     UINT32 * pnBackupMemSize,
                                                     UINT32 * pnMinWorkMemSize,
                                                     UINT32 * pnMaxWorkMemSize );



/**********************************************************/
/* Create/Delete Handle                                   */
/**********************************************************/
/* Create/Delete Pet Detection handle */
OMCV_API HPET      OMCV_PD_CreateHandle ( void );
OMCV_API INT32     OMCV_PD_DeleteHandle ( HPET hPD );

/* Create/Delete/Clear Pet Detection result handle */
OMCV_API HPDRESULT OMCV_PD_CreateResultHandle ( INT32 nMaxDetectionCount );
OMCV_API INT32     OMCV_PD_DeleteResultHandle ( HPDRESULT hPdResult );
OMCV_API INT32     OMCV_PD_ClearResultHandle ( HPDRESULT hPdResult );

/* Get the Max number of Pet to be detected */
OMCV_API INT32     OMCV_PD_GetMaxDetectionCount ( HPDRESULT hPdResult, INT32 *pnMaxDetectionCount );


/**********************************************************/
/* Pet Detection                                   */
/******************************************************** */
/* Execute Pet Detection */
OMCV_API INT32     OMCV_PD_Detect ( HPET hPD, RAWIMAGE *pImage, INT32 nWidth, INT32 nHeight,
                                    HPDRESULT hPdResult );


/**********************************************************/
/* Get Pet Detection Result                        */
/**********************************************************/
/* Get the number of detected pet */
OMCV_API INT32     OMCV_PD_GetResultCount ( HPDRESULT hPdResult, INT32 nObjectType, INT32 *pnCount );

/* Get the detection result for each pet */
OMCV_API INT32     OMCV_PD_GetResultInfo ( HPDRESULT hPdResult, INT32 nObjectType,  
                                           INT32 nIndex, DETECTION_INFO *psDetectionInfo );


/**********************************************************/
/* Set/Get Parameters                                     */
/**********************************************************/
/* Set/Get the Pet Detection Mode */
OMCV_API INT32     OMCV_PD_SetMode ( HPET hPD, INT32 nMode );
OMCV_API INT32     OMCV_PD_GetMode ( HPET hPD, INT32 *pnMode );

/* Set/Get the Pet Detection Type */
OMCV_API INT32     OMCV_PD_SetDetectionObjectType ( HPET hPD, INT32 nObjectType );
OMCV_API INT32     OMCV_PD_GetDetectionObjectType ( HPET hPD, INT32 *pnObjectType );

/* Set/Get the Min and Max Pet Size */
OMCV_API INT32     OMCV_PD_SetSizeRange ( HPET hPD, INT32 nMinSize, INT32 nMaxSize );
OMCV_API INT32     OMCV_PD_GetSizeRange ( HPET hPD, INT32 *pnMinSize, INT32 *pnMaxSize );

/* Set/Get the Direction to be detected */
OMCV_API INT32     OMCV_PD_SetAngle ( HPET hPD, UINT32 nNonTrackingAngle , UINT32 nTrackingAngleExtention );
OMCV_API INT32     OMCV_PD_GetAngle ( HPET hPD, UINT32 *pnNonTrackingAngle, UINT32 * pnTrackingAngleExtention );

/* Set/Get the Mask Area */
OMCV_API INT32     OMCV_PD_SetEdgeMask ( HPET hPD, RECT rcNonTrackingEdgeMask, RECT rcTrackingEdgeMask );
OMCV_API INT32     OMCV_PD_GetEdgeMask ( HPET hPD, RECT * prcNonTrackingEdgeMask, RECT * prcTrackingEdgeMask );

/* Set/Get the Detection Threshold */
OMCV_API INT32     OMCV_PD_SetThreshold ( HPET hPD, INT32 nNonTrackingThreshold, INT32 nTrackingThreshold );
OMCV_API INT32     OMCV_PD_GetThreshold ( HPET hPD, INT32 *pnNonTrackingThreshold, INT32 *pnTrackingThreshold );

/* Set/Get the Detection Search Density coefficient for detection */
OMCV_API INT32     OMCV_PD_SetStep ( HPET hPD, INT32 nNonTrackingStep, INT32 nTrackingStep );
OMCV_API INT32     OMCV_PD_GetStep ( HPET hPD, INT32 *pnNonTrackingStep, INT32 *pnTrackingStep );

/* Set/Get Pet Detection Direction Mask */
OMCV_API INT32     OMCV_PD_SetDirectionMask ( HPET hPD, BOOL bMask );
OMCV_API INT32     OMCV_PD_GetDirectionMask ( HPET hPD, BOOL *pbMask );

/* Set/Get Pet Detection Direction Skip */
OMCV_API INT32     OMCV_PD_SetDirectionSkip ( HPET hPD, INT32 nThresholdLevel );
OMCV_API INT32     OMCV_PD_GetDirectionSkip ( HPET hPD, INT32 *pnThresholdLevel );

/* Set/Get Motion Pet Detection Refresh Count */
OMCV_API INT32     OMCV_PD_SetRefreshCount ( HPET hPD, INT32 nMode, INT32 nRefreshCount );
OMCV_API INT32     OMCV_PD_GetRefreshCount ( HPET hPD, INT32 nMode, INT32 *pnRefreshCount );

/* Set/Get Motion Pet Detection Retry and Hold Count */
OMCV_API INT32     OMCV_PD_SetLostParam ( HPET hPD, INT32 nRetryCount, INT32 nHoldCount );
OMCV_API INT32     OMCV_PD_GetLostParam ( HPET hPD, INT32 *pnRetryCount, INT32 *pnHoldCount );

/* Set/Get Motion Mode Pet Detection Position Correction Parameter */
OMCV_API INT32     OMCV_PD_SetModifyMoveRate ( HPET hPD, INT32 nModifyMoveRate );
OMCV_API INT32     OMCV_PD_GetModifyMoveRate ( HPET hPD, INT32 *pnModifyMoveRate );

/* Get the maximum amount of working memory used for OMCV_PD_Detect() */
OMCV_API INT32     OMCV_PD_SetMaxUseMemSize ( HPET hPD, UINT32 unMaxUseMemSize );
OMCV_API INT32     OMCV_PD_GetMaxUseMemSize ( HPET hPD, UINT32 * punMaxUseMemSize );

/* Set/Get the timeout time for OMCV_PD_Detect() */
OMCV_API INT32     OMCV_PD_SetTimeout ( HPET hPD, INT32 nNonTrackingTimeout, INT32 nTrackingTimeout );
OMCV_API INT32     OMCV_PD_GetTimeout ( HPET hPD, INT32 *pnNonTrackingTimeout, INT32 *pnTrackingTimeout );

#ifdef  __cplusplus
}
#endif

#endif /* OMCVPDAPI_H__ */
