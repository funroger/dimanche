; copyright (c) 2013 Victor Cherepanov
; distributed under BSD 3-clause license

%ifndef __DIMANCHE_SYSTEM_ASM_DEFS_INC
%define __DIMANCHE_SYSTEM_ASM_DEFS_INC


;
; set up defines and platform macros
;

; set memory model
default rel

; set basic defines
%ifidn __OUTPUT_FORMAT__, win32
%define _X86
%define _WINDOWS
%endif

%ifidn __OUTPUT_FORMAT__, win64
%define _X64
%define _WINDOWS
%endif

%ifidn __OUTPUT_FORMAT__, elf32
%define _X86
%define _LINUX
%endif

%ifidn __OUTPUT_FORMAT__, elf64
%define _X64
%define _LINUX
%endif

%ifidn __OUTPUT_FORMAT__, macho32
%define _X86
%define _LINUX
%endif

%ifidn __OUTPUT_FORMAT__, macho64
%define _X64
%define _LINUX
%endif


%define BYTE DB
%define WORD DW
%define DWORD DD


; set shuffle constants
%assign CLONE_DWORD_0 00000000b


; set platform macros
%macro LOAD_ADDRESS 2
    lea %1, [%2]
%endmacro


%ifdef _X86

%assign _GEN_REG_SIZE 4
%assign _XMM_REG_SIZE 16
%define _BASE_REG ebp
%define STACK_REG esp

%assign _NUM_REG_ARGS 0
%define _NON_VLTL_GEN_REG_LIST ebx, esi, edi
%define _NON_VLTL_XMM_REG_LIST

%ifdef _LINUX

%unmacro LOAD_ADDRESS 2
%macro LOAD_ADDRESS 2
    call %%_LOAD_ADDRESS_LOCAL_LABEL
%%_LOAD_ADDRESS_LOCAL_LABEL:
    pop %1
    add %1, [%2 - %%_LOAD_ADDRESS_LOCAL_LABEL]
%endmacro ; LOAD_ADDRESS

%endif ; _LINUX

%elifdef _X64

%assign _GEN_REG_SIZE 8
%assign _XMM_REG_SIZE 16
%define _BASE_REG rbp
%define STACK_REG rsp

%ifdef _WINDOWS

%assign _NUM_REG_ARGS 4
%define _ARG_0 rcx
%define _ARG_1 rdx
%define _ARG_2 r8
%define _ARG_3 r9

%define _NON_VLTL_GEN_REG_LIST rbx, rsi, rdi, r12, r13, r14, r15
%define _NON_VLTL_XMM_REG_LIST xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15

%elifdef _LINUX

%assign _NUM_REG_ARGS 6
%define _ARG_0 rdi
%define _ARG_1 rsi
%define _ARG_2 rdx
%define _ARG_3 rcx
%define _ARG_4 r8
%define _ARG_5 r9

%define _NON_VLTL_GEN_REG_LIST rbx, r12, r13, r14, r15
%define _NON_VLTL_XMM_REG_LIST

%else ; _WINDOWS

%fatal Unknown platform

%endif ; _WINDOWS

%else ; _X86

%fatal Unknown CPU architecture

%endif ; _X86

;
; declare macros for function implementing
;


%macro _GET_ARG_TYPE 1

    %define arg_type %1

    %ifidni PTR, arg_type
        %define arg_type PTRDIFF_T
    %elifidni SIZE_T, arg_type
        %define arg_type PTRDIFF_T
    %endif

    %ifidni QWORD, arg_type

        %define arg_reg_type _QWORD
        %define arg_mem_type qword

    %elifidni DWORD, arg_type

        %define arg_reg_type _DWORD
        %define arg_mem_type dword

    %elifidni WORD, arg_type

        %define arg_reg_type _WORD
        %define arg_mem_type word

    %elifidni BYTE, arg_type

        %define arg_reg_type _BYTE
        %define arg_mem_type byte

    %elifidni PTRDIFF_T, arg_type

        %ifdef _X64
            %define arg_reg_type _QWORD
            %define arg_mem_type qword
        %else
            %define arg_reg_type _DWORD
            %define arg_mem_type dword
        %endif

    %else
        %fatal unknown argument type %1
    %endif

    %undef arg_type

%endmacro ; _GET_ARG_TYPE


