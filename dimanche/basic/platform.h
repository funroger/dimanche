
#pragma once

#if !defined(__DIMANCHE_BASIC_PLATFORM_H)
#define __DIMANCHE_BASIC_PLATFORM_H

/* define the OS identifier */
#if defined(_WIN32) || defined(_WIN64)

#ifndef _WINDOWS
#define _WINDOWS
#endif /* _WINDOWS */

#elif defined(__linux__)

#ifndef _LINUX
#define _LINUX
#endif /* _LINUX */

#else /* unknown platform */

#pragma message("ERROR: specify the platform at " __FILE__)
#error platform is unknown.

#endif /* OS platform */

#if defined(__cplusplus)

template <typename ... types_t>
void UNUSED(types_t ...) {}

#elif defined(_WIN32) || defined(_WIN64)

#if !defined(UNUSED)
#define UNUSED(smth) smth
#endif /* !defined(UNUSED) */

#else /* !(defined(_WIN32) || defined(_WIN64)) */

#if !defined(UNUSED)
#define UNUSED(smth) {(void) smth;}
#endif /* !defined(UNUSED) */

#endif /* defined(__cplusplus) */

/* define the platform identifier */
#if defined(_M_X64) || defined(_M_AMD64) || defined(__amd64__) || defined(__x86_64__)

/* declare Intel x64 / AMD64 platfrom */
#ifndef _X64
#define _X64
#endif /* _X64 */

#ifndef _64BIT
#define _64BIT
#endif /* _64BIT */

#if !defined(__CDECL)
#define __CDECL __cdecl
#endif /* !defined(__CDECL) */

#elif defined(_M_IX86) || defined(__i386__)

/* declare Intel x86 platfrom */
#ifndef _X86
#define _X86
#endif /* _X86 */

#ifndef _32BIT
#define _32BIT
#endif /* _32BIT */

#if !defined(__CDECL)
#define __CDECL __cdecl
#endif /* !defined(__CDECL) */

#elif defined(_M_ARM) || defined(ARM) || defined(_ARM) || defined(__arm__)

/* declare ARM platfrom */
#ifndef _ARM
#define _ARM
#endif /* _ARM */

#ifndef _32BIT
#define _32BIT
#endif /* _32BIT */

#if !defined(__CDECL)
#define __CDECL
#endif /* !defined(__CDECL) */

#elif defined(_M_ARM64) || defined(ARM64) || defined(_ARM64) || defined(__arm64__) || defined(__aarch64__)

/* declare ARM64 platfrom */
#ifndef _ARM64
#define _ARM64
#endif /* _ARM64 */

#ifndef _64BIT
#define _64BIT
#endif /* _64BIT */

#if !defined(__CDECL)
#define __CDECL
#endif /* !defined(__CDECL) */

#else /* unknown platform */

#pragma message("ERROR: specify the platform at " __FILE__)
#error platform is unknown.

#endif /* CPU platform */

#endif /* !defined(__DIMANCHE_BASIC_PLATFORM_H) */
