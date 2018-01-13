/**=============================================================================

@file
QpSolver.h

@brief
Definition of a quadratic programming solver class. 

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

#ifndef qcv_qpsolver_h_
#define qcv_qpsolver_h_


#include <cstddef>
#include <vector>
#include <list>
#include <cassert>
#include <algorithm>
#include <memory>

#include "qcvTypes.h"
#include "qcvMath.h"
#include "qcvMemory.h"
//#include "Classifier.h"
//#include "ClassifierTraining.h"

using namespace std;

namespace qcv
{

/* ==================================================================
class QpSolver
Descritpion:  
=================================================================== **/
//Declarations
  
template<class _DataType>
class QpSolver
{
public:
    QpSolver(uint32_t max_cached_samples, uint32_t  feat_len, float32_t svm_C, float32_t J, uint32_t pruneSize);

	virtual ~QpSolver();
    
	bool addSample(const TrainingSample<_DataType>& ex);

	void getModel(SVMClassifierModelLinear<_DataType> &svmModel);
	
	// Optimize QP until relative difference between lower and upper bound is below 'tol'
	uint32_t optimize(float32_t tol, uint32_t iter);

    //Re-indexes qp.x to begin with active consrtaints (eg, the support vectors)
	uint32_t prune();
    
private:
	
    //estimate the upper bound
    float32_t estimateUpperBound(uint32_t numSamples);

	// qp_one requires float data. 
	// perform one pass through current set of support vectors
	void optimizeStep();

    //compress sample buffer to move samples to the front
    void compressSampleBuffer();


private:    
    _DataType *b;  //weight for a in the dual form 
    _DataType *w;  //final output of weights for all sequences    
	_DataType *a;  //the weights for each sample x, a>0 means a support vector 
	_DataType *d;  //norm^2 of each x 
	
	float32_t ub; //upper bound
	float32_t lb; //lower bound
	float32_t l;  //loss 
	float32_t c;  //SVM training constant C	
    float32_t Jpos;  //the cost for positive samples
    _DataType bias;

    uint32_t featLength;

    uint32_t max_num_samples;  //maximum cached samples 
    
    uint8_t *nonSV;             //flag for Non-support vectors 	

    uint8_t *isdata;        //flag for data slots, bit-0 for data existance(1) or not(0), 
                             //bit-1 for positive(1)/negative flags(0)

    uint32_t* availableIdx; //indices for availalbe data slots 
    uint32_t numAvailable;  //the number of the availabe data slots

	uint32_t n;      //number of samples saved 

    _DataType** x;
	uint32_t        pruneDataSize; //number free sample slots to have after pruning

    //temprary buffer 
    qcvMemory   scratchBuffer;
    
    
};

    
/* ==================================================================
class QpSolver
Definitions
=================================================================== **/
    
template<class _DataType>
QpSolver<_DataType>::QpSolver(uint32_t max_cached_samples, uint32_t  feat_len, float32_t svm_C, float32_t J, uint32_t pruneSize) : 
    max_num_samples(max_cached_samples),
    c(svm_C),
    featLength(feat_len),
    Jpos(J),
    l(0),
    n(0),
    ub(0),
    lb(0),
    bias(1.0f),
    pruneDataSize(pruneSize)  //1000
{
    //allocate memory        
    _DataType *data = (_DataType *) MEMALLOC(max_cached_samples * (feat_len+1) * sizeof(_DataType), 16);
    x = (_DataType **) MEMALLOC(max_cached_samples * sizeof(_DataType*), 16);
    _DataType *dataPtr = data;
    for (uint32_t i=0;i<max_cached_samples;i++)
    {
        x[i] = dataPtr;
        dataPtr += feat_len+1;
    }
    
    b = (_DataType*) MEMALLOC(max_cached_samples * sizeof(_DataType), 16);
    std::memset(b, 0, max_cached_samples * sizeof(_DataType));

    d = (_DataType*) MEMALLOC(max_cached_samples * sizeof(_DataType), 16);
    std::memset(d, 0, max_cached_samples * sizeof(_DataType));

    a = (_DataType*) MEMALLOC(max_cached_samples * sizeof(_DataType), 16);
    std::memset(a, 0, max_cached_samples * sizeof(_DataType));
        
    w = (_DataType*) MEMALLOC( (featLength+1) * sizeof(_DataType), 16);
    std::memset(w, 0, (featLength+1) * sizeof(_DataType) );

    //non support vector flags
    nonSV = (uint8_t*) MEMALLOC(max_cached_samples * sizeof(uint8_t), 16);
    std::memset(nonSV, 0, max_cached_samples * sizeof(uint8_t));

    //data flags: existance (bit-0), positive and negative (bit-1)
    isdata  = (uint8_t*) MEMALLOC(max_cached_samples * sizeof(uint8_t), 16);
    std::memset(isdata, 0, max_cached_samples * sizeof(uint8_t));
    
    numAvailable = 0;
    availableIdx  = (uint32_t*) MEMALLOC(max_cached_samples * sizeof(uint32_t), 16); //indices for availalbe data slots
    std::memset(availableIdx, 0, max_cached_samples * sizeof(uint32_t));
    
    //temprary buffer 
    //Total size: 4*sizeof(uint32_t) + 4*sizeof(uint32_t *) + 4*max_num_samples*sizeof(uint32_t)
    scratchBuffer.setSize(max_num_samples * 16 + 100, 16);  
}

