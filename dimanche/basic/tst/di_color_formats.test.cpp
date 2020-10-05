
#include <di_color_formats.h>

#include <array>
#include <di_result.h>

namespace dimanche {
namespace image {
namespace {

inline
eColorFormat & operator ++(eColorFormat &colorFormat) {
    return colorFormat = (eColorFormat) ((uint32_t) colorFormat + 1);
}

struct COLOR_ID {
    eColorFormat colorFormat;
    uint32_t value;
};

std::array<COLOR_ID,  (size_t) eColorFormat::NUM> ColorIds = {
    {eColorFormat::NONE, 0},
    {eColorFormat::BGR, 1},
    {eColorFormat::BGRA, 2},
    {eColorFormat::UYVY, 3},
    {eColorFormat::YUY2, 4},
    {eColorFormat::YVYU, 5},
    {eColorFormat::NV12, 6}
};

eResult TestColorFormats() {

    if (ColorIds.size() < (size_t) eColorFormat::NUM) {
        return eResult::ERR_FAILED;
    }

    for (eColorFormat colorFormat; colorFormat < eColorFormat::NUM; ++colorFormat) {
        const auto colorId = ColorIds[(size_t) colorFormat];
        if ((colorId.colorFormat != colorFormat) ||
            (colorId.value != (uint32_t) colorFormat)) {
            return eResult::ERR_FAILED;
        }
    }
}

} // namespace
} // namespace image
} // namespace dimanche
