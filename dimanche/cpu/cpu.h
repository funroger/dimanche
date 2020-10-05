
#pragma once

#if !defined(__DIMANCHE_CPU_CPU_H)
#define __DIMANCHE_CPU_CPU_H

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
    Intel = 0x00000000,
    // authentic AMD CPU
    AMD = 0x40000000,

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
    // Supplemental SSE3 technology featured CPU (Core 2 Conroe)
    sse3s = (1 << 6) | sse3,
    // FMA technology featured CPU (Core iX Haswell or Broadwell)
    fma = (1 << 7),
    // SSE4.1 technology featured CPU (Core 2 Duo Penryn)
    sse41 = (1 << 8) | sse3s,
    // SSE4.2 technology featured CPU (Core iX Nehalem)
    sse42 = (1 << 9) | sse41,
    // Advanced bit manipulation featured CPU (AMD Barcelona core)
    abm = (1 << 10),
    // AES technology featured CPU (Core iX Clarkdale, except some i3)
    aes = (1 << 11),
    // AVX technology featured CPU (Core iX SandyBridge or IvyBridge)
    avx = (1 << 12) | sse42,
    // AVX technology featured CPU (Core iX IvyBridge)
    rdrand = (1 << 13),
    // Bit manipulation instruction set featured CPU (Core iX Haswell or Broadwell)
    bmi = (1 << 14) | abm,
    // AVX2 technology featured CPU (Core iX Haswell or Broadwell)
    avx2 = (1 << 15) | avx,
    // Bit manipulation instruction set 2 (Core iX Haswell or Broadwell)
    bmi2 = (1 << 16) | bmi,
    // Enhanced rep movsb/stosb (Core iX SandyBridge)
    erms = (1 << 17),
    // AVX512 foundation technology featured CPU (Core iX Skylake or Cannonlake)
    avx512f = (1 << 18) | avx2,
    // AVX512 byte & word technology featured CPU (Core iX Skylake or Cannonlake)
    avx512bw = (1 << 19) | avx512f,
    // AVX512 dword & qword technology featured CPU (Core iX Skylake or Cannonlake)
    avx512dq = (1 << 20) | avx512bw,
    // maximum support tech id is (1 << 23)

    // CPU with SSE technology (Pentium III)
    katmai = Intel | sse,
    // CPU with SSE 2 technology (Pentium 4)
    willamette = Intel | sse2,
    // CPU with SSE 3 technology (Pentium 4 Prescott)
    prescott = Intel | sse3,
    // CPU with supplemental SSE 3 technology (Core Duo)
    merom = Intel | sse3s,
    // CPU with SSE 4.1 technology (Core 2 Duo)
    penryn = Intel | sse41,
    // CPU with SSE 4.2 technology (Core i7)
    nehalem = Intel | sse42,
    // CPU with AVX instruction set (Core i7)
    sandybridge = Intel | avx | erms,
    // CPU with AVX & RDRAND instruction set (Core i7)
    ivybridge = Intel | avx | rdrand | erms,
    // CPU with AVX2 instruction set (Core i7)
    haswell = Intel | avx2 | fma | rdrand | bmi2 | erms,
    // CPU with AVX512F instruction set (Core i7)
    skylake = Intel | avx512f | fma | rdrand | bmi2 | erms,
    // CPU with AVX512(F, BW, DQ) instruction set (Core i7)
    cannonlake = Intel | avx512dq | fma | rdrand | bmi2 | erms,

    // CPU with MMP technology (classic Athlon)
    athlon = AMD | mmp,
    // CPU with SSE technology (Athlon XP)
    palomino = AMD | sse,
    // CPU with SSE 2 technology (Athlon 64, Opteron)
    clawhammer = AMD | sse2,
    // CPU with SSE 3 technology (Athlon 64 Venice)
    venice = AMD | sse3,
    // CPU with SSE 3 & ABM technology (Athlon 64 Barcelona)
    barcelona = AMD | sse3 | abm,
    // CPU with AVX & FMA technology (Athlon 64 Bulldozer)
    bulldozer = AMD | avx | fma | abm,
    // CPU with AVX & BMI & FMA technology (Athlon 64 Piledriver)
    piledriver = AMD | avx | fma | bmi,
    // CPU with AVX2 & BMI2 & FMA technology (Ryzen 3/5/7)
    zen = AMD | avx2 | fma | rdrand | bmi2

#elif defined(_ARM)

    neon = (1 << 0),
    aes = (1 << 1),
    crc32 = (1 << 2)

#elif defined(_ARM64)

    neon = (1 << 0),
    asimd = (1 << 1) | neon,
    aes = (1 << 2),
    crc32 = (1 << 3)

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

#endif // !defined(__DIMANCHE_CPU_CPU_H)
