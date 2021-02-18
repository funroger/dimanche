// copyright (c) 2013 Victor Cherepanov
// distributed under BSD 3-clause license

#include <dimanche/image_processing/inc/color_conversion_internal.h>

#include <array>

namespace dimanche {
namespace color_conversion {
namespace yuv_to_bgr {

constexpr double scale = (double) (1u << Converter::CONST_SCALE);
constexpr double round = 0.5;

const std::array<CONSTS, 4> Constants =
{{
    // BT.601.5
    {
        (int32_t) (1.164383562 * scale + round),
        (int32_t) (2.017232143 * scale + round),
        (int32_t) (0.391762290 * scale + round),
        (int32_t) (0.812967647 * scale + round),
        (int32_t) (1.596026786 * scale + round),
        // these constants were generated the special way to make conversion errors
        // as little as possible. The errors happen with 30% probability and don't
        // exceed the floating-point result more than on 1.
        8841,
        4355,
        7116
    },
    // BT.709.5
    {
        (int32_t) (1.164383562 * scale + round),
        (int32_t) (2.112401786 * scale + round),
        (int32_t) (0.213248614 * scale + round),
        (int32_t) (0.532909329 * scale + round),
        (int32_t) (1.792741071 * scale + round),
        // these constants were generated the special way to make conversion errors
        // as little as possible. The errors happen with 30% probability and don't
        // exceed the floating-point result more than on 1.
        9231,
        2477,
        7922
    },
    // BT.2020 (copied from BT.709.5)
    {
        (int32_t) (1.164383562 * scale + round),
        (int32_t) (2.112401786 * scale + round),
        (int32_t) (0.213248614 * scale + round),
        (int32_t) (0.532909329 * scale + round),
        (int32_t) (1.792741071 * scale + round),
        // these constants were generated the special way to make conversion errors
        // as little as possible. The errors happen with 30% probability and don't
        // exceed the floating-point result more than on 1.
        9231,
        2477,
        7922
    },
    // JPEG full range
    {
        (int32_t) (1.0 * scale + round),
        (int32_t) (1.772 * scale + round),
        (int32_t) (0.344136 * scale + round),
        (int32_t) (0.714136 * scale + round),
        (int32_t) (1.402 * scale + round),
        // these constants were generated the special way to make conversion errors
        // as little as possible. The errors happen with 30% probability and don't
        // exceed the floating-point result more than on 1.
        7242,
        4350,
        5726
    }
}};

const CONSTS GetConstants(const image::FORMAT &dstFormat) {
    const size_t idx = (size_t) dstFormat.colorSpace % Constants.size();
    return Constants[idx];
}

} // namespace yuv_to_bgr

} // namespace color_conversion
} // namespace dimanche