%macro DECL_ARGS 0-*

    %ifndef _decl_args_second_declaration
        %define _decl_args_second_declaration
    %else
        %warning "it's not recommended to have 2 DECL_ARGS in one file."
    %endif

    ; parameters are: for every argument TYPE + name
    ; ex. PTR, pSrc, SIZE_T, size, WORD, value

    %if %0 & 1
        %fatal number of passed arguments should be even (type + name)
    %endif

    %assign arg_idx 0
    %ifdef _X64
        %assign arg_offset 10h
    %else
        %assign arg_offset 08h
    %endif

    %rep %0 / 2
        _GET_ARG_TYPE %1
        %rotate 1

        %if arg_idx < _NUM_REG_ARGS
            %xdefine %1 TO %+ arg_reg_type(_ARG_ %+ arg_idx)
        %else
            %xdefine %1 arg_mem_type [_ARGS_BASE + arg_offset]
            %assign arg_offset arg_offset + _GEN_REG_SIZE
        %endif
        %undef arg_reg_type
        %undef arg_mem_type
        %rotate 1

        %assign arg_idx arg_idx + 1
    %endrep

%endmacro ; DECL_ARGS


%macro _CHECK_NON_VLTL_REG 1-*

    %define reg_to_find %1

    %rep %0 - 1
        %ifidni reg_to_find, %2
            %define found
        %endif
        %rotate 1
    %endrep

%endmacro ; _CHECK_NON_VLTL_REG


%macro _CHECK_NON_VLTL_REGS 0-*

    %rep %0
        _CHECK_NON_VLTL_REG %1, _NON_VLTL_GEN_REG_LIST, _NON_VLTL_XMM_REG_LIST
        %ifndef found
            %fatal is register reg_to_find to save?
        %else
            %undef found
        %endif

        %rotate 1
    %endrep

%endmacro ; _CHECK_NON_VLTL_REGS


%macro _PARSE_NON_VLTL_REG 1-*

    %ifndef regs_stack_size
        %assign regs_stack_size 0
    %endif
    %xdefine reg_to_save %1

    %rep %0 - 1
        %ifidni reg_to_save, %2
            %xdefine %2%[_store] regs_stack_size
            %assign regs_stack_size regs_stack_size + reg_size
        %endif
        %rotate 1
    %endrep

    %undef reg_to_save

%endmacro ; _PARSE_NON_VLTL_REG


%macro _PARSE_NON_VLTL_REGS 1-*

    %assign reg_size %1

    %rep %0 - 1
        _PARSE_NON_VLTL_REG %2, USED_REGS
        %rotate 1
    %endrep

    %undef reg_size

%endmacro ; _PARSE_NON_VLTL_REGS


%macro _SAVE_NON_VLTL_REGS 1-*

    %define save_instr %1

    %rep %0 - 1
        %ifdef %2%[_store]
            save_instr [_REGS_BASE + %2%[_store]], %2
        %endif
        %rotate 1
    %endrep

    %undef save_instr

%endmacro ; _SAVE_NON_VLTL_REGS


%macro DECL_USED_REGS 0-*

    ; parameters are the list of used registers (both general and XMM)

    _CHECK_NON_VLTL_REGS USED_REGS
    _PARSE_NON_VLTL_REGS _GEN_REG_SIZE, _NON_VLTL_GEN_REG_LIST
    _PARSE_NON_VLTL_REGS _XMM_REG_SIZE, _NON_VLTL_XMM_REG_LIST

%endmacro ; DECL_USED_REGS


%macro ENTER_PROC 0-1

    ; the parameter is an optional 'use the stack register as the base' flag

    ; create procedure frame
    push _BASE_REG
    mov _BASE_REG, STACK_REG

    %ifndef vars_stack_size
        %assign vars_stack_size 0
    %endif

    %ifndef regs_stack_size
        %assign regs_stack_size 0
    %endif

    ; align variable storage to store registers by aligned addresses
    %ifdef _X64
    %if %0
        %assign stack_align (vars_stack_size + regs_stack_size) % 16
        %assign vars_stack_size vars_stack_size + stack_align
    %endif
    %endif

    ; allocate stack memory
    %if vars_stack_size + regs_stack_size
        sub STACK_REG, vars_stack_size + regs_stack_size
    %endif

    ; create base register
    %if %0
        %define _VARS_BASE STACK_REG
        %define _REGS_BASE STACK_REG + vars_stack_size
        %define _ARGS_BASE STACK_REG + vars_stack_size + regs_stack_size
        %assign _BASE_REG%[_store] vars_stack_size + regs_stack_size
    %else
        %define _VARS_BASE _BASE_REG - vars_stack_size - regs_stack_size
        %define _REGS_BASE _BASE_REG - regs_stack_size
        %define _ARGS_BASE _BASE_REG
    %endif

    _SAVE_NON_VLTL_REGS mov, _NON_VLTL_GEN_REG_LIST
    _SAVE_NON_VLTL_REGS movaps, _NON_VLTL_XMM_REG_LIST

%endmacro ; ENTER_PROC


