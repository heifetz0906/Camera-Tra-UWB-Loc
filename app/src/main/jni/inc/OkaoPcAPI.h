/*--------------------------------------------------------------------*/
/*  Copyright(C) 2008-2009 by OMRON Corporation                       */
/*  All Rights Reserved.                                              */
/*--------------------------------------------------------------------*/
/* 
    Property Estimation Common Library API
*/

#ifndef OKAOPCAPI_H__
#define OKAOPCAPI_H__

#define OKAO_API
#include    "OkaoDef.h"

#ifdef  __cplusplus
extern "C" {
#endif

/**********************************************************/
/* Get Version                                            */
/**********************************************************/
/* Get Property Estimation Common Library API Version */
OKAO_API INT32      OKAO_PC_GetVersion(UINT8 *pbyMajor, UINT8 *pbyMinor);

#ifdef  __cplusplus
}
#endif


#endif /* OKAOPCAPI_H__ */