template<class _DataType>
QpSolver<_DataType>::~QpSolver() 
{
    MEMFREE(x[0]);   
    MEMFREE(x);   
    MEMFREE(b);
    MEMFREE(d);
    MEMFREE(a);
    MEMFREE(w);
    MEMFREE(nonSV);
    MEMFREE(isdata);
    MEMFREE(availableIdx);        
}
    
template<class _DataType>
bool QpSolver<_DataType>::addSample(const TrainingSample<_DataType>& ex) 
{        
    // if (~ qp->IsCacheFull())
	// qp->write(samples);
	//uint32_t i = n; // i is the index for vector, start from 0.        
	if (n >= max_num_samples ) 
    {
        //return false;        
        //ideally, we should have enough space after last optimization, so we should come in here often
        //optimize and prune
        optimize(0.05f, 200); //how many iterations we should run(?)
        prune();
        //assert(numAvailable>0 && n < max_num_samples );        
    }

    // find the index for adding example
    uint32_t i;
    if (numAvailable) // taken from the available slots 
    {
        numAvailable--;
        i = availableIdx[numAvailable];
    }
    else
        i = n;
    //n increase for both cases
    n++; 

    //% Add constraints to QP
	//% w*feat(:,i) >= margin(i) - slack(i)
	//assert(length(label) == 1);
	//assert(label == 1 || label == -1);
	assert(ex.classIndex ==1 || ex.classIndex == 0);
  
	/*% Absorb the label into the feature vector.
	if label == -1
		feat = -1*feat;
	end*/
    assert(ex.featureView.getLength()==featLength);

    //copy data to the current data slot (i)
    //qp.x(:,i) = feat;
    _DataType *featvect = ex.featureView.getData();        
	if (ex.classIndex == 0)
    {
        for (uint32_t idx = 0; idx < featLength; idx ++)
            x[i][idx] = (_DataType) -1 * featvect[idx];
        //set bias
        x[i][featLength] = -bias;
    }
    else
    {
        //for (uint32_t idx=0; idx < featLength; idx++)
        //    x[i][idx] = featvect[idx];
        std::memcpy(x[i], ex.featureView.getData(), featLength*sizeof(_DataType) );
        //set bias
        x[i][featLength] = bias;        
    }
    
	//qp.d(i) = sum(feat.^2);
    d[i] = fcvDotProductf32(x[i], x[i], featLength+1);
       
	/*if label == 1
		qp.b(i) = qp.Jpos;
	else
		qp.b(i) = 1;
	end*/
	if (ex.classIndex ==1)
		b[i] = Jpos;
	else
		b[i] = 1;

    //set isdata[i]
    if (ex.classIndex == 1)
        isdata[i] = 3; //bit 1 = 1, bit 0 = 1, 
    else
        isdata[i] = 1; //bit 1 = 0, bit 0 = 1, 
    return true;
}


template<class _DataType>
void QpSolver<_DataType>::getModel(SVMClassifierModelLinear<_DataType> &svmModel)
{	
    svmModel.setWeights(w, featLength);    
    svmModel.setBias(w[featLength]);
} 
	
