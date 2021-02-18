// copyright (c) 2013 Victor Cherepanov
// distributed under BSD 3-clause license

#pragma once

#if !defined(__DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_H)
#define __DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_H

#include <dimanche/basic/image.h>

namespace dimanche {
namespace color_conversion {

//
// general functions for color conversion
//

//
// functions for specific color conversion
//

eResult NV12ToBGRA(const IMAGE &dst, const IMAGE &src);

} // color_conversion
} // namespace dimanche

// declare the library name
#if defined(_WINDOWS)
#pragma comment (lib, "image_processing")
#endif // defined(_WINDOWS)

#endif // !defined(__DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_H)
