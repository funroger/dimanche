
#pragma once

#if !defined(__DIMANCHE_BASIC_TEMPLATES_H)
#define __DIMANCHE_BASIC_TEMPLATES_H

#include <dimanche/basic/types.h>

#include <algorithm>

namespace dimanche {

enum class eLog2Alignment : uint32_t {
    DEFAULT = 6, // 64 bytes
    MAX = 10 // 1024 * 1024
};

template <typename align_t, typename value_t> inline
value_t Align(const value_t value,
    const uint32_t log2alignment = (uint32_t) eLog2Alignment::DEFAULT)
{
    const int32_t alignment = (1l << log2alignment);
    return (value + (alignment - 1)) &
        ((value_t) 0 - (value_t) alignment);
}

inline
ptrdiff_t PtrToInt(const void * const p)
{
    return (((const uint8_t * const) p) - ((const uint8_t * const) 0));
}

template <typename ptr_t> inline
ptr_t * Align(ptr_t * const pv,
    const uint32_t log2alignment = (uint32_t) eLog2Alignment::DEFAULT)
{
    const ptrdiff_t alignment = ((ptrdiff_t) 1 << log2alignment);
    const uint8_t *pb = (const uint8_t *) pv;

    return (ptr_t *) (pb + ((alignment - PtrToInt(pb)) & (alignment - 1)));
}

template<typename type_t> inline
bool IsAligned(const type_t value,
    const uint32_t log2alignment = (uint32_t) eLog2Alignment::DEFAULT)
{
    const int32_t alignment = (1l << log2alignment);
    return (0 == (value & (alignment - 1)));
}

template<typename ptr_t> inline
bool IsAligned(ptr_t * const pv,
    const uint32_t alignment = (uint32_t) eLog2Alignment::DEFAULT)
{
    return IsAligned(PtrToInt(pv), alignment);
}

template <typename data_t> inline
data_t CheckAlignment(data_t alignment)
{
    data_t a = alignment;

    // saturate the align value
    a = saturate((data_t) 1, (1u << (uint32_t) eLog2Alignment::MAX), a);
    // make sure that the align value is a power of 2
    return (0 == (a & (a - 1))) ? (a) : (1u << (uint32_t) eLog2Alignment::DEFAULT);

} // data_t check_alignment(data_t alignment)

template <typename dst_t, typename src_t> inline
dst_t saturate(const dst_t min, const dst_t max, const src_t value) {
    return (dst_t) std::min((src_t) max, std::max((src_t) min, value));
}

template <typename src_t> inline
uint8_t to_byte(const src_t value) {
    return saturate<uint8_t> (0, 255, value);
}

} // namespace dimanche

#endif // __DIMANCHE_BASIC_TEMPLATES_H
