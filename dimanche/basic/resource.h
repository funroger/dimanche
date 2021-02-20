
#pragma once

#if !defined(__DIMANCHE_BASIC_RESOURCE_H)
#define __DIMANCHE_BASIC_RESOURCE_H

namespace dimanche {
namespace resource {

struct Unlock
{
    template <typename type_t>
    void operator () (type_t * const p) const {
        p->Unlock();
    }
};

struct Release
{
    template <typename type_t>
    void operator () (type_t * const p) const {
        p->Release();
    }
};

} // namespace resource
} // namespace dimanche

#endif // !defined(__DIMANCHE_BASIC_RESOURCE_H)
