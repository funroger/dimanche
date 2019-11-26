
#pragma once

#if !defined(__DI_TYPES_H)
#define __DI_TYPES_H

#include <cstddef>
#include <cstdint>
#include <utility>

namespace dimanche {

template <typename parameter_t>
class Dimension
{
public:

    using data_t = parameter_t;

    // constructors
    Dimension() = default;
    Dimension(const data_t width, const data_t height) :
        m_width(width),
        m_height(height) {}
    Dimension(const Dimension && right) :
        m_width(right.Width()),
        m_height(right.Height()) {}

    // assign operator
    inline
    Dimension<data_t> & operator = (const Dimension<data_t> && right) {
        m_width = right.Width();
        m_height = right.Height();
        return *this;
    }

    // cast operator
    template <typename cast_t>
    operator Dimension<cast_t> () const {
        return Dimension<cast_t> ((cast_t) Width(), (cast_t) Height());
    }

    // compare operators
    inline
    bool operator == (const MotionVector<data_t> && right) const {
        return ((Width() == right.Width()) && (Height() == right.Height()));
    }
    inline
    bool operator != (const MotionVector<data_t> && right) const {
        return !operator == (right);
    }

    // access data
    inline
    data_t Width() const {return m_width;}
    inline
    data_t Height() const {return m_height;}

private:
    data_t m_width;
    data_t m_height;
};
using dim_t = Dimension<uint32_t>;


template <typename parameter_t>
class MotionVector
{
public:

    using data_t = parameter_t;

    // constructors
    MotionVector() = default;
    MotionVector(const data_t x, const data_t y) :
        m_x(x),
        m_y(y) {}
    MotionVector(const MotionVector && right) :
        m_x(right.x()),
        m_y(right.y()) {}

    // assign operator
    inline
    MotionVector & operator = (const MotionVector && right) {
        m_x = right.x();
        m_y = right.y();
        return *this;
    }

    // cast operator
    template <typename cast_t>
    operator MotionVector<cast_t> () const {
        return MotionVector<cast_t> ((cast_t) x(), (cast_t) y());
    }

    // arithmetic operators
    inline
    MotionVector<data_t> & operator += (const MotionVector<data_t> && right) {
        m_x += right.x();
        m_y += right.y();
        return *this;
    }
    inline
    MotionVector<data_t> & operator -= (const MotionVector<data_t> && right) {
        m_x -= right.x();
        m_y -= right.y();
        return *this;
    }

    // compare operators
    inline
    bool operator == (const MotionVector<data_t> && right) const {
        return ((x() == right.x()) && (y() == right.y()));
    }
    inline
    bool operator != (const MotionVector<data_t> && right) const {
        return !operator == (right);
    }

    // access data
    inline
    data_t x() const {return m_x;}
    inline
    data_t y() const {return m_y;}

private:
    data_t m_x;
    data_t m_y;

};
using mv_t = MotionVector<int32_t>;

// arithmetic operators
template <typename data_t, typename result_t = decltype(std::declval<data_t>() + std::declval<data_t>())> inline
MotionVector<result_t>  operator + (const MotionVector<data_t> && left,
    const MotionVector<data_t> && right) {
    return MotionVector<result_t> (left.x() + right.x(), left.y() + right.y());
}
template <typename data_t, typename result_t = decltype(std::declval<data_t>() + std::declval<data_t>())> inline
MotionVector<result_t>  operator - (const MotionVector<data_t> && left,
    const MotionVector<data_t> && right) {
    return MotionVector<result_t> (left.x() - right.x(), left.y() - right.y());
}

} // namespace dimanche

#endif // __DI_TYPES_H
