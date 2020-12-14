
#pragma once

#if !defined(__DIMANCHE_BASIC_ALLOC_H)
#define __DIMANCHE_BASIC_ALLOC_H

#include <dimanche/basic/templates.h>

#include <functional>
#include <memory>

namespace dimanche {
namespace memory {

// allocate an aligned region of memory
void *aligned_alloc(const size_t size,
    const uint32_t log2alignment = (uint32_t) eLog2Alignment::DEFAULT);
// free an aligned region of memory
void aligned_free(void * const p);

struct deallocator
{
    template <typename type_t>
    void operator () (type_t * const p) const {
        aligned_free(p);
    }
};

// allocate a buffer of given type
template<typename data_t> inline
std::unique_ptr<data_t, deallocator> Alloc(const size_t size,
    const uint32_t log2alignment = (uint32_t) eLog2Alignment::DEFAULT)
{
    std::unique_ptr<data_t, deallocator> p((data_t *) aligned_alloc(size * sizeof(data_t),
        log2alignment));

    // call constructors
    new (p.get()) data_t[size];

    return p;
}

// basic allocator interface
class IAllocator
{
public:
    using deleter_t = std::function<void(void *)>;

    virtual
    std::unique_ptr<void, deleter_t> Alloc(const size_t size,
        const uint32_t log2Alignment = (uint32_t) eLog2Alignment::DEFAULT) = 0;

    template<typename data_t> inline
    std::unique_ptr<data_t, deleter_t> Alloc(const size_t size,
        const uint32_t log2Alignment = (uint32_t) eLog2Alignment::DEFAULT)
    {
        auto allocated = Alloc(size * sizeof(data_t), log2Alignment);
        std::unique_ptr<data_t, deleter_t> p((data_t *) allocated.get(),
            allocated.get_deleter());
        allocated.release();

        // call constructors
        new (p.get()) data_t[size];

        return p;
    }
};

std::shared_ptr<IAllocator> CreateAllocator();

} // memory
} // namespace dimanche

#endif // !defined(__DIMANCHE_BASIC_ALLOC_H)
