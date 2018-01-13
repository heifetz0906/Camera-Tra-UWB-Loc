/**=============================================================================

@file
Quadratric QuadraticMetric.h

@brief
Definition of a (weighted> quadratic metric implemetnation
d(x,y) = (x-y)^T * W * (x-y)

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

#ifndef qcv_quadraticmetric_h_
#define qcv_quadraticmetric_h_


#include <cstddef>

#include "Feature.h"
#include "Metric.h"

namespace qcv
{

    //class for a (weighted> quadratic metric implemetnation
    //d(x, y) = (x - y) ^ T * W * (x - y)
    class QuadraticMetric : public Metric<float32_t>
    {
    public:
        //default has no weight matrix, so feature length is 0
        QuadraticMetric(uint32_t featureLength, const float32_t *weightMatrix); 
        virtual ~QuadraticMetric();

        //measure the distance between two vectors        
        virtual float32_t distance(const FeatureVectorView<float32_t> &fv1, const FeatureVectorView<float32_t> &fv2) const;
        virtual float32_t distanceWithPreCom(const FeatureVectorView<float32_t> &fv, const FeatureVectorView<float32_t> &fvPC, const FeatureVectorView<float32_t> &fvAux);
        virtual void distanceWithPreComBatch(const FeatureVectorView<float32_t> &fv, const float32_t *fvPCBatch, const float32_t *fvAuxBatch, uint32_t numFVInBatch, float32_t *dist);

        //get the directin to sort the distance
        //1 for ascent order, -1 for descent order
        virtual int32_t getSortDirection() const;

        //precompute for a reference vector 
        virtual bool getPrecomputeDim(uint32_t &fvPCLen, uint32_t &auxFVLen);
        virtual bool preComputeFV(const float32_t *fv, float32_t *pcFV, float32_t *auxFV);

    private:
        float32_t *weight; 
        uint32_t  featLen;
        //internal variables
        float32_t *diff;
        float32_t *multiply;


    };

}   // namespace qcv

#endif // qcv_quadraticmetric_h_
//
