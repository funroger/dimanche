// copyright (c) 2013 Victor Cherepanov
// distributed under BSD 3-clause license

#pragma once

#if !defined(__DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_INTERNAL_H)
#define __DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_INTERNAL_H

#include <dimanche/image_processing/inc/internal.h>

#include <dimanche/basic/templates.h>
#include <dimanche/system/cpu_opt.h>

namespace dimanche {
namespace color_conversion {
namespace yuv_to_bgr {

// a struct with constants for YUV to RGB conversion
struct CONSTS
{
    uint32_t y;
    uint32_t u_for_b;
    uint32_t u_for_g;
    uint32_t v_for_g;
    uint32_t v_for_r;
    uint32_t blue;
    uint32_t green;
    uint32_t red;
};

// get constants for given conversion type
const CONSTS GetConstants(const image::FORMAT &srcFormat);

// a class for conversion from YUV to BGR conversion
class Converter
{
public:

    // declare common constants for conversion from YUV formats to BGR formats
    enum
    {
        DATA_SCALE = 8,
        CONST_SCALE = 13,
        BACK_SCALE = (DATA_SCALE + CONST_SCALE - 16)
    };

    // default constructor
    Converter(const CONSTS &constants,
        const uint32_t bitDepthMinus8 = 0,
        const uint32_t bitDepthChromaMinus8 = 0) :
        consts(constants),
        lumaScale(DATA_SCALE - bitDepthMinus8),
        chromaScale(DATA_SCALE - bitDepthChromaMinus8) {}

    inline
    uint8_t B(const int32_t addScale, const int32_t y, const int32_t u) const
    {
        int32_t value;

        // the original conversion formulae is
        // blue = (int) ((y - 16.0) * 1.164 +
        //               (u - 128.0) * 2.017 + 0.5);
        //
        // emulate SIMD algorithm for exact matching

        value = ((y << lumaScale) * consts.y) >> 16;
        value += ((u << (chromaScale - addScale)) * consts.u_for_b) >> 16;
        value -= consts.blue;
        value >>= BACK_SCALE;

        return to_byte(value);
    }

    inline
    uint8_t G(const int32_t addScale,
        const int32_t y, const int32_t u, const int32_t v) const
    {
        int32_t value;

        // the original conversion formulae is
        // green = (int) ((y - 16.0) * 1.164 -
        //                (u - 128.0) * 0.391 -
        //                (v - 128.0) * 0.812 + 0.5);
        //
        // emulate SIMD algorithm for exact matching

        value = ((y << lumaScale) * consts.y) >> 16;
        value -= ((u << (chromaScale - addScale)) * consts.u_for_g) >> 16;
        value -= ((v << (chromaScale - addScale)) * consts.v_for_g) >> 16;
        value += consts.green;
        value >>= BACK_SCALE;

        return to_byte(value);
    }

    inline
    uint8_t R(const int32_t addScale, const int32_t y, const int32_t v) const
    {
        int32_t value;

        // the original conversion formulae is
        // red = (int) ((y - 16.0) * 1.164 +
        //              (v - 128.0) * 1.596 + 0.5);
        //
        // emulate SIMD algorithm for exact matching

        value = ((y << lumaScale) * consts.y) >> 16;
        value += ((v << (chromaScale - addScale)) * consts.v_for_r) >> 16;
        value -= consts.red;
        value >>= BACK_SCALE;

        return to_byte(value);
    }

private:
    const int32_t lumaScale, chromaScale;
    const CONSTS consts;
};

} // namespace yuv_to_bgr
} // namespace color_conversion

#define COLOR_CONVERSION_FORMAL_PARAMS \
    const IMAGE *pDstImage, const IMAGE *pSrcImage
#define COLOR_CONVERSION_ACTUAL_PARAMS \
    pDstImage, pSrcImage

DECL_PROC(void, di_convert_nv12_to_bgra, (COLOR_CONVERSION_FORMAL_PARAMS), (COLOR_CONVERSION_ACTUAL_PARAMS))

} // namespace dimanche

#endif // !defined(__DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_INTERNAL_H)
