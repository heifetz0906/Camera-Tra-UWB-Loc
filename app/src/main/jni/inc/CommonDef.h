/*--------------------------------------------------------------------*/
/*  Copyright(C) 2003-2010 by OMRON Corporation                       */
/*  All Rights Reserved.                                              */
/*--------------------------------------------------------------------*/

#ifndef COMMONDEF_H__
#define COMMONDEF_H__

/* Error Code for OKAO Vision library */
#define     OKAO_MEMORYSHORTAGE          4      /* Memory Shortage */
#define     OKAO_HALT                    3      /* Halt in Process */
#define     OKAO_NOTIMPLEMENTED          2      /* Not Implemented */
#define     OKAO_TIMEOUT                 1      /* Time Out */
#define     OKAO_NORMAL                  0      /* Normal End */
#define     OKAO_ERR_VARIOUS            -1      /* Unexpected Error */
#define     OKAO_ERR_INITIALIZE         -2      /* Initialize Error */
#define     OKAO_ERR_INVALIDPARAM       -3      /* Invalid Parameter Error */
#define     OKAO_ERR_ALLOCMEMORY        -4      /* Memory Allocation Error */
#define     OKAO_ERR_MODEDIFFERENT      -5      /* Mode Error */
#define     OKAO_ERR_NOALLOC            -6      /* (reserved.) */
#define     OKAO_ERR_NOHANDLE           -7      /* Handle Error */
#define     OKAO_ERR_PROCESSCONDITION   -8      /* Can't process by condition */

/* Error Code for OMRON Computer Vision library */
#define     OMCV_MEMORYSHORTAGE          4      /* Memory Shortage */
#define     OMCV_HALT                    3      /* Halt in Process */
#define     OMCV_NOTIMPLEMENTED          2      /* Not Implemented */
#define     OMCV_TIMEOUT                 1      /* Time Out */
#define     OMCV_NORMAL                  0      /* Normal End */
#define     OMCV_ERR_VARIOUS            -1      /* Unexpected Error */
#define     OMCV_ERR_INITIALIZE         -2      /* Initialize Error */
#define     OMCV_ERR_INVALIDPARAM       -3      /* Invalid Parameter Error */
#define     OMCV_ERR_ALLOCMEMORY        -4      /* Memory Allocation Error */
#define     OMCV_ERR_MODEDIFFERENT      -5      /* Mode Error */
#define     OMCV_ERR_NOALLOC            -6      /* (reserved.) */
#define     OMCV_ERR_NOHANDLE           -7      /* Handle Error */
#define     OMCV_ERR_PROCESSCONDITION   -8      /* Can't process by condition */

/* Halt Status */
#define     HALT_STATUS_OFF  0      /* Halt status OFF */
#define     HALT_STATUS_ON   1      /* Halt status ON  */

#endif  /* COMMONDEF_H__ */

