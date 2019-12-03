
#pragma once

#if !defined(__DI_IMAGE_FORMAT_H)
#define __DI_IMAGE_FORMAT_H

#include <di_color_format.h>
#include <di_types.h>

namespace dimanche {
namespace image {

class Format
{
public:
    // constructors
    Format(const Format && format) :
        m_colorFormat(format.ColorFormat()),
        m_dim(format.Dim()) {}

    // access data
    inline
    eColorFormat ColorFormat() {return m_colorFormat;}
    inline
    const dim_t &Dim() const {return m_dim;}
    inline
    dim_t Dim() {return m_dim;}

private:
    eColorFormat m_colorFormat = eColorFormat::NONE;
    dim_t m_dim = {0, 0};

};

} // namespace image
} // namespace dimanche

#endif // __DI_IMAGE_FORMAT_H
