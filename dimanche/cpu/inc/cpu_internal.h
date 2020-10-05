
#pragma once

#if !defined(__DIMANCHE_CPU_CPU_INTERNAL_H)
#define __DIMANCHE_CPU_CPU_INTERNAL_H

#include <dimanche/cpu/cpu.h>

namespace dimanche {
namespace system {
namespace cpu {

eCpuType get_type();

#if defined(_X86) || defined(_X64)
extern "C"
eCpuType di_cpu_get_type_asm();
#endif // defined(_X86) || defined(_X64)

} // namespace cpu
} // namespace system
} // namespace dimanche

#endif // !defined(__DIMANCHE_CPU_CPU_INTERNAL_H)
