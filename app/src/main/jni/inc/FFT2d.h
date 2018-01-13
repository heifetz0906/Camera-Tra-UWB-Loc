//
/**=============================================================================

@file
FFT2d.h

@brief
Definition of an FFT 2d float.

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

#ifndef qcv_FFT2d_h_
#define qcv_FFT2d_h_

#include <math.h>

namespace qcv
{
    void bitrv2(int n, int *ip, double *a);
    void bitrv2col(int n1, int n, int *ip, double **a);
    void bitrv2row(int n, int n2, int *ip, double **a);
    void cftbcol(int n1, int n, double **a, double *w);
    void cftbrow(int n, int n2, double **a, double *w);
    void cftfcol(int n1, int n, double **a, double *w);
    void cftfrow(int n, int n2, double **a, double *w);
    void rftbcol(int n1, int n, double **a, int nc, double *c);
    void rftfcol(int n1, int n, double **a, int nc, double *c);
    void dctbsub(int n1, int n2, double **a, int nc, double *c);
    void dctfsub(int n1, int n2, double **a, int nc, double *c);
    void dstbsub(int n1, int n2, double **a, int nc, double *c);
    void dstfsub(int n1, int n2, double **a, int nc, double *c);
    void makewt(int nw, int *ip, double *w);
    void makect(int nc, int *ip, double *c);

    void cdft2d(int n1, int n2, int isgn, double **a, int *ip, double *w);
    void rdft2d(int n1, int n2, int isgn, double **a, int *ip, double *w);
    void ddct2d(int n1, int n2, int isgn, double **a, double **t, int *ip, double *w);
    void ddst2d(int n1, int n2, int isgn, double **a, double **t, int *ip, double *w);




 


}

#endif  //qcv_FFT2d_h_
