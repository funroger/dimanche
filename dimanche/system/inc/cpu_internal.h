
#pragma once

#if !defined(__DIMANCHE_SYSTEM_CPU_INTERNAL_H)
#define __DIMANCHE_SYSTEM_CPU_INTERNAL_H

#include <dimanche/system/cpu.h>
#include <dimanche/system/cpu_opt.h>

namespace dimanche {
namespace system {
namespace cpu {

eCpuType get_type();

#if defined(_X86) || defined(_X64)
extern "C"
eCpuType di_system_cpu_get_type_asm();
#endif // defined(_X86) || defined(_X64)

} // namespace cpu
} // namespace system
} // namespace dimanche

DECL_PROC(void, di_system_cpu_reset_state, (), ())

#endif // !defined(__DIMANCHE_SYSTEM_CPU_INTERNAL_H)
