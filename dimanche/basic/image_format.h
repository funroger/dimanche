
#pragma once

#if !defined(__DIMANCHE_BASIC_IMAGE_FORMAT_H)
#define __DIMANCHE_BASIC_IMAGE_FORMAT_H

#include <dimanche/basic/color_formats.h>
#include <dimanche/basic/result.h>
#include <dimanche/basic/types.h>

#if defined(__cplusplus)

namespace dimanche {
namespace image {

#pragma pack(push, 1)
// the structure is used for calling C functions.
// for C++ code class Format should be used
struct FORMAT
{
    eColorFormat colorFormat;
    dim_t dim;
    uint8_t bitDepthMinus8;
    uint8_t bitDepthChromaMinus8;
    eColorSpace colorSpace;
};
#pragma pack(pop)

class Format
{
public:
    // constructors
    Format() = default;
    Format(const Format & right) {
        operator = (right);}
    Format(const Format && right) {
        operator = (right);}

    // assignment operators
    inline
    Format & operator = (const Format & right) {
        return operator = ((const Format &&) right);
    }
    inline
    Format & operator = (const Format && right) {
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

    // reset the format
    void Close();
    // check the format
    eResult IsValid() const {return eResult::OK;}
    // get the number of planes
    size_t NumPlanes() const;

private:
    FORMAT m_format = {};

};

inline
bool operator == (const Format &left, const Format &right) {
    return ((left.ColorFormat() == right.ColorFormat()) &&
        (left.Dim() == right.Dim()) &&
        (left.ColorSpace() == right.ColorSpace()));
}

inline
bool operator != (const Format &left, const Format &right) {
    return !(left == right);
}

} // namespace image
} // namespace dimanche

#endif // defined(__cplusplus)

#endif // __DIMANCHE_BASIC_IMAGE_FORMAT_H
