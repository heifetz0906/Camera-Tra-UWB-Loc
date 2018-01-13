#ifndef qcv_ACFUTILS_h_
#define qcv_ACFUTILS_h_

#include <vector>
#include"fastcv.h"

float32_t root3(float32_t x);

void computeLUT(const uint8_t r, const uint8_t g,
	const uint8_t b, float32_t& L, float32_t& U, float32_t& V);

void computeGrad(float32_t* l,float32_t* u, float32_t* v, float32_t* mag, float32_t* ori,
	const uint32_t width, const uint32_t height);

void convTriangle(float32_t* src, float32_t* dst, int32_t rad,
	const uint32_t width, const uint32_t height);

void computeGradHist(float32_t *M, float32_t *O, float32_t *H, int32_t h, int32_t w,
	int32_t bin, int32_t nOrients, int32_t softBin, bool full);

void resize_f(float32_t* in, float32_t* out, int32_t w_in, int32_t h_in, int32_t stride_in,
	int32_t w_out, int32_t h_out, int32_t stride_out);

int32_t scalef32Bilinear(float32_t* src,uint32_t srcWidth,uint32_t srcHeight,uint32_t srcStride,float32_t* dst,uint32_t dstWidth,
                     uint32_t dstHeight,uint32_t dstStride,fcvBorderType borderType,float32_t borderValue);

uint32_t generateScales(std::vector<float32_t> & scales, std::vector<float32_t> & scaleshw, uint32_t num_scales, uint32_t scales_per_octave, float32_t shrink, uint32_t baseWidth, uint32_t baseHeight);

void PadImagef32(const float32_t* __restrict src,
    uint32_t srcWidth,
    uint32_t srcHeight,
    uint32_t srcStride,
    float32_t* padded,
    uint32_t kW,
    uint32_t kH,
    fcvBorderType btype,
    float32_t borderValue);

#endif