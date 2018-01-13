/**=============================================================================

@file
qcvMath.h

@brief
some internally defined math functions 

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


//
#ifndef qcv_Math_h__
#define qcv_Math_h__

// Some common shared global types

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <vector>
#include <algorithm>

#include "fastcv.h"

//Common Defines
using namespace std;
namespace qcv
{

#ifndef math_PI
#define math_PI 3.14159265358979
#endif

#ifndef MIN
#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

#ifndef SWAP
#define SWAP(a,b) { float32_t t=(a);(a)=(b);(b)=t; }
#endif

#ifndef Floor
#define Floor(value) ((int32_t)value - ((int32_t)value > value))
#endif

#ifndef Round
#define Round(value) (int32_t)((value>0)?((float32_t)(value) + 0.5f):(((float32_t)(value) - 0.5f)))
//#define Round(value) ( (int32_t)( (float32_t)value + 0.5 ) )
#endif

template<typename T> T abs(T v) { return v>=0? v : -v; }

template<typename T> T clamp(T v, T minv, T maxv) { return v<minv? minv : (v>maxv? maxv : v); }

/*
template<class T>
inline const T& min(const T& val1, const T& val2)
{
	return val1 < val2 ? val1 : val2;
}
template<class T>
inline const T& max(const T& val1, const T& val2)
{
	return val1 > val2 ? val1 : val2;
}
*/



//random number generator, return value between 0 and 1
//if started with seed=0,  always generate 1.0
int32_t randVal(int32_t seed);
float32_t randVal_f(void);

//Shuffle an array data
void permute(int32_t *data, uint32_t num);

void permuteAnyArray(void *data, uint32_t num, uint32_t size, void *swapbuff);

//compute the median, the temporary buffer should have length of "number", 
//if NULL is given, then the function will allocate a buffer
float32_t median(float32_t *mediaArray, int32_t number, float32_t *tempBuff=NULL);
float32_t fast_median_sort(float32_t *medianArray, int32_t number);

//a sigmoid function y = 1/(1+exp(-coeff_a*x+b))
float32_t sigmoid(float32_t x, float32_t coeff_a, float32_t coeff_b);

//compute the dot-product of two array

//compute the dot-product of two array
template<class _T>
_T dotProduct(const _T *arr0, const _T *arr1, uint32_t arrLen)
{
    _T n = (_T)0;
    
    for(uint32_t i = 0; i < arrLen; i++)
        n += arr0[i] * arr1[i];

    return n;
}

template<class _T>
_T autoCorrelation(const _T *arr, uint32_t arrLen)
{
    return dotProduct<_T>(arr, arr, arrLen); 
}

// Compute the dot-product of two vectors
template<class _T>
_T dotProduct(const vector<_T>& arr0, const vector<_T>& arr1)
{
	assert(!arr0.empty());
	assert(!arr1.empty());
	assert(arr0.size() == arr1.size());
	_T sum = (_T)0;
    
	for(uint32_t i = 0; i < arr0.size(); ++i)
		sum += arr0[i] * arr1[i];
	return sum;
}

// Compute the sum of two vectors
template<class _T>
void vectorSum(const vector<_T>& arr0, const vector<_T>& arr1, vector<_T>* out)
{
	assert(!arr0.empty());
	assert(!arr1.empty());
	assert(arr0.size() == arr1.size());
	out->resize(arr0.size());
    
	for(uint32_t i = 0; i < arr0.size(); ++i)
		(*out)[i] = arr0[i] + arr1[i];
}

// Substract a vector from another.
template<class _T>
void vectorSub(const vector<_T>& arr0, const vector<_T>& arr1, vector<_T>* out)
{
	assert(!arr0.empty());
	assert(!arr1.empty());
	assert(arr0.size() == arr1.size());
	out->resize(arr0.size());
    
	for(uint32_t i = 0; i < arr0.size(); ++i)
		(*out)[i] = arr0[i] - arr1[i];
}

