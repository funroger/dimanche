
#pragma once

#if !defined(__DIMANCHE_BASIC_IMAGE_H)
#define __DIMANCHE_BASIC_IMAGE_H

#include <dimanche/basic/image_format.h>
#include <dimanche/basic/result.h>

#include <array>

namespace dimanche {
namespace image {

enum {
    MAX_NUM_PLANES = 4
};

#pragma pack(push, 1)
// the structure is used for calling C functions.
struct PLANE
{
    void *p;
    ptrdiff_t stride;
};
#pragma pack(pop)

} // namespace image

#pragma pack(push, 1)
// the structure is used for calling C functions.
// for C++ code class Image should be used
struct IMAGE
{
    image::FORMAT format;
    image::PLANE planes[image::MAX_NUM_PLANES];
};
#pragma pack(pop)

class Image
{
public:

    // access data
    virtual
    const image::Format & Format() const = 0;

    // lock image planes, prepare them for using.
    // return value is self object if everything is OK, otherwise nullptr.
    virtual
    Image * const Lock() = 0;

    // access C-style format
    virtual
    const IMAGE c_image() const = 0;

    // unlock the image planes, using has been completed
    virtual
    void Unlock() = 0;

    // release the image
    virtual
    void Release() = 0;
};

namespace image {

// advance a pointer (typically a plane pointer) on a stride
template <typename type_t>
type_t * Advance(type_t *p, const ptrdiff_t advance) {
    return (type_t *) (((uint8_t *) p) + advance);
}

} // namespace image
} // namespace dimanche

#endif // __DIMANCHE_BASIC_IMAGE_H
