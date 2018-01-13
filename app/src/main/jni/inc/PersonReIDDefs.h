/**=============================================================================

@file
PersonReIDDefs.h

@brief
Basic definitions for Person ReID

Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

#ifndef qcv_person_reid_defs_h_
#define qcv_person_reid_defs_h_


namespace qcv
{
namespace person_reid
{


#define XQDA_DIST_TH  6 //6.0 //4.4842 //the maximum distance to for a possible match (about 60% precision)

#define NN_CONFIDENCE_TH 0.65 //0.70  //the mininum confidence for re-ID
#define NN_CONFIDENCE_TH_LEARN 0.70 //the mininum confidence for re-ID NN training


} //namespace person_reid

}   // namespace qcv

#endif // qcv_person_reid_defs_h_
//


