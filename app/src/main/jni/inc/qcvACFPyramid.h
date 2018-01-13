#ifndef qcv_ACFPyramid_h_
#define qcv_ACFPyramid_h_


#include <cstddef>
#include <vector>

#include "qcvTypes.h"
#include "qcvImage.h"
#include "qcvImageACF.h"
#include "fastcv.h"
#include "qcvMemory.h"

#ifndef ACF_DEBUG
#define ACF_DEBUG
#endif

#undef ACF_DEBUG

namespace qcv
{

	/*********************************************************************
	class ACFPyramid: wrapper class for ACF pyramid image
	**********************************************************************/
	class ACFPyramid
	{
	public:
		ACFPyramid();
		ACFPyramid(uint32_t imgW, uint32_t imgH, uint32_t imgStride, 
			uint32_t num_octaves, uint32_t scales_per_octave, uint32_t num_scales,uint32_t _shrink, uint32_t _padWidth, uint32_t _padHeight, uint32_t _smooth);
		virtual ~ACFPyramid();

		//public member functions
        virtual void initialize(uint32_t imgW, uint32_t imgH, uint32_t imgStride,
                                uint32_t _num_octaves, uint32_t _scales_per_octave,
                                uint32_t _num_scales, uint32_t _shrink, uint32_t _padWidth, uint32_t _padHeight, uint32_t _smooth);
		virtual void uninitialize();

		//compute the pyramid image for a given a color image 
		virtual void compute(const PlanarView baseImg);
        void padPyramidImages();


	public:
		uint32_t            baseWidth;
		uint32_t            baseHeight;
		uint32_t            baseStride;
		uint32_t            pyramidLevels;          //number of Pyramid Level	
		uint32_t			num_octaves;
		uint32_t			scales_per_octave;
        uint32_t            num_scales;
		bool				isInit;
        uint32_t            shrink;
        uint32_t            p_smooth;
		
		std::vector<ACFImage> acfImages;

        //Precomputed Relative Scales and Exact Scales for resampling width and height 
        std::vector<float32_t> scales, scaleshw;
        
		//TODO: add plannar views of each pyramid level
    private:
        qcvMemory   scratchbuff;
        //Vectors to keep track of octaves and intermediate scales
        std::vector<uint32_t> isR, isA, isN;

        void computeIndices();
        

	};

}   // namespace qcv
#endif  // qcv_ACFPyramid_h_