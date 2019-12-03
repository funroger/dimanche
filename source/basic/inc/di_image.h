
#pragma once

#include <di_image_format.h>

#if !defined(__DI_IMAGE_H)
#define __DI_IMAGE_H

namespace dimanche {

class Image
{
    using namespace image;

public:
    // constructors
    Image(const Format && format) :
        m_format(format) {}

    // access data
    inline
    const Format &Format() const {return m_format;}
    inline
    Format Format() {return m_format;}

    // access plane data
    template <typename data_t>
    class Plane
    {
    public:
        // constructors
        Plane(void *p = nullptr) :
            m_p((data_t *) p) {}

        // data access operators
        inline
        data_t & operator [] (const ptrdiff_t idx) {return m_p[idx];}
        inline
        data_t & operator * () {return *m_p;}
        inline
        data_t * get() const {return m_p;}

        // advance operators
        inline
        Plane<data_t> & operator += (const ptrdiff_t advance) {
            m_p += advance;
            return *this
        }
        inline
        Plane<data_t> & operator -= (const ptrdiff_t advance) {
            m_p -= advance;
            return *this
        }
        inline
        void Advance(const ptrdiff_t byteAdvance) {
            m_p = (data_t *) (((uint8_t *) m_p) + byteAdvance);
        }

    private:
        data_t *m_p;
    };

    template <typename data_t> inline
    Plane<data_t> Plane(const size_t planeIdx) const {
        if (m_planes.size() <= planeIdx) {return Plane<data_t>();}
        return Plane<data_t> (m_planes[planeIdx].first);
    }
    inline
    ptrdiff_t Stride(const size_t planeIdx) const {
        if (m_planes.size() <= planeIdx) {return 0;}
        return m_planes[planeIdx].second;
    }

protected:
    Format m_format;
    std::array<std::pair<void *, ptrdiff_t>, 4> m_planes;
};

} // namespace dimanche

#endif // __DI_IMAGE_H