// Optimize QP until relative difference between lower and upper bound is below 'tol'
// example:  tol = .05;		iter = 1000;	
template<class _DataType>
uint32_t QpSolver<_DataType>::optimize(float32_t tol, uint32_t iter)
{	
    uint32_t numSamples = min(max_num_samples, n + numAvailable);

    float32_t ubt = estimateUpperBound(numSamples);

	//float32_t lbt  = lb;
    //printf("[");
	/*%fprintf('\n LB=%.4f,UB=%.4f [',lb,ub);
	fprintf('[');
       
	% Iteratively apply coordinate descent, pruning active set (support vectors)
	% If we've possible converged over active set
	% 1) Compute true upper bound over full set
	% 2) If we haven't actually converged, 
	%    reinitialize optimization to full set*/
	//qp.sv(1:qp.n) = 1;

	/*for t = 1:iter,
		qp_one();
		lb     = qp.lb;
		ub_est = min(qp.ub,ub);
		fprintf('.');
		if lb > 0 && 1 - lb/ub_est < tol,
		slack = qp.b' - qp.w'*qp.x;
		loss  = sum(max(slack,0));
		ub    = min(ub,qp.w'*qp.w*.5 + qp.C*loss);  
		if 1 - lb/ub < tol,
			break;
		end
		qp.sv(1:qp.n) = 1;
		end
		%fprintf('t=%d: LB=%.4f,UB_true=%.5f,UB_est=%.5f,#SV=%d\n',t,lb,ub,ub_est,sum(qp.sv));
	end*/

    //set the sv flags
    std::memset(nonSV, 0, numSamples *sizeof(uint8_t));

    uint32_t t;
	for(t = 0; t < iter; ++t)
	{
		optimizeStep();
		//lbt = lb;
        float32_t ub_est = min(ub, ubt);

		//printf(".");           
		if (lb > 0 && ((1 - lb/ub_est) < tol)) //quick check for converge
		{		
            ubt = min(ubt, estimateUpperBound(numSamples));
			//check if converge
            if (1 - lb/ubt < tol) 
                break;

            //reset the sv flags
            std::memset(nonSV, 0, numSamples *sizeof(uint8_t));            
		}
	}
	/*qp.ub = ub;
	fprintf(']');
	%fprintf('] LB=%.4f,UB=%.4f\n',lb,ub);*/
	ub = ubt;
    //printf("]");

    return numSamples; //return the number of iterations
}

