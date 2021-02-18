; copyright (c) 2013 Victor Cherepanov
; distributed under BSD 3-clause license

%include "dimanche/basic/asm_defs.x86.inc"
%include "dimanche/basic/image.x86.inc"


; data section of file
align 10h
section .data

%xdefine BT601_5 BT.601.5
%xdefine BT709_5 BT.709.5
%xdefine JPEG_FR JPEG_FULL_RANGE

; possible parameters are:
; standard - BT.601.5, BT.709.5, JPEG_FULL_RANGE
; type - sse, avx2
%macro ASM_GENERATE_CONST_TABLE 2

    %push
    %xdefine %$standard %1
    %xdefine %$type %2

    ; set duplication counter
    ; sse+ - 8, avx2+ - 16
    %ifidni %$type, sse
    %assign %$COUNTER 08h
    %else
    %assign %$COUNTER 10h
    %endif

    times %$COUNTER WORD (0ff00h)                                 ; upper byte mask

    %ifidni %$standard, BT601_5

    ; conversion table ITU-R BT.601.5
    times %$COUNTER WORD (02543h)                                   ; mult. y for bgr
    times %$COUNTER WORD (0408dh)                                   ; mult. u for b
    times %$COUNTER WORD (03313h)                                   ; mult. v for r
    times %$COUNTER WORD (00c89h)                                   ; mult. u for g
    times %$COUNTER WORD (01a04h)                                   ; mult. v for g

    times %$COUNTER WORD (02289h)                                   ; const for b
    times %$COUNTER WORD (01bcch)                                   ; const for r
    times %$COUNTER WORD (01103h)                                   ; const for g

    %elifidni %$standard, BT709_5

    ; conversion table ITU-R BT.709.5
    times %$COUNTER WORD (02543h)                                   ; mult. y for bgr
    times %$COUNTER WORD (04399h)                                   ; mult. u for b
    times %$COUNTER WORD (0395eh)                                   ; mult. v for r
    times %$COUNTER WORD (006d3h)                                   ; mult. u for g
    times %$COUNTER WORD (0110eh)                                   ; mult. v for g

    times %$COUNTER WORD (0240fh)                                   ; const for b
    times %$COUNTER WORD (01ef2h)                                   ; const for r
    times %$COUNTER WORD (009adh)                                   ; const for g

    %elifidni %$standard, JPEG_FR

    ; conversion table for JPEG full range
    times %$COUNTER WORD (02000h)                                   ; mult. y for bgr
    times %$COUNTER WORD (038b4h)                                   ; mult. u for b
    times %$COUNTER WORD (02cddh)                                   ; mult. v for r
    times %$COUNTER WORD (00b03h)                                   ; mult. u for g
    times %$COUNTER WORD (016dah)                                   ; mult. v for g

    times %$COUNTER WORD (01c4ah)                                   ; const for b
    times %$COUNTER WORD (0165eh)                                   ; const for r
    times %$COUNTER WORD (010feh)                                   ; const for g

    %else

    %fatal unknown standard

    %endif

    %ifidni %$type, sse

    ; generate other constants
    BYTE 0,0,0,4,4,4,4,8,2,2,2,6,6,6,6,10                       ; chrominance shuffle constant (UYVY)
    BYTE 1,1,1,5,5,5,5,9,3,3,3,7,7,7,7,11                       ; chrominance shuffle constant (YUY2)
    BYTE 3,3,3,7,7,7,7,11,1,1,1,5,5,5,5,9                       ; chrominance shuffle constant (YVYU)
    times 10h BYTE (01h)                                          ; chrominance filtering constant
    BYTE 0,1,255,2,3,255,4,5,255,6,7,255,8,9,255,10             ; shuffle constants for BGR packing
    BYTE 3,255,4,5,255,6,7,255,8,9,255,10,11,255,12,13
    BYTE 255,6,7,255,8,9,255,10,11,255,12,13,255,14,15,255
    BYTE 255,255,0,255,255,1,255,255,2,255,255,3,255,255,4,255
    BYTE 255,5,255,255,6,255,255,7,255,255,8,255,255,9,255,255
    BYTE 10,255,255,11,255,255,12,255,255,13,255,255,14,255,255,15
    BYTE 8,0,8,1,8,2,8,3,8,4,8,5,8,6,8,7                        ; luminance shuffle constant (Y16)
    BYTE 0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4                        ; chrominance shuffle constant (Y16)
    BYTE 0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,12                       ; chrominance shuffle constant (Y16)
    BYTE 4,4,4,5,5,5,5,6,6,6,6,7,7,7,7,7                        ; chrominance shuffle constant (Y16)
    BYTE 4,9,4,11,4,13,4,15,4,0,4,1,4,2,4,3                     ; luminance shuffle constant (Y41P)
    BYTE 0,0,0,2,0,2,0,2,2,2,2,4,2,4,2,4                        ; chrominance shuffle constant (Y41P)
    BYTE 4,0,3,1,2,2,1,3,4,0,3,1,2,2,1,3                        ; chrominance filtering constant (Y41P)
    BYTE 1,1,1,3,1,3,1,3,3,3,3,5,3,5,3,5                        ; chrominance shuffle constant (Y41P)
    BYTE 4,4,4,6,4,6,4,6,6,6,6,8,6,8,6,8                        ; chrominance shuffle constant (Y41P)
    BYTE 5,5,5,7,5,7,5,7,7,7,7,9,7,9,7,9                        ; chrominance shuffle constant (Y41P)
    BYTE 0,0,0,1,1,1,1,3,4,4,4,5,5,5,5,7                        ; chrominance shuffle constant (Y422)
    BYTE 4,4,4,5,5,5,5,6,6,6,6,7,7,7,7,9                        ; chrominance shuffle constant (Y422)
    BYTE 0,0,0,2,2,2,2,4,4,4,4,6,6,6,6,8                        ; chrominance shuffle constant (NV12)

    %elifidni %$type, avx2

    ; generate other constants
    BYTE 0,0,0,4,4,4,4,8,2,2,2,6,6,6,6,10                       ; chrominance shuffle constant (UYVY)
    BYTE 0,0,0,4,4,4,4,8,2,2,2,6,6,6,6,10
    BYTE 1,1,1,5,5,5,5,9,3,3,3,7,7,7,7,11                       ; chrominance shuffle constant (YUY2)
    BYTE 1,1,1,5,5,5,5,9,3,3,3,7,7,7,7,11
    BYTE 3,3,3,7,7,7,7,11,1,1,1,5,5,5,5,9                       ; chrominance shuffle constant (YVYU)
    BYTE 3,3,3,7,7,7,7,11,1,1,1,5,5,5,5,9
    times 20h BYTE (01h)                                        ; chrominance filtering constant
    BYTE 0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4                        ; chrominance shuffle constant (Y422)
    BYTE 0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4
    BYTE 4,4,4,5,5,5,5,6,6,6,6,7,7,7,7,9                        ; chrominance shuffle constant (Y422)
    BYTE 4,4,4,5,5,5,5,6,6,6,6,7,7,7,7,9
    BYTE 0,0,0,2,2,2,2,4,4,4,4,6,6,6,6,8                        ; chrominance shuffle constant (NV12)
    BYTE 0,0,0,2,2,2,2,4,4,4,4,6,6,6,6,8

    %else

    %fatal unknown type

    %endif

