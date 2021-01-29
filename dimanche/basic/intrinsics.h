// copyright (c) 2013 Victor Cherepanov
// distributed under BSD 3-clause license

#pragma once

#if !defined(__DIMANCHE_BASIC_INTRINSICS_H)
#define __DIMANCHE_BASIC_INTRINSICS_H

#include <dimanche/basic/platform.h>

#if defined(_X86) || defined(_X64)

#if defined(_MSC_VER)
/* disable warning 'found Intel(R) Advanced Vector Extensions; consider using /arch:AVX' */
#pragma warning(disable : 4752)
#endif /* defined(_MSC_VER) */

/*
    x86 platform
*/

#if defined(_WINDOWS)
#include <intrin.h>
#elif defined(_LINUX)
#include <x86intrin.h>
#else /* unknown platform */
#error PLATFORM IS NOT SET
#endif /* defined(_WINDOWS) */

#elif defined(_ARM) || defined(_ARM64)

/*
    ARM platform
*/

#include <arm_neon.h>

/* declare unaligned pointers */
typedef int16_t __attribute__ ((__aligned__(1))) unaligned_int16_t;
typedef int32_t __attribute__ ((__aligned__(1))) unaligned_int32_t;
typedef int64_t __attribute__ ((__aligned__(1))) unaligned_int64_t;

typedef uint16_t __attribute__ ((__aligned__(1))) unaligned_uint16_t;
typedef uint32_t __attribute__ ((__aligned__(1))) unaligned_uint32_t;
typedef uint64_t __attribute__ ((__aligned__(1))) unaligned_uint64_t;

#endif /* defined(_X86) || defined(_X64) */

#endif /* !defined(__DIMANCHE_BASIC_INTRINSICS_H) */
