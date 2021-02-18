
#pragma once

#if !defined(__DIMANCHE_BASIC_IMAGE_H)
#define __DIMANCHE_BASIC_IMAGE_H

#include <dimanche/basic/color_formats.h>
#include <dimanche/basic/result.h>
#include <dimanche/basic/types.h>

#include <array>

namespace dimanche {

// namespace image contains various types and constants that are used
// for image management
namespace image {

enum {
    MAX_NUM_PLANES = 4
};

#pragma pack(push, 1)
// the structures are used for calling C functions only.
// for C++ code class Image/Format should be used.
// following structures have mirror declarations at image.x86.inc file.
// change them accordingly.
struct FORMAT
{
    eColorFormat colorFormat;
    dim_t dim;
    eColorSpace colorSpace;
    uint8_t bitDepthMinus8;
    uint8_t bitDepthChromaMinus8;
};

struct PLANE
{
    void *p;
    ptrdiff_t stride;
};

} // namespace image

struct IMAGE
{
    image::FORMAT format;
    image::PLANE planes[image::MAX_NUM_PLANES];
};
#pragma pack(pop)

namespace image {

class Format
{
public:
    // constructors
    Format() = default;
    Format(const Format & right) {
        operator = (right);}

    // assignment operators
    inline
    Format & operator = (const Format & right) {
        m_format = right.c_format();
        return *this;
    }

    // access data
    inline
    eColorFormat ColorFormat() const {return m_format.colorFormat;}
    inline
    const dim_t &Dim() const {return m_format.dim;}
    inline
    eColorSpace ColorSpace() const {return m_format.colorSpace;}

    // access C-style format
    inline
    const FORMAT &c_format() const {return m_format;}

    //
    // service methods
    //

    // get the number of planes
    size_t NumPlanes() const;
    // get plane bit depth
    uint32_t BitDepthMinus8(size_t planeIdx = 0) const;

    // reset the format
    void Close();
    // check the format
    eResult IsValid() const {return eResult::OK;}

private:
    FORMAT m_format = {};
};

} // namespace image

class IImage
{
public:

    class CLock;

    // access format
    virtual
    const image::Format & Format() const = 0;

    // lock image planes, prepare them for using.
    virtual
    CLock Lock() = 0;

    virtual
    void *Plane(const size_t planeIdx) const = 0;

    template<typename data_t> inline
    data_t *Plane(const size_t planeIdx) const {
        return (data_t *) Plane(planeIdx);
    }

    virtual
    ptrdiff_t Stride(const size_t planeIdx) const = 0;

    // access C-style format
    virtual
    const IMAGE c_image() const = 0;

private:
    // unlock the image planes, using has been completed
    virtual
    void Unlock() = 0;
};

class IImage::CLock
{
public:
    CLock(IImage * const pImage) :
        m_pImage(pImage) {}
    ~CLock() {
        if (m_pImage) {
            m_pImage->Unlock();
        }
    }
private:
    IImage * const m_pImage;
};

namespace image {

// advance a pointer (typically a plane pointer) on a stride
template <typename type_t>
type_t * Advance(type_t *p, const ptrdiff_t advance) {
    return (type_t *) (((uint8_t *) p) + advance);
}

} // namespace image
} // namespace dimanche

// declare the library name
#if defined(_WINDOWS)
#pragma comment (lib, "basic")
#endif // defined(_WINDOWS)

#endif // __DIMANCHE_BASIC_IMAGE_H
