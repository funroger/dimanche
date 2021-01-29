; copyright (c) 2013 Victor Cherepanov
; distributed under BSD 3-clause license

%include "dimanche/basic/asm_defs.x86.inc"

section .data

CPU_UNKNOWN EQU 00000000h ; not featured CPU (Pentium, K5)
CPU_INTEL EQU 40000000h
CPU_AMD EQU 80000000h

; cpuid (eax = 01h), edx
CPUID_CMOV_BIT EQU 00008000h
CPUID_MMX_BIT EQU 00800000h
CPUID_SSE_BIT EQU 02000000h
CPUID_SSE2_BIT EQU 04000000h
; cpuid (eax = 01h), ecx
CPUID_SSE3_BIT EQU 00000001h
CPUID_SSE3S_BIT EQU 00000200h
CPUID_FMA_BIT EQU 00001000h
CPUID_SSE41_BIT EQU 00080000h
CPUID_SSE42_BIT EQU 00100000h
CPUID_AES_BIT EQU 02000000h
CPUID_OSXSAVE_BIT EQU 08000000h
CPUID_AVX_BIT EQU 10000000h
CPUID_RDRAND_BIT EQU 40000000h
; cpuid (eax = 07h, ecx = 00h), ebx
CPUID_BMI_BIT EQU 00000008h
CPUID_AVX2_BIT EQU 00000020h
CPUID_BMI2_BIT EQU 00000100h
CPUID_ERMS_BIT EQU 00000200h
CPUID_AVX512F_BIT EQU 00010000h
CPUID_AVX512DQ_BIT EQU 00020000h
CPUID_AVX512BW_BIT EQU 40000000h
CPUID_AVX512VL_BIT EQU 80000000h
; cpuid (eax = 80000001h), edx
CPUID_MMP_BIT EQU 00400000h
; cpuid (eax = 80000001h), ecx
CPUID_ABM_BIT EQU 00000020h

XCR0_SSE_BIT EQU 00000002h
XCR0_AVX_BIT EQU 00000004h
XCR0_opmask_BIT EQU 00000020h
XCR0_ZMM_hi256_BIT EQU 00000040h
XCR0_Hi16_ZMM_BIT EQU 00000080h

XCR0_AVX_BITSET EQU XCR0_AVX_BIT | XCR0_SSE_BIT
XCR0_AVX512_BITSET EQU XCR0_Hi16_ZMM_BIT | XCR0_ZMM_hi256_BIT | XCR0_opmask_BIT | XCR0_AVX_BITSET

FEATURE_CMOV_BIT EQU 00000001h
FEATURE_MMX EQU 00000002h
FEATURE_MMP_BIT EQU 00000004h
FEATURE_MMP EQU FEATURE_MMP_BIT + FEATURE_MMX
FEATURE_SSE EQU 00000008h + FEATURE_MMP
FEATURE_SSE2 EQU 00000010h + FEATURE_SSE
FEATURE_SSE3 EQU 00000020h + FEATURE_SSE2
FEATURE_SSE3S EQU 00000040h + FEATURE_SSE3
FEATURE_FMA_BIT EQU 00000080h
FEATURE_SSE41 EQU 00000100h + FEATURE_SSE3S
FEATURE_SSE42 EQU 00000200h + FEATURE_SSE41
FEATURE_ABM_BIT EQU 00000400h
FEATURE_AES_BIT EQU 00000800h
FEATURE_AVX EQU 00001000h + FEATURE_SSE42
FEATURE_RDRAND_BIT EQU 00002000h
FEATURE_BMI_BIT EQU 00004000h
FEATURE_AVX2 EQU 00008000h + FEATURE_AVX
FEATURE_BMI2_BIT EQU 00010000h
FEATURE_ERMS_BIT EQU 00020000h
FEATURE_AVX512F EQU 00040000h + FEATURE_AVX2
FEATURE_AVX512VL EQU 00080000h + FEATURE_AVX512F
FEATURE_AVX512BW EQU 00100000h
FEATURE_AVX512DQ EQU 00200000h


; data section of file
align 10h
IntelVendor: BYTE "GenuineIntel"
AMDVendor: BYTE "AuthenticAMD"


; code section of file
section .text

