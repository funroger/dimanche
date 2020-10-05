
%ifndef __DIMANCHE_ASM_DEFS_INC
%define __DIMANCHE_ASM_DEFS_INC


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


; set platform macros
%macro LOAD_ADDRESS 2
    lea %1, [%2]
%endmacro


%ifdef _X86

%assign GEN_REG_SIZE 4
%assign XMM_REG_SIZE 16
%define BASE_REG ebp
%define STACK_REG esp

%define NON_VLTL_GEN_REG_LIST ebx, esi, edi
%define NON_VLTL_XMM_REG_LIST

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

%assign GEN_REG_SIZE 8
%assign XMM_REG_SIZE 16
%define BASE_REG rbp
%define STACK_REG rsp

%ifdef _WINDOWS

%define NON_VLTL_GEN_REG_LIST rbx, rsi, rdi, r12, r13, r14, r15
%define NON_VLTL_XMM_REG_LIST xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15

%elifdef _LINUX

%define NON_VLTL_GEN_REG_LIST rbx, r12, r13, r14, r15
%define NON_VLTL_XMM_REG_LIST

%else ; _WINDOWS

%fatal Unknown platform

%endif ; _WINDOWS

%else ; _X86

%fatal Unknown CPU architecture

%endif ; _X86

;
; declare macros for function implementing
;

%macro CHECK_NON_VLTL_REG 1-*

    %define reg_to_find %1

    %rep %0 - 1
        %ifidni reg_to_find, %2
            %define found
        %endif
        %rotate 1
    %endrep

%endmacro ; CHECK_NON_VLTL_REG


%macro CHECK_NON_VLTL_REGS 0-*

    %rep %0
        CHECK_NON_VLTL_REG %1, NON_VLTL_GEN_REG_LIST, NON_VLTL_XMM_REG_LIST
        %ifndef found
            %fatal is register reg_to_find to save?
        %else
            %undef found
        %endif

        %rotate 1
    %endrep

%endmacro ; CHECK_NON_VLTL_REGS


%macro PARSE_NON_VLTL_REG 1-*

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

%endmacro ; PARSE_NON_VLTL_REG


%macro PARSE_NON_VLTL_REGS 1-*

    %assign reg_size %1

    %rep %0 - 1
        PARSE_NON_VLTL_REG %2, USED_REGS
        %rotate 1
    %endrep

    %undef reg_size

%endmacro ; PARSE_NON_VLTL_REGS


%macro SAVE_NON_VLTL_REGS 1-*

    %define save_instr %1

    %rep %0 - 1
        %ifdef %2%[_store]
            save_instr [REGS_BASE + %2%[_store]], %2
        %endif
        %rotate 1
    %endrep

    %undef save_instr

%endmacro ; SAVE_NON_VLTL_REGS


%macro ENTER_PROC 0-1

    ; the first parameter is an optional 'use the stack register as the base' flag

    ; create procedure frame
    push BASE_REG
    mov BASE_REG, STACK_REG

    CHECK_NON_VLTL_REGS USED_REGS
    PARSE_NON_VLTL_REGS GEN_REG_SIZE, NON_VLTL_GEN_REG_LIST
    PARSE_NON_VLTL_REGS XMM_REG_SIZE, NON_VLTL_XMM_REG_LIST

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
        %define VARS_BASE STACK_REG
        %define REGS_BASE STACK_REG + vars_stack_size
        %define ARGS_BASE STACK_REG + vars_stack_size + regs_stack_size
        %assign BASE_REG%[_store] vars_stack_size + regs_stack_size
    %else
        %define VARS_BASE BASE_REG - vars_stack_size - regs_stack_size
        %define REGS_BASE BASE_REG - regs_stack_size
        %define ARGS_BASE BASE_REG
    %endif

    SAVE_NON_VLTL_REGS mov, NON_VLTL_GEN_REG_LIST
    SAVE_NON_VLTL_REGS movaps, NON_VLTL_XMM_REG_LIST

%endmacro ; ENTER_PROC


%macro LOAD_NON_VLTL_REGS 1-*

    %define load_instr %1

    %rep %0 - 1
        %ifdef %2%[_store]
            load_instr %2, [REGS_BASE + %2%[_store]]
            %undef %2%[_store]
        %endif
        %rotate 1
    %endrep

%endmacro ; LOAD_NON_VLTL_REGS


%macro LEAVE_PROC 0

    LOAD_NON_VLTL_REGS mov, NON_VLTL_GEN_REG_LIST
    LOAD_NON_VLTL_REGS movaps, NON_VLTL_XMM_REG_LIST

    %ifdef BASE_REG%[_store]
        add STACK_REG, BASE_REG%[_store]
        %undef BASE_REG%[_store]
    %else
        mov STACK_REG, BASE_REG
    %endif

    pop BASE_REG
    ret

    %undef VARS_BASE
    %undef REGS_BASE
    %undef ARGS_BASE

    %undef vars_stack_size
    %undef regs_stack_size

%endmacro ; LEAVE_PROC

%endif ; __DIMANCHE_ASM_DEFS_INC
