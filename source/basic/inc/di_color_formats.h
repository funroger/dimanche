
#pragma once

#if !defined(__DI_COLOR_FORMATS_H)
#define __DI_COLOR_FORMATS_H

#include <di_types.h>

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

} // namespace image
} // namespace dimanche

#endif // __DI_COLOR_FORMATS_H