; copyright (c) 2013 Victor Cherepanov
; distributed under BSD 3-clause license

%define SIMD AVX2
%include "dimanche/image_processing/inc/color_conversion.nv12.x86.inc"
%include "dimanche/image_processing/inc/color_conversion.nv12_to_bgr.x86.inc"

align 10h
section .data

align 10h
section .text

PROC di_convert_nv12_to_bgra_even_row_avx2_asm

    test TO_DWORD(dst), 07h                                     ; is destination pointer aligned ?
    jnz _PROCESS_ALL_UNALIGNED_EVEN

    ; align line
    ASM_ALIGN_CONVERT_YUV422_TO_BGRA_8U_AVX2 dst, src_luma, TO_DWORD(counter), , even

    ; convert line
    ASM_CONVERT_YUV422_TO_BGRA_8U_AVX2 vmovntdq, vmovdqu, dst, src_luma, TO_DWORD(counter), 100h, , even

    ; process the rest of line
    ASM_REMAIN_CONVERT_YUV422_TO_BGRA_8U_AVX2 vmovdqa, vmovdqu, dst, src_luma, TO_DWORD(counter), , even

    ret

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; unaligned destination code branch
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ALIGN 10h
_PROCESS_ALL_UNALIGNED_EVEN:
    ASM_CONVERT_YUV422_TO_BGRA_8U_AVX2 vmovdqu, vmovdqu, dst, src_luma, TO_DWORD(counter), 100h, , even

    ; process the rest of line
    ASM_REMAIN_CONVERT_YUV422_TO_BGRA_8U_AVX2 vmovdqu, vmovdqu, dst, src_luma, TO_DWORD(counter), , even

    ret


PROC di_convert_nv12_to_bgra_odd_row_avx2_asm

    test TO_DWORD(dst), 07h                                     ; is destination pointer aligned ?
    jnz _PROCESS_ALL_UNALIGNED_ODD

    ; align line
    ASM_ALIGN_CONVERT_YUV422_TO_BGRA_8U_AVX2 dst, src_luma, TO_DWORD(counter), , odd

    ; convert line
    ASM_CONVERT_YUV422_TO_BGRA_8U_AVX2 vmovntdq, vmovdqu, dst, src_luma, TO_DWORD(counter), 100h, , odd

    ; process the rest of line
    ASM_REMAIN_CONVERT_YUV422_TO_BGRA_8U_AVX2 vmovdqa, vmovdqu, dst, src_luma, TO_DWORD(counter), , odd

    ret

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; unaligned destination code branch
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ALIGN 10h
_PROCESS_ALL_UNALIGNED_ODD:
    ASM_CONVERT_YUV422_TO_BGRA_8U_AVX2 vmovdqu, vmovdqu, dst, src_luma, TO_DWORD(counter), 100h, , odd

    ; process the rest of line
    ASM_REMAIN_CONVERT_YUV422_TO_BGRA_8U_AVX2 vmovdqu, vmovdqu, dst, src_luma, TO_DWORD(counter), , odd

    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Function to convert picture in NV12 format to BGRA format
; PARAMS:   const IMAGE * - pointer to a destination IMAGE structure
;           const IMAGE * - pointer to a source IMAGE structure
; RETURN:   none
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


PROC di_convert_nv12_to_bgra_avx2_asm, global

    DECL_ARGS PTR, pDstImage, PTR, pSrcImage
    DECL_USED_REGS USED_REGS
    DECL_LOCAL_VARS LOCAL_VARS
    ENTER_PROC

    LOAD_POINTERS bgra_bpp, change_direction

    cmp imageHeight, 02h
    jb _EXIT
    jmp _NEXT

ALIGN 10h
_RESTART:
    call di_convert_nv12_to_bgra_even_row_avx2_asm

    ; advance pointers
    add src_luma, srcInc                                        ; set new luma source line
    add dst, dstInc                                             ; set new destination line
    mov TO_DWORD(counter), imageWidth                           ; load width
    ; restore chrominance pointers for the second line
    sub src_chroma, counter                                     ; restore chroma pointer

    call di_convert_nv12_to_bgra_odd_row_avx2_asm

    ; advance pointers
    add src_luma, srcInc                                        ; set new luma source line
    add dst, dstInc                                             ; set new destination line
    mov TO_DWORD(counter), imageWidth                           ; load width
    sub src_chroma, counter                                     ; restore chroma pointer
    add src_chroma, chromaStride                                ; set new chroma source line

ALIGN 10h
_NEXT:
    sub imageHeight, 02h                                        ; decrement height
    jnz _RESTART

    call di_convert_nv12_to_bgra_even_row_avx2_asm

    ; advance pointers
    add src_luma, srcInc                                        ; set new luma source line
    add dst, dstInc                                             ; set new destination line
    mov TO_DWORD(counter), imageWidth                           ; load width
    ; restore chrominance pointers for the second line
    sub src_chroma, counter                                     ; restore chroma pointer

    call di_convert_nv12_to_bgra_even_row_avx2_asm

ALIGN 10h
_EXIT:
    vzeroupper                                                  ; reset AVX state
    sfence                                                      ; store fence
    LEAVE_PROC