global di_system_cpu_get_type_asm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Function to retrieve CPU features
; PARAMS:   none
; RETURN:   CPU type
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; x86 definition section
%ifdef _X86

    %define temp eax
    %define feature_list esi

    %define USED_REGS ebx, esi

; x64 Windows definition section
%elifdef _WINDOWS

    %define temp rax
    %define feature_list esi

    %define USED_REGS rbx, rsi

; x64 Linux definition section
%elifdef _LINUX

    %define temp rax
    %define feature_list esi

    %define USED_REGS rbx

%endif ; _X86


align 10h
di_system_cpu_get_type_asm:

    DECL_USED_REGS USED_REGS
    ENTER_PROC

    ; get standard feature set
    mov eax, 00000001h
    cpuid

    ; check OXSAVE feature enabled for correct YMM? instructons saving
    and ecx, CPUID_OSXSAVE_BIT | CPUID_AVX_BIT
    cmp ecx, CPUID_OSXSAVE_BIT | CPUID_AVX_BIT
    jne _CHECK_SSE_TECHNOLOGIES

    ; processor supports AVX instructions and XGETBV is enabled by OS
    xor ecx, ecx ; specify 0 for XFEATURE_ENABLED_MASK register
    ; xgetbv instruction emulation
    ; xgetbv
    BYTE 0fh, 01h, 0d0h

    and eax, XCR0_AVX512_BITSET ; mask bits [7:5] and [2:1]
    cmp eax, XCR0_AVX512_BITSET ; check OS has enabled both XMM and YMM state support
    je _CHECK_AVX512_TECHNOLOGIES

    and eax, XCR0_AVX_BITSET ; mask bits [2:1]
    cmp eax, XCR0_AVX_BITSET ; check OS has enabled both XMM and YMM state support
    je _CHECK_AVX2_TECHNOLOGY

    mov feature_list, FEATURE_SSE42
    jmp _CHECK_ADVANCED_TECHNOLOGIES

    ; decide which AVX technology is most suitable

ALIGN 10h
_CHECK_AVX512_TECHNOLOGIES:

    ; get advanced feature set
    mov eax, 07h
    mov ecx, 00h
    cpuid

    ; check AVX512F bit
    test ebx, CPUID_AVX512F_BIT
    jz _CHECK_AVX2_TECHNOLOGY

    mov feature_list, FEATURE_AVX512F

    ; don't support any advanced AVX512 technology without 'Vector Length Extensions'
    test ebx, CPUID_AVX512VL_BIT
    jz _CHECK_ADVANCED_TECHNOLOGIES
    or feature_list, FEATURE_AVX512VL

    ; set AVX512BW bit
    test ebx, CPUID_AVX512BW_BIT
    lea eax, [feature_list + FEATURE_AVX512BW]
    cmovnz feature_list, eax

    ; set AVX512DQ bit
    test ebx, CPUID_AVX512DQ_BIT
    lea eax, [feature_list + FEATURE_AVX512DQ]
    cmovnz feature_list, eax

    jmp _CHECK_ADVANCED_TECHNOLOGIES

ALIGN 10h
_CHECK_AVX2_TECHNOLOGY:

    ; set AVX technology
    mov feature_list, FEATURE_AVX

    ; get advanced feature set
    mov eax, 07h
    mov ecx, 00h
    cpuid

    ; set AVX2 bit
    test ebx, CPUID_AVX2_BIT
    mov eax, FEATURE_AVX2
    cmovnz feature_list, eax

    jmp _CHECK_ADVANCED_TECHNOLOGIES

ALIGN 10h
_CHECK_SSE_TECHNOLOGIES:

    ; get standard feature set
    mov eax, 00000001h
    cpuid

    ; set SSE42 bit
    test ecx, CPUID_SSE42_BIT
    mov feature_list, FEATURE_SSE42
    jnz _CHECK_ADVANCED_TECHNOLOGIES

    ; set SSE41 bit
    test ecx, CPUID_SSE41_BIT
    mov feature_list, FEATURE_SSE41
    jnz _CHECK_ADVANCED_TECHNOLOGIES

    ; set SSE3S bit
    test ecx, CPUID_SSE3S_BIT
    mov feature_list, FEATURE_SSE3S
    jnz _CHECK_ADVANCED_TECHNOLOGIES

    ; set SSE3 bit
    test ecx, CPUID_SSE3_BIT
    mov feature_list, FEATURE_SSE3
    jnz _CHECK_ADVANCED_TECHNOLOGIES

    ; set SSE2 bit
    test edx, CPUID_SSE2_BIT
    mov feature_list, FEATURE_SSE2
    jnz _CHECK_ADVANCED_TECHNOLOGIES

    ; set SSE bit
    test edx, CPUID_SSE_BIT
    mov feature_list, FEATURE_SSE
    jnz _CHECK_ADVANCED_TECHNOLOGIES

    ; set MMX bit
    test edx, CPUID_MMX_BIT
    mov feature_list, FEATURE_MMX

