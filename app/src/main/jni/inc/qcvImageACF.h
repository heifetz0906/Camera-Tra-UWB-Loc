#ifndef qcv_ACFImage_h_
#define qcv_ACFImage_h_

#include "qcvTypes.h"
#include "qcvImage.h"

namespace qcv {
    class ACFImage {
        public:
        ACFImage();
        //ACFImage(uint32_t _width, uint32_t _height, uint32_t _num_channels);		
        ~ACFImage();
        void initialize(uint32_t _width, uint32_t _height, uint32_t _scaledWidth, uint32_t _scaledHeight, uint32_t _padWidth, uint32_t _padHeight, uint32_t _num_channels);
        void uninitialize();

        void compute(const PlanarView baseImg, uint32_t idx, uint32_t shrink, uint32_t smooth);

        float32_t* getDataPtr() { return data; }
        float32_t* getPadPtr() { return data_padded; }
        uint32_t getWidth() { return width; }
        uint32_t getHeight() { return height; }
        uint32_t getWidth2() { return width_2; }
        uint32_t getHeight2() { return height_2; }
        uint32_t getWidthPad() { return width_pad; }
        uint32_t getHeightPad() { return height_pad; }
        uint32_t getNumChannels() { return num_channels; }

#ifdef ACF_DEBUG
        // Debug calls
#ifdef WIN32
        void display();
        void displayPad(uint32_t channelIdx);
#endif
#endif
        private:

        uint32_t width, width_2, width_pad;
        uint32_t height, height_2, height_pad;
        uint32_t num_channels;
        float32_t* data;  // Continous block of data for all channels
        float32_t* data_padded;
    };
}

#endif  // qcv_ACF_Image_h_