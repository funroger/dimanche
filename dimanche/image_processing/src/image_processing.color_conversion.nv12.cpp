// copyright (c) 2013 Victor Cherepanov
// distributed under BSD 3-clause license

#include <dimanche/image_processing/color_conversion.h>

#include <dimanche/system/cpu_opt.h>
#include <dimanche/image_processing/inc/color_conversion_internal.h>

namespace dimanche {

#if defined(_X86) || defined(_X64)
IMPL_PROC_2(di_convert_nv12_to_bgra, ssse3, avx2);
#else // !(defined(_X86) || defined(_X64))
IMPL_PROC_0(di_convert_nv12_to_bgra);
#endif // defined(_X86) || defined(_X64)

namespace color_conversion {

eResult NV12ToBGRA(const IMAGE &dst, const IMAGE &src) {
    // check error
    if ((eColorFormat::NV12 != src.format.colorFormat) ||
        (eColorFormat::BGRA != dst.format.colorFormat)) {
        return eResult::ERR_BAD_FORMAT;
    }
    if (dst.format.dim != src.format.dim) {
        return eResult::ERR_BAD_DIM;
    }
    if ((eResult::OK != image_processing::CheckFormat(dst.format)) ||
        (eResult::OK != image_processing::CheckFormat(src.format))) {
        return eResult::ERR_BAD_FORMAT;
    }

    di_convert_nv12_to_bgra(&dst, &src);

    return eResult::OK;

} // eResult NV12ToBGRA(const IMAGE &dst, const IMAGE &src)

} // namespace color_conversion
} // namespace dimanche