ALIGN 10h
_CHECK_ADVANCED_TECHNOLOGIES:

    ; get standard feature set again to avoid too complex code
    mov eax, 00000001h
    cpuid

    ; check cMOV bit
    test edx, CPUID_CMOV_BIT
    lea eax, [feature_list + FEATURE_CMOV_BIT]
    cmovnz feature_list, eax

    ; set FMA bit
    test ecx, CPUID_FMA_BIT
    lea eax, [feature_list + FEATURE_FMA_BIT]
    cmovnz feature_list, eax

    ; set AES bit
    nop
    test ecx, CPUID_AES_BIT
    lea eax, [feature_list + FEATURE_AES_BIT]
    cmovnz feature_list, eax

    ; set RDRAND bit
    test ecx, CPUID_RDRAND_BIT
    lea eax, [feature_list + FEATURE_RDRAND_BIT]
    cmovnz feature_list, eax

    ; don't worry about high function. if something is not supported,
    ; zeros are returned.

    ; get advanced feature set
    mov eax, 80000001h
    cpuid

    ; set ABM bit
    test ecx, CPUID_ABM_BIT
    lea eax, [feature_list + FEATURE_ABM_BIT]
    cmovnz feature_list, eax

    ; get advanced feature set
    mov eax, 07h
    mov ecx, 00h
    cpuid

    ; set BMI bit
    test ebx, CPUID_BMI_BIT
    lea eax, [feature_list + FEATURE_BMI_BIT]
    cmovnz feature_list, eax

    ; set BMI2 bit
    test ebx, CPUID_BMI2_BIT
    lea eax, [feature_list + FEATURE_BMI2_BIT]
    cmovnz feature_list, eax

    ; set ERMS bit
    test ebx, CPUID_ERMS_BIT
    lea eax, [feature_list + FEATURE_ERMS_BIT]
    cmovnz feature_list, eax

    ; identify CPU vendor

    xor eax, eax
    cpuid
    LOAD_ADDRESS temp, IntelVendor
    xor ebx, dword [temp + 00h]
    xor edx, dword [temp + 04h]
    xor ecx, dword [temp + 08h]
    or ebx, edx
    xor eax, eax
    or ecx, ebx
    mov ebx, CPU_INTEL
    cmovz eax, ebx ; Intel processor found
    lea feature_list, [feature_list + eax] ; add vendor to features
    jnz _CHECK_OTHER_VENDORS

ALIGN 10h
_EXIT:
    mov eax, feature_list
    LEAVE_PROC
    ret

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; checking other vendors specific technologies
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ALIGN 10h
_CHECK_OTHER_VENDORS:

    xor eax, eax
    cpuid
    LOAD_ADDRESS temp, AMDVendor
    xor ebx, dword [temp + 00h]
    xor edx, dword [temp + 04h]
    xor ecx, dword [temp + 08h]
    or ebx, edx
    xor eax, eax
    or ecx, ebx
    mov ebx, CPU_AMD
    cmovz eax, ebx ; AMD processor found
    lea feature_list, [feature_list + eax] ; add vendor to features
%IFDEF _X86
    jnz _EXIT ; jump if the vendor is unknown

    ; MMP-present bit is already set
    test feature_list, FEATURE_MMP_BIT
    jnz _EXIT

    ; check MMP technology
    mov eax, 80000001h
    cpuid
    test edx, CPUID_MMP_BIT
    lea eax, [feature_list + FEATURE_MMP_BIT]
    cmovnz feature_list, eax

%ENDIF ; _X86

    jmp _EXIT