%endmacro ; ASM_GENERATE_YUV_TO_BGR_CONST_TABLE


ALIGN 10h
di_color_conversion_yuv_to_bgr_bt601_5_table_asm:
    ASM_GENERATE_CONST_TABLE BT601_5, sse

di_color_conversion_yuv_to_bgr_bt709_5_table_asm:
    ASM_GENERATE_CONST_TABLE BT709_5, sse

di_color_conversion_yuv_to_bgr_jpeg_fr_table_asm:
    ASM_GENERATE_CONST_TABLE JPEG_FR, sse

di_color_conversion_yuv_to_bgr_bt601_5_table_avx2_asm:
    ASM_GENERATE_CONST_TABLE BT601_5, avx2

di_color_conversion_yuv_to_bgr_bt709_5_table_avx2_asm:
    ASM_GENERATE_CONST_TABLE BT709_5, avx2

di_color_conversion_yuv_to_bgr_jpeg_fr_table_avx2_asm:
    ASM_GENERATE_CONST_TABLE JPEG_FR, avx2

di_color_conversion_yuv_to_bgr_tables_sse2:
    PTR di_color_conversion_yuv_to_bgr_bt601_5_table_asm
    PTR di_color_conversion_yuv_to_bgr_bt709_5_table_asm
    PTR di_color_conversion_yuv_to_bgr_bt709_5_table_asm ; use 709.5 for 2020
    PTR di_color_conversion_yuv_to_bgr_jpeg_fr_table_asm

di_color_conversion_yuv_to_bgr_tables_avx2:
    PTR di_color_conversion_yuv_to_bgr_bt601_5_table_avx2_asm
    PTR di_color_conversion_yuv_to_bgr_bt709_5_table_avx2_asm
    PTR di_color_conversion_yuv_to_bgr_bt709_5_table_avx2_asm ; use 709.5 for 2020
    PTR di_color_conversion_yuv_to_bgr_jpeg_fr_table_avx2_asm


; code section of file
section .text


%assign COLOR_SPACE_MASK 3

PROC di_get_yuv_to_bgr_constants_sse2_asm, global
    DECL_ARGS PTR, pSource
    ENTER_PROC

    %ifdef _X86

    mov eax, pSource
    movzx eax, word [eax + IMAGE_format_colorSpace] ; load color space id
    and eax, COLOR_SPACE_MASK
    LOAD_ADDRESS ecx, di_color_conversion_yuv_to_bgr_tables_sse2 ; load the constant table
    mov eax, [ecx + PTR_size * eax]

    %elifdef _X64

    movzx eax, word [pSource + IMAGE_format_colorSpace] ; load color space id
    and eax, COLOR_SPACE_MASK
    lea pSource, [di_color_conversion_yuv_to_bgr_tables_sse2] ; load the constant table
    mov rax, [pSource + PTR_size * rax]

    %else ; some other platform

    %fatal unknown platform

    %endif

    LEAVE_PROC

%undef pSource

PROC di_get_yuv_to_bgr_constants_avx2_asm, global
    DECL_ARGS PTR, pSource
    ENTER_PROC

    %ifdef _X86

    mov eax, pSource
    movzx eax, word [eax + IMAGE_format_colorSpace] ; load color space id
    and eax, COLOR_SPACE_MASK
    LOAD_ADDRESS ecx, di_color_conversion_yuv_to_bgr_tables_avx2 ; load the constant table
    mov eax, [ecx + PTR_size * eax]

    %elifdef _X64

    movzx eax, word [pSource + IMAGE_format_colorSpace] ; load color space id
    and eax, COLOR_SPACE_MASK
    lea pSource, [di_color_conversion_yuv_to_bgr_tables_avx2] ; load the constant table
    mov rax, [pSource + PTR_size * rax]

    %else ; some other platform

    %fatal unknown platform

    %endif

    LEAVE_PROC
