/**=============================================================================

@file
Metric.h

@brief
Definition of an interface class of metric implemetnation, which measuer the distance between 
to vectors. 

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

#ifndef qcv_metric_h_
#define qcv_metric_h_


#include <cstddef>

#include "Feature.h"

namespace qcv
{

    //Base class for metric that measure the distance between two vectors    
    template< class _DataType>
    class Metric
    {
    public:
        virtual ~Metric() { }

        //measure the distance between two vectors        
        virtual float32_t distance(const FeatureVectorView<_DataType> &fv1, const FeatureVectorView<_DataType> &fv2) const = 0;        
        virtual float32_t distanceWithPreCom(const FeatureVectorView<_DataType> &fv, const FeatureVectorView<_DataType> &fvPC, const FeatureVectorView<_DataType> &fvAux) = 0;
        virtual void distanceWithPreComBatch(const FeatureVectorView<_DataType> &fv, const _DataType *fvPCBatch, const _DataType *fvAuxBatch, uint32_t numFVInBatch, float32_t *dist) = 0;

        //get the directin to sort the distance
        //1 for ascent order, -1 for descent order
        virtual int32_t getSortDirection() const = 0;

        //get the precomputation dimension, return false means that the metric doesn't support precomptuation
        virtual bool getPrecomputeDim(uint32_t &fvPCLen, uint32_t &auxFVLen) = 0;
        //precompute for a reference vector 
        virtual bool preComputeFV(const _DataType *fv, _DataType *pcFV, _DataType *auxFV) = 0;


    };

}   // namespace qcv

#endif // qcv_metric_h_
//
