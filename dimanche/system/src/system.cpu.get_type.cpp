
#include <dimanche/system/cpu.h>

#include <dimanche/system/inc/cpu_internal.h>

namespace dimanche {
namespace system {
namespace cpu {

inline
eCpuType operator |= (eCpuType &left, const eCpuType right) {
    return left = (eCpuType) ((uint32_t) left | (uint32_t) right);
}

eCpuType get_type(void)
{
    eCpuType cpuType = eCpuType::plain;

#if defined(_X86) || defined(_X64)

    cpuType = di_system_cpu_get_type_asm();

#elif defined(_ARM) || defined(_ARM64)

#if defined(__ANDROID__)

    const AndroidCpuFamily family = android_getCpuFamily();
    const AndroidCpuFeatures cpuFeatures = android_getCpuFeatures();

    switch (family) {
#if defined(_ARM64)
    case ANDROID_CPU_FAMILY_ARM64:
        if (cpuFeatures() & ANDROID_CPU_ARM64_FEATURE_ASIMD) {
            cpuType |= cpuType::asimd;
        }
#elif defined(_ARM)
    case ANDROID_CPU_FAMILY_ARM:
        if (cpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) {
            cpu = eCpuType::neon;
        }
#endif // defined(_ARM64)
        if (cpuFeatures() & ANDROID_CPU_ARM64_FEATURE_AES) {
            cpuType |= cpuType::aes;
        }
        if (cpuFeatures() & ANDROID_CPU_ARM64_FEATURE_CRC32) {
            cpuType |= cpuType::crc32;
        }
        break;

        // unkown family
    default:
        break;
    }

#elif defined(__APPLE__)

    cpuType = eCpu::neon;

#if defined(_ARM64)

    cpuType |= eCpu::asimd | eCPU::aes | eCPU::crc32;

#endif // defined(_ARM64)

#endif // defined(__ANDROID__)

#endif // defined(_X86) || defined(_X64)

    return cpuType;

} // eCpuType get_type(void)

} // namespace cpu
} // namespace system
} // namespace dimanche
