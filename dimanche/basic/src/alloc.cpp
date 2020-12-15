
#include <dimanche/basic/alloc.h>

#include <dimanche/basic/assert.h>
#include <dimanche/basic/templates.h>

#include <chrono>
#include <map>
#include <mutex>
#include <unordered_map>

namespace dimanche {
namespace memory {

enum
{
    ALLOCATION_GUARD = 1234567890
};

#pragma pack(1)
typedef
struct __ALLOC_GUARD_STRUCT
{
    // magic guard value
    size_t guard0;
    // real allocated pointer
    uint8_t *pAllocated;
    // allocated data size
    size_t dataSize;
    // allocated buffer size
    size_t allocSize;
    // magic guard value
    size_t guard1;

} __ALLOC_GUARD_STRUCT;
#pragma pack()

void *aligned_alloc(const size_t size, const uint32_t log2alignment)
{
    __ALLOC_GUARD_STRUCT allocInfo;
    uint8_t *pToReturn;
    const size_t ptrSize = sizeof(size_t);
    const uint32_t log2align = saturate((uint32_t) eLog2Alignment::DEFAULT,
        (uint32_t) eLog2Alignment::MAX, log2alignment);
    const uint32_t align = 1 << log2align;

    // get number of bytes to allocate
    allocInfo.guard0 = ALLOCATION_GUARD;
    allocInfo.dataSize = Align<size_t> (size, ptrSize);
    allocInfo.allocSize = allocInfo.dataSize + align +
        sizeof(__ALLOC_GUARD_STRUCT) + ptrSize;
    allocInfo.guard1 = ALLOCATION_GUARD;

    // allocate the memory
    allocInfo.pAllocated = new (std::nothrow) uint8_t[allocInfo.allocSize];
    if (nullptr == allocInfo.pAllocated) {
        return nullptr;
    }

    // align the pointer to return
    pToReturn = allocInfo.pAllocated + sizeof(__ALLOC_GUARD_STRUCT);
    pToReturn = Align(pToReturn, log2align);

    // save the real pointer
    *(((__ALLOC_GUARD_STRUCT *) pToReturn) - 1) = allocInfo;
    *((size_t *) (pToReturn + allocInfo.dataSize)) = ALLOCATION_GUARD;

    return pToReturn;

} // void *aligned_alloc(const size_t size, const size_t align_to)

void aligned_free(void * const p)
{
    // nothing to do
    if (nullptr == p) {
        return;
    }

    try {
        // get allocation info
        __ALLOC_GUARD_STRUCT &allocInfo = *(((__ALLOC_GUARD_STRUCT *) p) - 1);

        // check error(s)
        if ((ALLOCATION_GUARD != allocInfo.guard0) ||
            (ALLOCATION_GUARD != allocInfo.guard1) ||
            (ALLOCATION_GUARD != *((size_t *) (((uint8_t *) p) + allocInfo.dataSize)))) {
            di_assert(false);
        }

        delete [] allocInfo.pAllocated;
    } catch (...) {
        // there is something went wrong
        di_assert(false);
    }

} // void aligned_free(void *p)

// retirement may be off to save some clocks.
// in real life it slows down execution a bit, but keeps the heap from growing.
#define RETIRE 0

class CBasicAllocator : public IAllocator
{
public:
    CBasicAllocator() = default;
    virtual
    ~CBasicAllocator() = default;

