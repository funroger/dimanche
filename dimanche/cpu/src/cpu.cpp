
#include <dimanche/cpu/cpu.h>
#include <dimanche/cpu/intrinsics.h>
#include <dimanche/cpu/inc/cpu_internal.h>

namespace dimanche {
namespace system {
namespace cpu {

namespace {

enum
{
    UNKNOWN = -1
};

eCpuType cpuType = (eCpuType) UNKNOWN;

} // namespace

eCpuType GetType(void)
{
    // update the CPU type
    if ((eCpuType) UNKNOWN == cpuType) {
        cpuType = get_type();
    }

    return cpuType;
}

void SetType(const eCpuType requiredCpuType)
{
    cpuType = requiredCpuType;
}

void ResetState(void)
{
#if defined(_X86) || defined(_X64)
/*
    const auto cpu = GetType();

    if ((uint32_t) (eCpuType::mmx & cpu)) {
        _mm_empty();
    }
    if ((uint32_t) (eCpuType::avx & cpu)) {
        _mm256_zeroupper();
    }*/

#endif // defined(_X86) || defined(_X64)

} // void ResetState(void)

} // namespace cpu
} // namespace system
} // namespace dimanche