%macro _LOAD_NON_VLTL_REGS 1-*

    %define load_instr %1

    %rep %0 - 1
        %ifdef %2%[_store]
            load_instr %2, [_REGS_BASE + %2%[_store]]
            %undef %2%[_store]
        %endif
        %rotate 1
    %endrep

%endmacro ; _LOAD_NON_VLTL_REGS


%macro LEAVE_PROC 0

    _LOAD_NON_VLTL_REGS mov, _NON_VLTL_GEN_REG_LIST
    _LOAD_NON_VLTL_REGS movaps, _NON_VLTL_XMM_REG_LIST

    %ifdef _BASE_REG%[_store]
        add STACK_REG, _BASE_REG%[_store]
        %undef _BASE_REG%[_store]
    %else
        mov STACK_REG, _BASE_REG
    %endif

    pop _BASE_REG
    ret

    %undef _VARS_BASE
    %undef _REGS_BASE
    %undef _ARGS_BASE

    %undef vars_stack_size
    %undef regs_stack_size

%endmacro ; LEAVE_PROC

;
; register casting macros
;

%define TO_BYTE(reg) _BYTE_%+reg
%define TO_WORD(reg) _WORD_%+reg
%define TO_DWORD(reg) _DWORD_%+reg
%define TO_QWORD(reg) _QWORD_%+reg

%define _BYTE_al al
%define _BYTE_ax al
%define _BYTE_eax al
%define _BYTE_rax al
%define _BYTE_bl bl
%define _BYTE_bx bl
%define _BYTE_ebx bl
%define _BYTE_rbx bl
%define _BYTE_cl cl
%define _BYTE_cx cl
%define _BYTE_ecx cl
%define _BYTE_rcx cl
%define _BYTE_dl dl
%define _BYTE_dx dl
%define _BYTE_edx dl
%define _BYTE_rdx dl
%define _BYTE_sil sil
%define _BYTE_si sil
%define _BYTE_esi sil
%define _BYTE_rsi sil
%define _BYTE_dil dil
%define _BYTE_di dil
%define _BYTE_edi dil
%define _BYTE_rdi dil
%define _BYTE_r8b r8b
%define _BYTE_r8w r8b
%define _BYTE_r8d r8b
%define _BYTE_r8 r8b
%define _BYTE_r9b r9b
%define _BYTE_r9w r9b
%define _BYTE_r9d r9b
%define _BYTE_r9 r9b
%define _BYTE_r10b r10b
%define _BYTE_r10w r10b
%define _BYTE_r10d r10b
%define _BYTE_r10 r10b
%define _BYTE_r11b r11b
%define _BYTE_r11w r11b
%define _BYTE_r11d r11b
%define _BYTE_r11 r11b
%define _BYTE_r12b r12b
%define _BYTE_r12w r12b
%define _BYTE_r12d r12b
%define _BYTE_r12 r12b
%define _BYTE_r13b r13b
%define _BYTE_r13w r13b
%define _BYTE_r13d r13b
%define _BYTE_r13 r13b
%define _BYTE_r14b r14b
%define _BYTE_r14w r14b
%define _BYTE_r14d r14b
%define _BYTE_r14 r14b
%define _BYTE_r15b r15b
%define _BYTE_r15w r15b
%define _BYTE_r15d r15b
%define _BYTE_r15 r15b

%define _WORD_al ax
%define _WORD_ax ax
%define _WORD_eax ax
%define _WORD_rax ax
%define _WORD_bl bx
%define _WORD_bx bx
%define _WORD_ebx bx
%define _WORD_rbx bx
%define _WORD_cl cx
%define _WORD_cx cx
%define _WORD_ecx cx
%define _WORD_rcx cx
%define _WORD_dl dx
%define _WORD_dx dx
%define _WORD_edx dx
%define _WORD_rdx dx
%define _WORD_sil si
%define _WORD_si si
%define _WORD_esi si
%define _WORD_rsi si
%define _WORD_dil di
%define _WORD_di di
%define _WORD_edi di
%define _WORD_rdi di
%define _WORD_r8b r8w
%define _WORD_r8w r8w
%define _WORD_r8d r8w
%define _WORD_r8 r8w
%define _WORD_r9b r9w
%define _WORD_r9w r9w
%define _WORD_r9d r9w
%define _WORD_r9 r9w
%define _WORD_r10b r10w
%define _WORD_r10w r10w
%define _WORD_r10d r10w
%define _WORD_r10 r10w
%define _WORD_r11b r11w
%define _WORD_r11w r11w
%define _WORD_r11d r11w
%define _WORD_r11 r11w
%define _WORD_r12b r12w
%define _WORD_r12w r12w
%define _WORD_r12d r12w
%define _WORD_r12 r12w
%define _WORD_r13b r13w
%define _WORD_r13w r13w
%define _WORD_r13d r13w
%define _WORD_r13 r13w
%define _WORD_r14b r14w
%define _WORD_r14w r14w
%define _WORD_r14d r14w
%define _WORD_r14 r14w
%define _WORD_r15b r15w
%define _WORD_r15w r15w
%define _WORD_r15d r15w
%define _WORD_r15 r15w

