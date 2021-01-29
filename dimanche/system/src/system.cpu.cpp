
#include <dimanche/system/cpu.h>

#include <dimanche/basic/intrinsics.h>
#include <dimanche/system/inc/cpu_internal.h>

#if defined(_X86) || defined(_X64)

IMPL_PROC_2(di_system_cpu_reset_state, mmx, avx);

#else // !(defined(_X86) || defined(_X64))

IMPL_PROC_0(di_system_cpu_reset_state);

#endif // defined(_X86) || defined(_X64)

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

extern "C"
void di_system_cpu_reset_state_c() {
  // do nothing in non-featured version
}

void ResetState(void)
{
    di_system_cpu_reset_state();
}

} // namespace cpu
} // namespace system
} // namespace dimanche