//Re-indexes qp.x to begin with active consrtaints (eg, the support vectors)
template<class _DataType>
uint32_t QpSolver<_DataType>::prune()
{     
    //count four different types sampels 
    //four types for sampels to remove in the following order
    //1. nonSV==true (easy negative samples)
    //2. nonSV==false and a == 0 and negative (non support vector negative samples)
    //3. nonSV==false and a == 0 and positive (non support vector positive samples)
    //4. nonSV==false and a != 0 (support vectors)    
    uint32_t *typeCounts = (uint32_t *) scratchBuffer.reserveMem(4*sizeof(uint32_t));
    memset(typeCounts, 0, 4*sizeof(uint32_t));
    //indices for four different types
    uint32_t **typeIndices = (uint32_t **) scratchBuffer.reserveMem( 4*sizeof(uint32_t *) );
    uint32_t *tempBuf= (uint32_t *) scratchBuffer.reserveMem(4*max_num_samples*sizeof(uint32_t));    
    for (uint8_t t=0; t<4; t++)
    { 
        typeIndices[t] = tempBuf;
        tempBuf += max_num_samples;
    }
    
    uint32_t numSamples = min(max_num_samples, n + numAvailable);

    for (int32_t i=numSamples-1; i>=0 ; i--)
    {
        if (isdata[i]==0)
            continue;

        if (nonSV[i]) //type 1: nonSV==true (easy negative samples)
        {            
            typeIndices[0][typeCounts[0]] = i;
            typeCounts[0]++;
        }
        else if (a[i]==0) //non support vector 
        {
            if (isdata[i]==1) //type 2. nonSV==false and a == 0 and negative (non support vector negative samples)
            {
                typeIndices[1][typeCounts[1]]=i;
                typeCounts[1]++;
            }
            else //type 3. nonSV==false and a == 0 and positive (non support vector positive samples)
            {
                typeIndices[2][typeCounts[2]]=i;
                typeCounts[2]++;
            }
        }
        else //type 4. nonSV==false and a != 0 (support vectors)    
        {
            typeIndices[3][typeCounts[3]]=i;
            typeCounts[3]++;
        }
    }

    //remove samples by types    
    bool updateW;
    int32_t num2Remove = n + pruneDataSize - max_num_samples;    
    uint32_t curr2Remove;
    for (uint32_t type=0; type < 4; type++)    
    {
        //if (num2Remove<=0)
        //    break;
        //always remove the type 1 samples, so we check after pruning         
        if (type ==0)
            curr2Remove = typeCounts[type];
        else 
            curr2Remove = (uint32_t)MIN((int32_t)typeCounts[type], num2Remove);
        for (uint32_t i=0; i < curr2Remove; i++)
        {
            //remove sample 
            isdata[ typeIndices[type][i] ] = 0;            
            availableIdx[numAvailable] = typeIndices[type][i];
            numAvailable ++;
            n--;
        }
        updateW = (type==3) && (curr2Remove>0);
        num2Remove -= curr2Remove;
        if (num2Remove<=0)
            break;
    }       
    scratchBuffer.releaseMem(4*sizeof(uint32_t));
    scratchBuffer.releaseMem( 4*sizeof(uint32_t *) );
    scratchBuffer.releaseMem(4*max_num_samples*sizeof(uint32_t));

    //compress the sample buffer if there were lots of samples being removed fro type 0
    if (numAvailable > n / 4)
        compressSampleBuffer();

    //Updating the weights doesn't show good results, so we don't run it now
    //if removed support vectors, we then need to update the weights
    //if (updateW)
    //{
        //optimize(0.05f, 10); 
        /*
        //remove samples from loss and weights
		for (uint32_t j = 0; j < curr2Remove; j++) 
        {
			int32_t i = typeIndices[3][j];			
            //update loss 
			l -= b[i]*a[i];
            //update w
            for (uint32_t idx=0; idx< featLength+1; idx++)
                w[idx] -= x[i][idx]*a[i];
		}
        */
        /*
		//reset loss and weights       
        l = 0;        
        std::memset(w, 0, (featLength+1) * sizeof(_DataType) );
		for (uint32_t j = curr2Remove; j < typeCounts[3]; j++) 
        //for (uint32_t j = 0; j < typeCounts[3]; j++) 
        {
			int32_t i = typeIndices[3][j];			
            //update loss 
			l += b[i]*a[i];
            //update w
            for (uint32_t idx=0; idx< featLength+1; idx++)
                w[idx] += x[i][idx]*a[i];
		}
        */
    //}
    
    return n;
}


    
// qp_one requires float data. 
// perform one pass through current set of support vectors
template<class _DataType>
void QpSolver<_DataType>::optimizeStep()
{ 
    uint32_t numSamples = min(max_num_samples, n + numAvailable);

	//I = find(qp.sv);
    //reserve scratch buffer
    int32_t *I = (int32_t *) scratchBuffer.reserveMem(numSamples*sizeof(int32_t));
    uint32_t sizeI=0;	
    for(uint32_t i = 0; i < numSamples; ++i)
	{
        if (isdata[i] && nonSV[i]==0) 
        {
            I[sizeI] = i;
            sizeI++;
        }
    }
    assert(sizeI);

	// myvector stores indices find(qp.sv)
	//I = I(randperm(length(I)));
    qcv::permute(I, sizeI);

	//loss = qp_one_c(qp.x,qp.b,qp.d,qp.a,qp.w,qp.sv,qp.l,qp.C,I);
	/* float  *X  = (float  *)mxGetPr(prhs[0]);
	float  *B  = (float  *)mxGetPr(prhs[1]);
	double *D  = (double *)mxGetPr(prhs[2]);
	double *A  = (double *)mxGetPr(prhs[3]);
	double *W  = (double *)mxGetPr(prhs[4]);
	bool   *SV = (bool   *)mxGetPr(prhs[5]);
	double *L  = (double *)mxGetPr(prhs[6]);
	double  C  = (double  )mxGetScalar(prhs[7]);
	double *I  = (double *)mxGetPr(prhs[8]);*/
	/* mwSize m = mxGetM(prhs[0]);
	mwSize n = MAX(mxGetN(prhs[8]),mxGetM(prhs[8]));*/
	// uint32_t m = x.size();    
	//double loss = 0;
	float32_t loss = 0;

	for (uint32_t cnt = 0; cnt < sizeI; cnt++) 
	{
		// Use C indexing
		uint32_t i = (uint32_t)I[cnt];
        float32_t G = fcvDotProductf32(w, x[i], featLength+1) - b[i];

		//float32_t *x = X + m*i;
    
		//for (int d = 0; d < m; d++) {
		//  G += W[d] * (double)x[d];
		//  //printf("(%g,%g,%g)",G,W[d],x[d]);
		//}
			
		float32_t PG = G;
    
		if ((a[i] == 0 && G >= 0) || (a[i] == c && G <= 0)) 
			PG = 0;

		if (a[i] == 0 && G > 0 && isdata[i]==1) //only set for negative samples 
			nonSV[i] = 1;

		if (G < 0) 
			loss -= G;

	    //printf("[%d,%d,%g,%g,%g]\n",cnt,i,G,PG,A[i]);
		if (PG > 1e-12 || PG < -1e-12) 
		{
			float32_t dA = a[i];      
			a[i] = min ( max ( a[i] - G/d[i], 0.0f ) , c );
			dA   = a[i] - dA;
			l += dA * b[i];
			//printf("%g,%g,%g,%g\n",A[i],B[i],dA,*L);
			//for (int d = 0; d < m; d++) {
			//W[d] += dA * (double) x[d];
			//}

            //update w
            for (uint32_t idx=0; idx< featLength+1; idx++)
                w[idx] += x[i][idx]*dA;
		}
	}
	//plhs[0] = mxCreateDoubleScalar(loss);
			
	/*else
	loss = 0;
	for i = I,
		% Compute clamped gradient
		G = qp.w'*qp.x(:,i) - qp.b(i);
		if (qp.a(i) == 0 && G >= 0) || (qp.a(i) == qp.C && G <= 0),
		PG = 0;
		else
		PG = G;
		end
		if (qp.a(i) == 0 && G > 0),
		qp.sv(i) = 0;
		end
		if G < 0,
		loss = loss - G;
		end
		% Update alpha,w, dual objective, support vector
		if (abs(PG) > 1e-12)
		a = qp.a(i);
		qp.a(i) = min(max(qp.a(i) - G/qp.d(i),0),qp.C);
		qp.w = qp.w + (qp.a(i) - a)*qp.x(:,i);
		qp.l = qp.l + (qp.a(i) - a)*qp.b(i);
		end
	end
	end*/
	
    //update upper bound 			  
	//qp.lb = qp.l - qp.w'*qp.w*.5;
    float32_t wSqr = fcvDotProductf32(w, w, featLength+1);
	lb = l - 0.5f*wSqr;
	//qp.ub = qp.w'*qp.w*.5 + qp.C*loss;
	ub = 0.5f*wSqr + c*loss;

/*  %{
	% Sanity check (expensive to commute)
	J = find(mask);
	K = qp.x(:,J)*qp.a(J);
	dual = qp.b(J)'*qp.a(J) - K'*K*.5;
	fprintf('obj=(%.3f,%.3f)',qp.obj,dual);
	assert(abs(qp.obj - dual) < .1);
	%}
*/	
    //release scratch buffer
    scratchBuffer.releaseMem(numSamples*sizeof(int32_t));

}