    virtual
    std::unique_ptr<void, deleter_t> Alloc(const size_t size,
        const uint32_t log2Alignment = (uint32_t) eLog2Alignment::DEFAULT) override {
        std::unique_lock<std::mutex> lock(m_guard);

        // try to re-use a memory region
        for (auto iter = m_free.lower_bound(size); iter != m_free.end(); ++iter) {
            const size_t memSize = iter->first;
            MEM_REGION &mem = *(iter->second);

            // available pieces are too big (allow only 1/8 bigger)
            if (memSize * 8 > size * 9) {
                break;
            }

            if (IsAligned(mem.get(), log2Alignment)) {

                mem.State(MEM_REGION::eState::IN_USE);
                m_free.erase(iter);
#if RETIRE
                auto timeIter = m_accessTimes.lower_bound(mem.LastAccessTime());
                while (m_accessTimes.end() != timeIter) {
                    if (&mem == timeIter->second) {
                        m_accessTimes.erase(timeIter);
                        break;
                    }
                    ++timeIter;
                }
#endif // RETIRE
                return std::unique_ptr<void, deleter_t> (mem.get(),
                    [=](void *p ){this->Free(p);});
            }
        }

        // need to allocate one more memory region
        {
            mem_region_t mem(memory::Alloc<uint8_t> (size, log2Alignment), size);

            auto p = mem.get();
            if (nullptr == p) {
                return {};
            }
            mem.State(MEM_REGION::eState::IN_USE);

            m_all.insert({p, std::move(mem)});

            return std::unique_ptr<void, deleter_t> (p,
                [=](void *p ){this->Free(p);});
        }
    }

    void Free(void * const p) {
        if (nullptr == p) {
            return;
        }

        std::unique_lock<std::mutex> lock(m_guard);

        auto iter = m_all.find(p);
        if (m_all.end() == iter) {
            return;
        }
        auto &mem = iter->second;
        // already freed
        if (MEM_REGION::eState::FREE == mem.State()) {
            return;
        }
        mem.State(MEM_REGION::eState::FREE);

        // update containers
        const auto size = mem.Size();
        m_free.insert({size, &mem});
#if RETIRE
        const auto curTime = timer_t::now();
        const auto lastAccessTime = mem.LastAccessTime();

        mem.LastAccessTime(curTime);
        m_accessTimes.insert({curTime, &mem});

        Retire();
#endif // RETIRE
    }

private:

#if RETIRE
    using timer_t = std::chrono::high_resolution_clock;

    constexpr static std::chrono::duration<double> retireTime {1.0};
#endif // RETIRE

    struct MEM_REGION {
        enum class eState : uint32_t {
            FREE = 0,
            IN_USE = 1
        };

        MEM_REGION() {}
        MEM_REGION(std::unique_ptr<void, deallocator> p, const size_t size) :
            m_p(std::move(p)),
            m_size(size) {}

        inline
        void * get() const {return m_p.get();}
        inline
        size_t Size() const {return m_size;}

#if RETIRE
        inline
        void LastAccessTime(const timer_t::time_point time) {m_lastAccessTime = time;}
        inline
        timer_t::time_point LastAccessTime() const {return m_lastAccessTime;}
#endif // RETIRE

        inline
        void State(const eState state) {m_state = state;}
        inline
        eState State() const {return m_state;}

    private:
        std::unique_ptr<void, deallocator> m_p;
        size_t m_size = 0;
#if RETIRE
        timer_t::time_point m_lastAccessTime;
#endif // RETIRE
        eState m_state = eState::FREE;
    };

#if RETIRE
    void Retire() {
        const auto curTime = timer_t::now();
        auto oldest = m_accessTimes.begin();

        const auto oldestAccessTime = oldest->first;
        const auto &memOldest = *(oldest->second);

         // remove only one item at a time

        const std::chrono::duration<double> timeSinceLastAccess(curTime - oldestAccessTime);
        if (retireTime < timeSinceLastAccess) {
            auto iter = m_free.lower_bound(memOldest.Size());
            while (memOldest.Size() == iter->first) {
                if (iter->second == &memOldest) {
                    m_free.erase(iter);
                    break;
                }
            }
            m_accessTimes.erase(oldest);
            m_all.erase(memOldest.get());
        }

    } // void Retire()
#endif // RETIRE

    using mem_region_t = MEM_REGION;

    std::mutex m_guard;

    // free memory pieces
    std::multimap<size_t, mem_region_t *> m_free;
#if RETIRE
    // access timestamps
    std::multimap<timer_t::time_point, mem_region_t *> m_accessTimes;
#endif // RETIRE
    // all memory pieces
    std::unordered_map<void *, mem_region_t> m_all;
};

std::shared_ptr<IAllocator> CreateAllocator()
{
    std::shared_ptr<IAllocator> allocator(new (std::nothrow) CBasicAllocator(),
        [](CBasicAllocator * const p) {delete p;});

    return allocator;
}

} // memory
} // namespace dimanche