// Compute the sum of the vector elements.
template<class _T>
_T vectorSelfSum(const vector<_T>& vec)
{
	assert(!vec.empty());
	_T sum = (_T)0;
 
	for(uint32_t i = 0; i < vec.size(); ++i)
		sum += vec[i];
	return sum;
}

// Compute the squared norm of the vector elements.
template<class _T>
_T vectorNorm2(const vector<_T>& vec)
{
	assert(!vec.empty());
	_T norm2 = (_T)0;
 
	for(int32_t i = 0; i < vec.size(); ++i)
		norm2 += (vec[i]*vec[i]);
	return norm2;
}

// Compute the element-wise max between a vector and a scalar.
template<class _T>
void vectorScalarMax(const vector<_T>& vec, _T s, vector<_T>* out)
{
	assert(!vec.empty());
	out->resize(vec.size());
    
	for(uint32_t i = 0; i < vec.size(); ++i)
		(*out)[i] = max(vec[i], s);
}

// Compute the element-wise max between a vector and a scalar.
template<class _T>
void vectorScalarMin(const vector<_T>& vec, _T s, vector<_T>* out)
{
	assert(!vec.empty());
	out->resize(vec.size());
    
	for(uint32_t i = 0; i < vec.size(); ++i)
		(*out)[i] == min(vec[i], s);
}

// Compute the element-wise max between a vector and a scalar.
template<class _T>
void vectorScalarMultiply(const vector<_T>& vec, _T s, vector<_T>* out)
{
	assert(!vec.empty());
	out->resize(vec.size());
    
	for(uint32_t i = 0; i < vec.size(); ++i)
		(*out)[i] = vec[i] * s;
}
 
// Compute the element-wise max between a vector and a scalar.
template<class _T>
void vectorScalarSelfMultiply(vector<_T>& vec, _T s)
{
	assert(!vec.empty());
    
	for(uint32_t i = 0; i < vec.size(); ++i)
		vec[i] = vec[i] * s;
}

// Matrix-vector multiplication: out = mat * v.
// Inputs:
// mat is a n-by-d matrix.
// v is a d-by-1 vector.
// Output:
// out is a n-by-1 vector.
template<class _T>
void matrixVectorMultiply(const vector<vector<_T> >& mat, const vector<_T>& v, vector<_T>* out)
{
	assert(!mat.empty());
	assert(!v.empty());
	out->resize(mat.size());
	for (uint32_t i = 0; i < mat.size(); ++i)
		(*out)[i] = dotProduct(mat[i], v);
}


//compare used to sort only indices instead of content
//sort ascently 
template<class _T>
class compareIndex
{
public:
  const _T *baseValues;

  compareIndex (const _T *val) : baseValues (val) {}
 
  //a and b are index of the value array
  bool operator () (int a, int b) const
  {
    if (baseValues[a] > baseValues[b])
        return 1;
    else if (baseValues[a] == baseValues[b])
    {
        if (a < b)
            return -1;
        else
            return 1;
    }
    else
        return -1;
  }
};

//c code to compare the value and sort the index (in ascent order)
template<class _T>
struct ValueIndexPair
{
    uint32_t index;
    _T       value;
};

//comparison function for qsort, order = 1 for ascent order(default), -1 for descent order
template<class _T, int32_t order>
int qsortValueIndexPair (const void *e1, const void *e2)
{
    assert( order==1 || order==-1);
    ValueIndexPair<_T> *p1 = (ValueIndexPair<_T> *)e1;
    ValueIndexPair<_T> *p2 = (ValueIndexPair<_T> *)e2;
    
    if (p1->value > p2-> value )
        return order;
    else if (p1->value == p2->value)
    {
        if (p1->index < p2->index)
            return -1;
        else
            return 1;
    }
    else
        return -order;
}

}   // namespace qcv

#endif // qcv_Math_h__
