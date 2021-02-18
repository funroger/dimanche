// copyright (c) 2013 Victor Cherepanov
// distributed under BSD 3-clause license

#pragma once

#if !defined(__DIMANCHE_SYSTEM_CPU_H)
#define __DIMANCHE_SYSTEM_CPU_H

#include <dimanche/basic/types.h>

namespace dimanche {
namespace system {
namespace cpu {

enum class eCpuType : uint32_t {
    // not featured CPU (ARM, Pentium, K5)
    plain = 0x0000000,
    // cpu vendor mask
    vendorMask = (uint32_t) 0xc0000000,

#if defined(_X86) || defined(_X64)
    // genuine Intel CPU
    Intel = 0x40000000,
    // authentic AMD CPU
    AMD = 0x80000000,

    // CMOV technology featured CPU (Pentium Pro)
    cmov = (1 << 0),
    // MMX technology featured CPU (Pentium MMX)
    mmx = (1 << 1),
    // MMP technology featured CPU (classic Athlon)
    mmp = (1 << 2) | cmov | mmx,
    // SSE technology featured CPU (Pentium III, Athlon XP)
    sse = (1 << 3) | mmp,
    // SSE2 technology featured CPU (Pentium 4, Athlon 64)
    sse2 = (1 << 4) | sse,
    // SSE3 technology featured CPU (Pentium 4 Prescott, Athlon 64 Venice)
    sse3 = (1 << 5) | sse2,
    // supplemental SSE3 technology featured CPU (Core 2 Conroe)
    ssse3 = (1 << 6) | sse3,
    // FMA technology featured CPU (Core iX Haswell or Broadwell)
    fma = (1 << 7),
    // SSE4.1 technology featured CPU (Core 2 Duo Penryn)
    sse41 = (1 << 8) | ssse3,
    // SSE4.2 technology featured CPU (Core iX Nehalem)
    sse42 = (1 << 9) | sse41,
    // advanced bit manipulation featured CPU (AMD Barcelona core)
    abm = (1 << 10),
    // AES technology featured CPU (Core iX Clarkdale, except some i3)
    aes = (1 << 11),
    // AVX technology featured CPU (Core iX SandyBridge or IvyBridge)
    avx = (1 << 12) | sse42,
    // RAND technology featured CPU (Core iX IvyBridge)
    rdrand = (1 << 13),
    // bit manipulation instruction set featured CPU (Core iX Haswell or Broadwell)
    bmi = (1 << 14) | abm,
    // AVX2 technology featured CPU (Core iX Haswell or Broadwell)
    avx2 = (1 << 15) | avx,
    // bit manipulation instruction set 2 (Core iX Haswell or Broadwell)
    bmi2 = (1 << 16) | bmi,
    // enhanced rep movsb/stosb (Core iX SandyBridge)
    erms = (1 << 17),
    // AVX512 foundation technology featured CPU (Core iX Skylake or Cannonlake)
    avx512f = (1 << 18) | avx2,
    // AVX512 vector length extensions featured CPU (Core iX Skylake or Cannonlake)
    avx512vl = (1 << 19) | avx512f,
    // AVX512 byte & word technology featured CPU (Core iX Skylake or Cannonlake)
    avx512bw = (1 << 20) | avx512vl,
    // AVX512 dword & qword technology featured CPU (Core iX Skylake or Cannonlake)
    avx512dq = (1 << 21) | avx512vl,
    // maximum support tech id is (1 << 29)

    // declare custom feature combinations

    // CPU with SSE 4.1 technology (Core 2 Duo)
    penryn = Intel | sse41,
    // AVX512 f/vl/bw/dq technologies featured CPU (Core iX Skylake or Cannonlake)
    avx512bwdq = avx512dq | avx512bw,

#elif defined(_ARM)

    neon = (1 << 0),
    aes = (1 << 1),
    crc32 = (1 << 2)

#elif defined(_ARM64)

    neon = (1 << 0),
    aes = (1 << 1),
    crc32 = (1 << 2),
    asimd = (1 << 3) | neon

#endif // defined(_X86) || defined(_X64)

};

inline
eCpuType operator & (eCpuType left, const eCpuType right) {
    return (eCpuType) ((uint32_t) left & (uint32_t) right);
}

inline
eCpuType operator | (eCpuType left, const eCpuType right) {
    return (eCpuType) ((uint32_t) left | (uint32_t) right);
}

eCpuType GetType(void);
void SetType(const eCpuType requiredCpuType);
void ResetState(void);

} // namespace cpu
} // namespace system
} // namespace dimanche

// declare the library name
#if defined(_WINDOWS)
#pragma comment (lib, "system")
#endif // defined(_WINDOWS)

#endif // !defined(__DIMANCHE_SYSTEM_CPU_H)