%define _DWORD_al eax
%define _DWORD_ax eax
%define _DWORD_eax eax
%define _DWORD_rax eax
%define _DWORD_bl ebx
%define _DWORD_bx ebx
%define _DWORD_ebx ebx
%define _DWORD_rbx ebx
%define _DWORD_cl ecx
%define _DWORD_cx ecx
%define _DWORD_ecx ecx
%define _DWORD_rcx ecx
%define _DWORD_dl edx
%define _DWORD_dx edx
%define _DWORD_edx edx
%define _DWORD_rdx edx
%define _DWORD_ebp ebp
%define _DWORD_rbp ebp
%define _DWORD_esp esp
%define _DWORD_rsp esp
%define _DWORD_sil esi
%define _DWORD_si esi
%define _DWORD_esi esi
%define _DWORD_rsi esi
%define _DWORD_dil edi
%define _DWORD_di edi
%define _DWORD_edi edi
%define _DWORD_rdi edi
%define _DWORD_r8b r8d
%define _DWORD_r8w r8d
%define _DWORD_r8d r8d
%define _DWORD_r8 r8d
%define _DWORD_r9b r9d
%define _DWORD_r9w r9d
%define _DWORD_r9d r9d
%define _DWORD_r9 r9d
%define _DWORD_r10b r10d
%define _DWORD_r10w r10d
%define _DWORD_r10d r10d
%define _DWORD_r10 r10d
%define _DWORD_r11b r11d
%define _DWORD_r11w r11d
%define _DWORD_r11d r11d
%define _DWORD_r11 r11d
%define _DWORD_r12b r12d
%define _DWORD_r12w r12d
%define _DWORD_r12d r12d
%define _DWORD_r12 r12d
%define _DWORD_r13b r13d
%define _DWORD_r13w r13d
%define _DWORD_r13d r13d
%define _DWORD_r13 r13d
%define _DWORD_r14b r14d
%define _DWORD_r14w r14d
%define _DWORD_r14d r14d
%define _DWORD_r14 r14d
%define _DWORD_r15b r15d
%define _DWORD_r15w r15d
%define _DWORD_r15d r15d
%define _DWORD_r15 r15d

%define _QWORD_al rax
%define _QWORD_ax rax
%define _QWORD_eax rax
%define _QWORD_rax rax
%define _QWORD_bl rbx
%define _QWORD_bx rbx
%define _QWORD_ebx rbx
%define _QWORD_rbx rbx
%define _QWORD_cl rcx
%define _QWORD_cx rcx
%define _QWORD_ecx rcx
%define _QWORD_rcx rcx
%define _QWORD_dl rdx
%define _QWORD_dx rdx
%define _QWORD_edx rdx
%define _QWORD_rdx rdx
%define _QWORD_ebp rbp
%define _QWORD_rbp rbp
%define _QWORD_esp rsp
%define _QWORD_rsp rsp
%define _QWORD_sil rsi
%define _QWORD_si rsi
%define _QWORD_esi rsi
%define _QWORD_rsi rsi
%define _QWORD_dil rdi
%define _QWORD_di rdi
%define _QWORD_edi rdi
%define _QWORD_rdi rdi
%define _QWORD_r8b r8
%define _QWORD_r8w r8
%define _QWORD_r8d r8
%define _QWORD_r8 r8
%define _QWORD_r9b r9
%define _QWORD_r9w r9
%define _QWORD_r9d r9
%define _QWORD_r9 r9
%define _QWORD_r10b r10
%define _QWORD_r10w r10
%define _QWORD_r10d r10
%define _QWORD_r10 r10
%define _QWORD_r11b r11
%define _QWORD_r11w r11
%define _QWORD_r11d r11
%define _QWORD_r11 r11
%define _QWORD_r12b r12
%define _QWORD_r12w r12
%define _QWORD_r12d r12
%define _QWORD_r12 r12
%define _QWORD_r13b r13
%define _QWORD_r13w r13
%define _QWORD_r13d r13
%define _QWORD_r13 r13
%define _QWORD_r14b r14
%define _QWORD_r14w r14
%define _QWORD_r14d r14
%define _QWORD_r14 r14
%define _QWORD_r15b r15
%define _QWORD_r15w r15
%define _QWORD_r15d r15
%define _QWORD_r15 r15

%endif ; __DIMANCHE_SYSTEM_ASM_DEFS_INC