template<class _DataType>
float32_t QpSolver<_DataType>::estimateUpperBound(uint32_t numSamples)
{

    // slack = b-w*x;
	//matrixVectorMultiply(x, w, &scores);// should do by idx 
	//ub  = w'* w * .5 + c * loss;
    float32_t loss  = 0.0f;
    for (uint32_t i = 0; i < numSamples; ++i)
    {
		if (isdata[i])
            loss += max(b[i] - fcvDotProductf32(x[i], w, featLength+1), 0.0f);
    }
    float32_t wSqr = fcvDotProductf32(w, w, featLength+1);
    float32_t ubt = 0.5f * wSqr + c * loss;
    return ubt;
/*
    float32_t *scores = (float32_t *)scratchBuffer.reserveMem(numSamples*sizeof(float32_t));
    
	for (uint32_t i = 0; i < numSamples; ++i)
    {
		if (isdata[i])
            scores[i] = dotProduct(x[i], w);
        else
            scores[i] = 0;
    }
        
    vector<float32_t> slack;
	vectorSub(b, scores, &slack);
    
	vectorScalarMax(slack, 0.0f, &max0);
	float32_t loss = vectorSelfSum(max0);
    
	float32_t ubt = dotProduct(w, w)*(0.5f) + c * loss;
*/

}


//compress sample buffer to move samples to the front
template<class _DataType>
void QpSolver<_DataType>::compressSampleBuffer()
{
    //the compression doesn't remove the value of n, but reduce the value of numAvailable
    //numAvailable;
    //availableIdx
    
    uint32_t bottomIdx = min(max_num_samples, n + numAvailable);

    //go from the bottom of the sample list
    while (bottomIdx > n) // && numAvailable > 0)
    {
        bottomIdx--;
        //copy sample up into the list 
        if (isdata[bottomIdx])
        {
            numAvailable--;
            uint32_t topIdx = availableIdx[numAvailable];
            
            assert(topIdx < bottomIdx);
            
            //move data
            std::memcpy(x[topIdx], x[bottomIdx], (featLength+1)*sizeof(_DataType) );
            isdata[topIdx] = isdata[bottomIdx];            
            a[topIdx] = a[bottomIdx];
            b[topIdx] = b[bottomIdx];
            d[topIdx] = d[bottomIdx];
            //nonSV[idx] = nonSV[numSamples];

            //clear the old slot
            isdata[bottomIdx] = 0;
        }
    }

    //clean the available index
    numAvailable = 0;
}



}   // namespace qcv

#endif // qcv_qpsolver_h_
//

