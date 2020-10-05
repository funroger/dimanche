
#pragma once

#if !defined(__DIMANCHE_BASIC_COLOR_FORMATS_H)
#define __DIMANCHE_BASIC_COLOR_FORMATS_H

#include <dimanche/basic/types.h>

namespace dimanche {
namespace image {

enum class eColorFormat : uint32_t {
    NONE = 0,
    BGR = 1,
    BGRA = 2,
    UYVY = 3,
    YUY2 = 4,
    YVYU = 5,
    NV12 = 6,
    // total number of color formats
    NUM
};

enum class eColorSpace : uint16_t
{
    DEFAULT = 0,

    ITUR_BT601_5 = 0,
    ITUR_BT709_5 = 1,
    ITUR_BT2020 = 2,
    ITUR_BT601_5_FULL_RANGE = 3,
    JPEG_FULL_RANGE = 3
};

} // namespace image
} // namespace dimanche

#endif // __DIMANCHE_BASIC_COLOR_FORMATS_H
