; copyright (c) 2013 Victor Cherepanov
; distributed under BSD 3-clause license

%ifndef __DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_YUV_TO_BGR_INC
%xdefine __DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_YUV_TO_BGR_INC

%include "dimanche/image_processing/inc/color_conversion.yuv_to_bgr_tables.x86.inc"

;---------------------------------------------------------------
; Conversion to BGR(X)
;---------------------------------------------------------------

%assign CONST_YUV_TO_BGR_BACK_SCALE 05h

; macro converts 8 YUV pixels to 8 BGR pixels.
; Inputs are scaled on 8 bits:
; xmm0 - U values
; xmm1 - Y values
; xmm2 - V values
; outputs are bytes:
; xmm0 - B values
; xmm1 - G values
; xmm2 - R values

%macro ASM_CONVERT_8_YUV_TO_BGR_SSE2 0
    ; perform YUV->BGR conversion
    pmulhuw xmm1, const_y_for_bgr                               ; y * 1.164 7-0
    movdqa xmm3, xmm0
    pmulhuw xmm0, const_u_for_b                                 ; u * 2.017 7-0
    movdqa xmm7, xmm2
    pmulhuw xmm2, const_v_for_r                                 ; v * 1.596 7-0
    paddsw xmm0, xmm1                                           ; u* += y* 7-0
    pmulhuw xmm3, const_u_for_g                                 ; u * 0.391 7-0
    paddsw xmm2, xmm1                                           ; v* += y* 7-0
    pmulhuw xmm7, const_v_for_g                                 ; v * 0.812 7-0
    psubsw xmm1, xmm3                                           ; y* -= u* 7-0
    psubsw xmm0, const_for_blue                                 ; B* = y* + u* + const 7-0
    psubsw xmm1, xmm7                                           ; y* - u* -= v* 7-0
    psubsw xmm2, const_for_red                                  ; R* = y* + v* + const 7-0
    psraw xmm0, CONST_YUV_TO_BGR_BACK_SCALE                     ; descale b 7-0
    paddsw xmm1, const_for_green                                ; G* = y* - u* - v* + const 7-0
    psraw xmm2, CONST_YUV_TO_BGR_BACK_SCALE                     ; descale r 7-0
    packuswb xmm0, xmm0                                         ; [        bbbbbbbb] 7-0
    psraw xmm1, CONST_YUV_TO_BGR_BACK_SCALE                     ; descale g 7-0
    packuswb xmm1, xmm1                                         ; [        gggggggg] 7-0
    packuswb xmm2, xmm2                                         ; [        rrrrrrrr] 7-0
%endmacro ; ASM_CONVERT_8_YUV_TO_BGR_SSE2


%macro ASM_CONVERT_8_YUV_TO_BGR_SSSE3 0
    ASM_CONVERT_8_YUV_TO_BGR_SSE2
%endmacro ; ASM_CONVERT_8_YUV_TO_BGR_SSSE3


%macro ASM_CONVERT_8_YUV_TO_BGR_AVX2 0
    ; perform YUV->BGR conversion
    vpmulhuw xmm1, xmm1, const_y_for_bgr                        ; y * 1.164 7-0
    vpmulhuw xmm3, xmm0, const_u_for_g                          ; u * 0.391 7-0
    vpmulhuw xmm0, xmm0, const_u_for_b                          ; u * 2.017 7-0
    vpmulhuw xmm7, xmm2, const_v_for_g                          ; v * 0.812 7-0
    vpaddsw xmm0, xmm0, xmm1                                    ; u* += y* 7-0
    vpmulhuw xmm2, xmm2, const_v_for_r                          ; v * 1.596 7-0
    vpaddsw xmm2, xmm2, xmm1                                    ; v* += y* 7-0
    vpsubsw xmm1, xmm1, xmm3                                    ; y* -= u* 7-0
    vpsubsw xmm0, xmm0, const_for_blue                          ; B* = y* + u* + const 7-0
    vpsubsw xmm1, xmm1, xmm7                                    ; y* - u* -= v* 7-0
    vpsubsw xmm2, xmm2, const_for_red                           ; R* = y* + v* + const 7-0
    vpsraw xmm0, xmm0, CONST_YUV_TO_BGR_BACK_SCALE              ; descale b 7-0
    vpaddsw xmm1, xmm1, const_for_green                         ; G* = y* - u* - v* + const 7-0
    vpsraw xmm2, xmm2, CONST_YUV_TO_BGR_BACK_SCALE              ; descale r 7-0
    vpackuswb xmm0, xmm0, xmm0                                  ; [        bbbbbbbb] 7-0
    vpsraw xmm1, xmm1, CONST_YUV_TO_BGR_BACK_SCALE              ; descale g 7-0
    vpackuswb xmm1, xmm1, xmm1                                  ; [        gggggggg] 7-0
    vpackuswb xmm2, xmm2, xmm2                                  ; [        rrrrrrrr] 7-0
%endmacro ; ASM_CONVERT_8_YUV_TO_BGR_AVX2


; macro converts 16 YUV pixels to 16 BGR pixels.
; Inputs are scaled on 8 bits:
; xmm0, xmm4 - U values
; xmm1, xmm5 - Y values
; xmm2, xmm6 - V values
; outputs are bytes:
; xmm0 - B values
; xmm1 - G values
; xmm2 - R values

%macro ASM_CONVERT_16_YUV_TO_BGR_SSE2 0
    ; perform YUV->BGR conversion
    pmulhuw xmm1, const_y_for_bgr                               ; y * 1.164 7-0
    movdqa xmm3, xmm0
    pmulhuw xmm0, const_u_for_b                                 ; u * 2.017 7-0
    movdqa xmm7, xmm2
    pmulhuw xmm2, const_v_for_r                                 ; v * 1.596 7-0
    paddsw xmm0, xmm1                                           ; u* += y* 7-0
    pmulhuw xmm3, const_u_for_g                                 ; u * 0.391 7-0
    paddsw xmm2, xmm1                                           ; v* += y* 7-0
    pmulhuw xmm7, const_v_for_g                                 ; v * 0.812 7-0
    psubsw xmm1, xmm3                                           ; y* -= u* 7-0
    psubsw xmm0, const_for_blue                                 ; B* = y* + u* + const 7-0
    psubsw xmm1, xmm7                                           ; y* - u* -= v* 7-0
    psubsw xmm2, const_for_red                                  ; R* = y* + v* + const 7-0
    psraw xmm0, CONST_YUV_TO_BGR_BACK_SCALE                     ; descale b 7-0
    paddsw xmm1, const_for_green                                ; G* = y* - u* - v* + const 7-0
    psraw xmm2, CONST_YUV_TO_BGR_BACK_SCALE                     ; descale r 7-0
    pmulhuw xmm5, const_y_for_bgr                               ; y * 1.164 f-8
    psraw xmm1, CONST_YUV_TO_BGR_BACK_SCALE                     ; descale g 7-0
    movdqa xmm3, xmm4
    pmulhuw xmm4, const_u_for_b                                 ; u * 2.017 f-8
    movdqa xmm7, xmm6
    pmulhuw xmm6, const_v_for_r                                 ; v * 1.596 f-8
    paddsw xmm4, xmm5                                           ; u* += y* f-8
    pmulhuw xmm3, const_u_for_g                                 ; u * 0.391 f-8
    paddsw xmm6, xmm5                                           ; v* += y* f-8
    pmulhuw xmm7, const_v_for_g                                 ; v * 0.812 f-8
    psubsw xmm5, xmm3                                           ; y* -= u* f-8
    psubsw xmm4, const_for_blue                                 ; B* = y* + u* + const f-8
    psubsw xmm5, xmm7                                           ; y* - u* -= v* f-8
    psubsw xmm6, const_for_red                                  ; R* = y* + v* + const f-8
    psraw xmm4, CONST_YUV_TO_BGR_BACK_SCALE                     ; descale b f-8
    paddsw xmm5, const_for_green                                ; G* = y* - u* - v* + const f-8
    psraw xmm6, CONST_YUV_TO_BGR_BACK_SCALE                     ; descale r f-8
    psraw xmm5, CONST_YUV_TO_BGR_BACK_SCALE                     ; descale g f-8
    packuswb xmm0, xmm4                                         ; [bbbbbbbbbbbbbbbb] f-0
    packuswb xmm1, xmm5                                         ; [gggggggggggggggg] f-0
    packuswb xmm2, xmm6                                         ; [rrrrrrrrrrrrrrrr] f-0
%endmacro ; ASM_CONVERT_16_YUV_TO_BGR_SSE2


%macro ASM_CONVERT_16_YUV_TO_BGR_SSSE3 0
    ASM_CONVERT_16_YUV_TO_BGR_SSE2
%endmacro ; ASM_CONVERT_16_YUV_TO_BGR_SSSE3


%macro ASM_CONVERT_32_YUV_TO_BGR_AVX2 0
    ; perform YUV->BGR conversion
    vpmulhuw ymm1, ymm1, const_y_for_bgr_avx2                   ; y * 1.164 7-0
    vmovdqa ymm3, ymm0
    vpmulhuw ymm0, ymm0, const_u_for_b_avx2                     ; u * 2.017 7-0
    vmovdqa ymm7, ymm2
    vpmulhuw ymm2, ymm2, const_v_for_r_avx2                     ; v * 1.596 7-0
    vpaddsw ymm0, ymm0, ymm1                                    ; u* += y* 7-0
    vpmulhuw ymm3, ymm3, const_u_for_g_avx2                     ; u * 0.391 7-0
    vpaddsw ymm2, ymm2, ymm1                                    ; v* += y* 7-0
    vpmulhuw ymm7, ymm7, const_v_for_g_avx2                     ; v * 0.812 7-0
    vpsubsw ymm1, ymm1, ymm3                                    ; y* -= u* 7-0
    vpsubsw ymm0, ymm0, const_for_blue_avx2                     ; B* = y* + u* + const 7-0
    vpsubsw ymm1, ymm1, ymm7                                    ; y* - u* -= v* 7-0
    vpsubsw ymm2, ymm2, const_for_red_avx2                      ; R* = y* + v* + const 7-0
    vpsraw ymm0, ymm0, CONST_YUV_TO_BGR_BACK_SCALE              ; descale b 7-0
    vpaddsw ymm1, ymm1, const_for_green_avx2                    ; G* = y* - u* - v* + const 7-0
    vpsraw ymm2, ymm2, CONST_YUV_TO_BGR_BACK_SCALE              ; descale r 7-0
    vpmulhuw ymm5, ymm5, const_y_for_bgr_avx2                   ; y * 1.164 f-8
    vpsraw ymm1, ymm1, CONST_YUV_TO_BGR_BACK_SCALE              ; descale g 7-0
    vmovdqa ymm3, ymm4
    vpmulhuw ymm4, ymm4, const_u_for_b_avx2                     ; u * 2.017 f-8
    vmovdqa ymm7, ymm6
    vpmulhuw ymm6, ymm6, const_v_for_r_avx2                     ; v * 1.596 f-8
    vpaddsw ymm4, ymm4, ymm5                                    ; u* += y* f-8
    vpmulhuw ymm3, ymm3, const_u_for_g_avx2                     ; u * 0.391 f-8
    vpaddsw ymm6, ymm6, ymm5                                    ; v* += y* f-8
    vpmulhuw ymm7, ymm7, const_v_for_g_avx2                     ; v * 0.812 f-8
    vpsubsw ymm5, ymm5, ymm3                                    ; y* -= u* f-8
    vpsubsw ymm4, ymm4, const_for_blue_avx2                     ; B* = y* + u* + const f-8
    vpsubsw ymm5, ymm5, ymm7                                    ; y* - u* -= v* f-8
    vpsubsw ymm6, ymm6, const_for_red_avx2                      ; R* = y* + v* + const f-8
    vpsraw ymm4, ymm4, CONST_YUV_TO_BGR_BACK_SCALE              ; descale b f-8
    vpaddsw ymm5, ymm5, const_for_green_avx2                    ; G* = y* - u* - v* + const f-8
    vpsraw ymm6, ymm6, CONST_YUV_TO_BGR_BACK_SCALE              ; descale r f-8
    vpsraw ymm5, ymm5, CONST_YUV_TO_BGR_BACK_SCALE              ; descale g f-8
    vpackuswb ymm0, ymm0, ymm4                                  ; [bbbbbbbbbbbbbbbb] f-0
    vpackuswb ymm1, ymm1, ymm5                                  ; [gggggggggggggggg] f-0
    vpackuswb ymm2, ymm2, ymm6                                  ; [rrrrrrrrrrrrrrrr] f-0
%endmacro ; ASM_CONVERT_32_YUV_TO_BGR_AVX2

; Macros packes 2 pixels' data into the BGRX format and
; writes them to the destination memory.
; Inputs are following:
; xmm0 - B values
; xmm1 - G values
; xmm2 - R values
; xmm3 - A values

%macro ASM_PACK_2_BGRX_PIXELS_SSE2 1
    %push
    %xdefine %$dst %1

    punpcklbw xmm0, xmm1                                        ; [            gbgb] 1-0
    punpcklbw xmm2, xmm3                                        ; [            arar] 1-0
    punpcklwd xmm0, xmm2                                        ; [        argbargb] 1-0
    movq qword [%1 + 00h], xmm0
    %pop
%endmacro ; ASM_PACK_2_BGRX_PIXELS_SSE2


%macro ASM_PACK_2_BGRX_PIXELS_SSSE3 1
    ASM_PACK_2_BGRX_PIXELS_SSE2 %1
%endmacro ; ASM_PACK_2_BGRX_PIXELS_SSSE3


%macro ASM_PACK_2_BGRX_PIXELS_AVX2 1
    %push
    %xdefine %$dst %1

    vpunpcklbw xmm0, xmm0, xmm1                                 ; [            gbgb] 1-0
    vpunpcklbw xmm2, xmm2, xmm3                                 ; [            arar] 1-0
    vpunpcklwd xmm0, xmm0, xmm2                                 ; [        argbargb] 1-0
    vmovq qword [%$dst + 00h], xmm0
    %pop
%endmacro ; ASM_PACK_2_BGRX_PIXELS_AVX2


%macro ASM_PACK_2_BGRA_PIXELS_SSE2 1-2
    %push
    %xdefine %$dst %1
    %xdefine %$alpha_buffer %2

    %ifempty %$alpha_buffer
    pcmpeqw xmm3, xmm3                                          ; set default alpha values
    %else
    pxor xmm3, xmm3                                             ; clean register
    pinsrw xmm3, word [%$alpha_buffer], 00b                     ; load alpha values
    %endif
    ASM_PACK_2_BGRX_PIXELS_SSE2 %$dst
    %pop
%endmacro ; ASM_PACK_2_BGRA_PIXELS_SSE2


%macro ASM_PACK_2_BGRA_PIXELS_SSSE3 1-2
    ASM_PACK_2_BGRA_PIXELS_SSE2 %1, %2
%endmacro ; ASM_PACK_2_BGRA_PIXELS_SSSE3


%macro ASM_PACK_2_BGRA_PIXELS_AVX2 1-2
    %push
    %xdefine %$dst %1
    %xdefine %$alpha_buffer %2

    %ifempty %$alpha_buffer
    vpcmpeqw xmm3, xmm3, xmm3                                   ; set default alpha values
    %else
    vpxor xmm3, xmm3, xmm3                                      ; clean register
    vpinsrw xmm3, xmm3, word [%$alpha_buffer], 00b              ; load alpha values
    %endif
    ASM_PACK_2_BGRX_PIXELS_AVX2 %$dst
    %pop
%endmacro ; ASM_PACK_2_BGRA_PIXELS_AVX2


; Macros packes 4 pixels' data into the BGRX format and
; writes them to the destination memory.
; Inputs are following:
; xmm0 - B values
; xmm1 - G values
; xmm2 - R values
; xmm3 - A values

%macro ASM_PACK_4_BGRX_PIXELS_SSE2 2
    %push
    %xdefine %$store_instr %1
    %xdefine %$dst %2

    punpcklbw xmm0, xmm1                                        ; [        gbgbgbgb] 3-0
    punpcklbw xmm2, xmm3                                        ; [        arararar] 3-0
    punpcklwd xmm0, xmm2                                        ; [argbargbargbargb] 3-0
    %$store_instr oword [%$dst + 00h], xmm0
    %pop
%endmacro ; ASM_PACK_4_BGRX_PIXELS_SSE2


%macro ASM_PACK_4_BGRX_PIXELS_SSSE3 2
    ASM_PACK_4_BGRX_PIXELS_SSE2 %1, %2
%endmacro ; ASM_PACK_4_BGRX_PIXELS_SSSE3


%macro ASM_PACK_4_BGRX_PIXELS_AVX2 2
    %push
    %xdefine %$store_instr %1
    %xdefine %$dst %2

    vpunpcklbw xmm0, xmm0, xmm1                                 ; [        gbgbgbgb] 3-0
    vpunpcklbw xmm2, xmm2, xmm3                                 ; [        arararar] 3-0
    vpunpcklwd xmm0, xmm0, xmm2                                 ; [argbargbargbargb] 3-0
    %$store_instr oword [%$dst + 00h], xmm0
    %pop
%endmacro ; ASM_PACK_4_BGRX_PIXELS_AVX2


%macro ASM_PACK_4_BGRA_PIXELS_SSE2 2-3
    %push
    %xdefine %$store_instr %1
    %xdefine %$dst %2
    %xdefine %$alpha_buffer %3

    %ifempty %$alpha_buffer
    pcmpeqw xmm3, xmm3                                          ; set default alpha values
    %else
    movd xmm3, dword [%$alpha_buffer]                             ; clear register
    %endif
    ASM_PACK_4_BGRX_PIXELS_SSE2 %$store_instr, %$dst
    %pop
%endmacro ; ASM_PACK_4_BGRA_PIXELS_SSE2


%macro ASM_PACK_4_BGRA_PIXELS_SSSE3 2-3
    ASM_PACK_4_BGRA_PIXELS_SSE2 %1, %2, %3
%endmacro ; ASM_PACK_4_BGRA_PIXELS_SSSE3


%macro ASM_PACK_4_BGRA_PIXELS_AVX2 2-3
    %push
    %xdefine %$store_instr %1
    %xdefine %$dst %2
    %xdefine %$alpha_buffer %3

    %ifempty %$alpha_buffer
    vpcmpeqw xmm3, xmm3, xmm3                                   ; set default alpha values
    %else
    vmovd xmm3, dword [%$alpha_buffer]                            ; clear register
    %endif
    ASM_PACK_4_BGRX_PIXELS_AVX2 %$store_instr, %$dst
    %pop
%endmacro ; ASM_PACK_4_BGRA_PIXELS_AVX2


; Macros packes 8 pixels' data into the BGRX format and
; writes them to the destination memory.
; Inputs are following:
; xmm0 - B values
; xmm1 - G values
; xmm2 - R values
; xmm3 - A values

%macro ASM_PACK_8_BGRX_PIXELS_SSE2 2
    %push
    %xdefine %$store_instr %1
    %xdefine %$dst %2

    punpcklbw xmm0, xmm1                                        ; [gbgbgbgbgbgbgbgb] 7-0
    punpcklbw xmm2, xmm3                                        ; [arararararararar] 7-0
    movdqa xmm1, xmm0
    punpcklwd xmm0, xmm2                                        ; [argbargbargbargb] 3-0
    %$store_instr oword [%$dst + 00h], xmm0
    punpckhwd xmm1, xmm2                                        ; [argbargbargbargb] 7-4
    %$store_instr oword [%$dst + 10h], xmm1
    %pop
%endmacro ; ASM_PACK_8_BGRX_PIXELS_SSE2


%macro ASM_PACK_8_BGRX_PIXELS_SSSE3 2
    ASM_PACK_8_BGRX_PIXELS_SSE2 %1, %2
%endmacro ; ASM_PACK_8_BGRX_PIXELS_SSSE3


%macro ASM_PACK_8_BGRA_PIXELS_SSE2 3-4
    %push
    %xdefine %$store_instr %1
    %xdefine %$load_instr %2
    %xdefine %$dst %3
    %xdefine %$alpha_buffer %4

    %ifempty %$alpha_buffer
    pcmpeqw xmm3, xmm3                                          ; set default alpha values
    %else
    movq xmm3, qword [%$alpha_buffer]                         ; load alpha
    %endif
    ASM_PACK_8_BGRX_PIXELS_SSE2 %$store_instr, %$dst
    %pop
%endmacro ; ASM_PACK_8_BGRA_PIXELS_SSE2


%macro ASM_PACK_8_BGRA_PIXELS_SSSE3 3-4
    ASM_PACK_8_BGRA_PIXELS_SSE2 %1, %2, %3, %4
%endmacro ; ASM_PACK_8_BGRA_PIXELS_SSSE3


; Macros packes 16 pixels' data into the BGRX format and
; writes them to the destination memory.
; Inputs are following:
; xmm0 - B values
; xmm1 - G values
; xmm2 - R values
; xmm3 - A values

%macro ASM_PACK_16_BGRX_PIXELS_SSE2 2
    %push
    %xdefine %$store_instr %1
    %xdefine %$dst %2

    movdqa xmm4, xmm0
    punpcklbw xmm0, xmm1                                        ; [gbgbgbgbgbgbgbgb] 7-0
    movdqa xmm6, xmm2
    punpckhbw xmm4, xmm1                                        ; [gbgbgbgbgbgbgbgb] f-8
    movdqa xmm1, xmm0
    punpcklbw xmm2, xmm3                                        ; [arararararararar] 7-0
    movdqa xmm5, xmm4
    punpckhbw xmm6, xmm3                                        ; [arararararararar] f-8
    punpcklwd xmm0, xmm2                                        ; [argbargbargbargb] 3-0
    %$store_instr oword [%$dst + 00h], xmm0
    punpckhwd xmm1, xmm2                                        ; [argbargbargbargb] 7-4
    %$store_instr oword [%$dst + 10h], xmm1
    punpcklwd xmm4, xmm6                                        ; [argbargbargbargb] b-8
    %$store_instr oword [%$dst + 20h], xmm4
    punpckhwd xmm5, xmm6                                        ; [argbargbargbargb] f-c
    %$store_instr oword [%$dst + 30h], xmm5
    %pop
%endmacro ; ASM_PACK_16_BGRX_PIXELS_SSE2


%macro ASM_PACK_16_BGRX_PIXELS_SSSE3 2
    ASM_PACK_16_BGRX_PIXELS_SSE2 %1, %2
%endmacro ; ASM_PACK_16_BGRX_PIXELS_SSSE3


%macro ASM_PACK_32_BGRX_PIXELS_AVX2 2
    %push
    %xdefine %$store_instr %1
    %xdefine %$dst %2

    vpunpckhbw ymm4, ymm0, ymm1                                 ; [gbgbgbgbgbgbgbgb] 1f-18, f-8
    vpunpcklbw ymm0, ymm0, ymm1                                 ; [gbgbgbgbgbgbgbgb] 17-10, 7-0
    vpunpckhbw ymm6, ymm2, ymm3                                 ; [arararararararar] 1f-18, f-8
    vpunpcklbw ymm2, ymm2, ymm3                                 ; [arararararararar] 17-10, 7-0
    vpunpckhwd ymm1, ymm0, ymm2                                 ; [argbargbargbargb] 17-14, 7-4
    vpunpcklwd ymm0, ymm0, ymm2                                 ; [argbargbargbargb] 13-10, 3-0
    vpunpckhwd ymm5, ymm4, ymm6                                 ; [argbargbargbargb] 1f-1c, f-c
    vpunpcklwd ymm4, ymm4, ymm6                                 ; [argbargbargbargb] 1b-18, b-8
    vperm2i128 ymm2, ymm0, ymm1, 00100000b                      ; combine data 7-0
    vperm2i128 ymm3, ymm4, ymm5, 00100000b                      ; combine data f-8
    %$store_instr yword [%$dst + 00h], ymm2
    vperm2i128 ymm0, ymm0, ymm1, 00110001b                      ; combine data 17-10
    %$store_instr yword [%$dst + 20h], ymm3
    vperm2i128 ymm1, ymm4, ymm5, 00110001b                      ; combine data 1f-18
    %$store_instr yword [%$dst + 40h], ymm0
    %$store_instr yword [%$dst + 60h], ymm1
    %pop
%endmacro ; ASM_PACK_32_BGRX_PIXELS_AVX2


%macro ASM_PACK_16_BGRA_PIXELS_SSE2 2-4
    %push
    %xdefine %$store_instr %1
    %xdefine %$load_instr %2
    %xdefine %$dst %3
    %xdefine %$alpha_buffer %4

    %ifempty %$alpha_buffer
    pcmpeqw xmm3, xmm3                                          ; set default alpha values
    %else
    %$load_instr xmm3, oword [%$alpha_buffer]                   ; load alpha
    %endif
    ASM_PACK_16_BGRX_PIXELS_SSE2 %$store_instr, %$dst

    %pop
%endmacro ; ASM_PACK_16_BGRA_PIXELS_SSE2


%macro ASM_PACK_16_BGRA_PIXELS_SSSE3 3-4
    ASM_PACK_16_BGRA_PIXELS_SSE2 %1, %2, %3, %4
%endmacro ; ASM_PACK_16_BGRA_PIXELS_SSSE3


%macro ASM_PACK_32_BGRA_PIXELS_AVX2 3-4
    %push
    %xdefine %$store_instr %1
    %xdefine %$load_instr %2
    %xdefine %$dst %3
    %xdefine %$alpha_buffer %4

    %ifempty %$alpha_buffer
    vpcmpeqw ymm3, ymm3, ymm3                                   ; set default alpha values
    %else
    %$load_instr ymm3, ymmword [%$alpha_buffer]                 ; load alpha
    %endif
    ASM_PACK_32_BGRX_PIXELS_AVX2 %$store_instr, %$dst
    %pop
%endmacro ; ASM_PACK_32_BGRA_PIXELS_AVX2

;
; Macros to convert a few of pixels from YUV to BGRA,
; trying to align the destination pointer.
;

%macro ASM_ALIGN_CONVERT_YUV422_TO_BGRA_8U_SSSE3 3-5

    %push
    %xdefine %$dst %1
    %xdefine %$src %2
    %xdefine %$counter %3
    %xdefine %$alpha_buffer %4
    %xdefine %$row_parity %5

    test TO_DWORD(%$dst), 3fh                                    ; is the destination aligned
    jz %%_END
    cmp %$counter, 04h
    jb %%_END
    jmp %%_NEXT_4
align 10h
%%_RESTART_4:

    ASM_LOAD_2_YUV_PIXELS_8U_SSSE3 %$src, %$row_parity
    ASM_CONVERT_8_YUV_TO_BGR_SSSE3
    ASM_PACK_2_BGRA_PIXELS_SSSE3 %$dst, %$alpha_buffer

    add %$dst, 08h                                                ; advance the destination pointer
    sub %$counter, 02h                                            ; decrement the counter

%%_NEXT_4:
    test TO_DWORD(%$dst), 0fh                                    ; is the destination aligned to 16 bytes ?
    jnz %%_RESTART_4

    jmp %%_NEXT_16
align 10h
%%_RESTART_16:

    ASM_LOAD_4_YUV_PIXELS_8U_SSSE3 %$src, %$row_parity
    ASM_CONVERT_8_YUV_TO_BGR_SSSE3
    ASM_PACK_4_BGRA_PIXELS_SSSE3 movdqa, %$dst, %$alpha_buffer

    add %$dst, 10h                                                ; advance the destination pointer
    sub %$counter, 04h                                            ; decrement the counter

%%_NEXT_16:
    cmp %$counter, 06h                                            ; do we have enough data?
    jb %%_END
    test TO_DWORD(%$dst), 3fh                                    ; is the destination aligned to 64 bytes ?
    jnz %%_RESTART_16
%%_END:

    %pop

%endmacro ; ASM_ALIGN_CONVERT_YUV422_TO_BGRA_8U_SSSE3


%macro ASM_ALIGN_CONVERT_YUV422_TO_BGRA_8U_AVX2 3-5

    %push
    %xdefine %$dst %1
    %xdefine %$src %2
    %xdefine %$counter %3
    %xdefine %$alpha_buffer %4
    %xdefine %$row_parity %5

    test TO_DWORD(%$dst), 3fh                                    ; is the destination aligned
    jz %%_END
    cmp %$counter, 04h
    jb %%_END
    jmp %%_NEXT_4

align 10h
%%_RESTART_4:

    ASM_LOAD_2_YUV_PIXELS_8U_AVX2 %$src, %$row_parity
    ASM_CONVERT_8_YUV_TO_BGR_AVX2
    ASM_PACK_2_BGRA_PIXELS_AVX2 %$dst, %$alpha_buffer

    add %$dst, 08h                                               ; advance the destination pointer
    sub %$counter, 02h                                           ; decrement the counter

align 10h
%%_NEXT_4:
    test TO_DWORD(%$dst), 0fh                                    ; is the destination aligned to 16 bytes ?
    jnz %%_RESTART_4

    jmp %%_NEXT_16
align 10h
%%_RESTART_16:

    ASM_LOAD_4_YUV_PIXELS_8U_AVX2 %$src, %$row_parity
    ASM_CONVERT_8_YUV_TO_BGR_AVX2
    ASM_PACK_4_BGRA_PIXELS_AVX2 vmovdqa, %$dst, %$alpha_buffer

    add %$dst, 10h                                               ; advance the destination pointer
    sub %$counter, 04h                                           ; decrement the counter

align 10h
%%_NEXT_16:
    cmp %$counter, 06h                                           ; do we have enough data?
    jb %%_END
    test TO_DWORD(%$dst), 3fh                                    ; is the destination aligned to 64 bytes ?
    jnz %%_RESTART_16

align 10h
%%_END:

    %pop

%endmacro ; ASM_ALIGN_CONVERT_YUV422_TO_BGRA_8U_AVX2


%macro ASM_CONVERT_YUV422_TO_BGRA_8U_SSSE3 5-8

    %push
    %xdefine %$store_instr %1
    %xdefine %$load_instr %2
    %xdefine %$dst %3
    %xdefine %$src %4
    %xdefine %$counter %5
    %xdefine %$prefetch_ahead %6
    %xdefine %$alpha_buffer %7
    %xdefine %$row_parity %8

    jmp %%_NEXT
ALIGN 10h
%%_RESTART:
    %ifnempty %$prefetch_ahead
    prefetchnta [%$src + %$prefetch_ahead]
    %endif

    ASM_LOAD_16_YUV_PIXELS_8U_SSSE3 %$load_instr, %$src, %$row_parity

    %ifdef src_u
    %ifnempty %$prefetch_ahead
    prefetchnta [src_u + %$prefetch_ahead]
    %endif
    %endif

    ASM_CONVERT_16_YUV_TO_BGR_SSSE3

    %ifdef src_v
    %ifnempty %$prefetch_ahead
    prefetchnta [src_v + %$prefetch_ahead]
    %endif
    %endif

    ASM_PACK_16_BGRA_PIXELS_SSSE3 %$store_instr, movdqa, %$dst, %$alpha_buffer

    add %$dst, 40h                                              ; advance the destination pointer
    sub %$counter, 10h                                          ; decrement counter

%%_NEXT:
    cmp %$counter, 10h                                          ;  do we have enough pixel(s) ?
    ja %%_RESTART

    %pop
%endmacro ; ASM_CONVERT_YUV422_TO_BGRA_8U_SSSE3


%macro ASM_CONVERT_YUV422_TO_BGRA_8U_AVX2 5-8

    %push
    %xdefine %$store_instr %1
    %xdefine %$load_instr %2
    %xdefine %$dst %3
    %xdefine %$src %4
    %xdefine %$counter %5
    %xdefine %$prefetch_ahead %6
    %xdefine %$alpha_buffer %7
    %xdefine %$row_parity %8

    jmp %%_NEXT
ALIGN 10h
%%_RESTART:
    %ifnempty %$prefetch_ahead
    prefetchnta [%$src + %$prefetch_ahead]
    %endif

    ASM_LOAD_32_YUV_PIXELS_8U_AVX2 %$load_instr, %$src, %$row_parity

    %ifdef src_u
    %ifnempty %$prefetch_ahead
    prefetchnta [src_u + %$prefetch_ahead]
    %endif
    %endif

    ASM_CONVERT_32_YUV_TO_BGR_AVX2

    %ifdef src_v
    %ifnempty %$prefetch_ahead
    prefetchnta [src_v + %$prefetch_ahead]
    %endif
    %endif

    ASM_PACK_32_BGRA_PIXELS_AVX2 %$store_instr, vmovdqa, %$dst, %$alpha_buffer

    sub %$dst, -080h                                            ; advance the destination pointer
    sub %$counter, 20h                                          ; decrement counter

ALIGN 10h
%%_NEXT:
    cmp %$counter, 20h                                          ;  do we have enough pixel(s) ?
    ja %%_RESTART

    %pop
%endmacro ; ASM_CONVERT_YUV422_TO_BGRA_8U_AVX2


%macro ASM_REMAIN_CONVERT_YUV422_TO_BGRA_8U_SSSE3 5-7

    %push
    %xdefine %$store_instr %1
    %xdefine %$load_instr %2
    %xdefine %$dst %3
    %xdefine %$src %4
    %xdefine %$counter %5
    %xdefine %$alpha_buffer %6
    %xdefine %$row_parity %7

    jmp %%_NEXT_16
ALIGN 10h
%%_RESTART_16:

    ASM_LOAD_4_YUV_PIXELS_8U_SSSE3 %$src, %$row_parity
    ASM_CONVERT_8_YUV_TO_BGR_SSSE3
    ASM_PACK_4_BGRA_PIXELS_SSSE3 %$store_instr, %$dst, %$alpha_buffer

    add %$dst, 10h                                              ; advance the destination pointer
    sub %$counter, 04h                                          ; decrement the counter

%%_NEXT_16:
    cmp %$counter, 04h                                          ; do we have enough data?
    ja %%_RESTART_16

    jmp %%_NEXT_4
ALIGN 10h
%%_RESTART_4:

    ASM_LOAD_2_YUV_PIXELS_8U_SSSE3 %$src, %$row_parity
    ASM_CONVERT_8_YUV_TO_BGR_SSSE3
    ASM_PACK_2_BGRA_PIXELS_SSSE3 %$dst, %$alpha_buffer

    add %$dst, 08h                                              ; advance the destination pointer
    sub %$counter, 02h                                          ; decrement the counter

ALIGN 10h
%%_NEXT_4:
    cmp counter, 02h                                            ; do we have enough data?
    ja %%_RESTART_4
    jb %%_END

    ASM_LOAD_LAST_2_YUV_PIXELS_8U_SSSE3 %$src, %$row_parity
    ASM_CONVERT_8_YUV_TO_BGR_SSSE3
    ASM_PACK_2_BGRA_PIXELS_SSSE3 %$dst, %$alpha_buffer

    add %$dst, 08h                                              ; advance the destination pointer
    sub %$counter, 02h                                          ; decrement the counter

ALIGN 10h
%%_END:

    %pop
%endmacro ; ASM_REMAIN_CONVERT_YUV422_TO_BGRA_8U_SSSE3


%macro ASM_REMAIN_CONVERT_YUV422_TO_BGRA_8U_AVX2 5-7

    %push
    %xdefine %$store_instr %1
    %xdefine %$load_instr %2
    %xdefine %$dst %3
    %xdefine %$src %4
    %xdefine %$counter %5
    %xdefine %$alpha_buffer %6
    %xdefine %$row_parity %7

    jmp %%_NEXT_16
ALIGN 10h
%%_RESTART_16:

    ASM_LOAD_4_YUV_PIXELS_8U_AVX2 %$src, %$row_parity
    ASM_CONVERT_8_YUV_TO_BGR_AVX2
    ASM_PACK_4_BGRA_PIXELS_AVX2 %$store_instr, %$dst, %$alpha_buffer

    add %$dst, 10h                                              ; advance the destination pointer
    sub %$counter, 04h                                          ; decrement the counter

ALIGN 10h
%%_NEXT_16:
    cmp %$counter, 04h                                          ; do we have enough data?
    ja %%_RESTART_16

    jmp %%_NEXT_4
ALIGN 10h
%%_RESTART_4:

    ASM_LOAD_2_YUV_PIXELS_8U_AVX2 %$src, %$row_parity
    ASM_CONVERT_8_YUV_TO_BGR_AVX2
    ASM_PACK_2_BGRA_PIXELS_AVX2 %$dst, %$alpha_buffer

    add %$dst, 08h                                              ; advance the destination pointer
    sub %$counter, 02h                                          ; decrement the counter

ALIGN 10h
%%_NEXT_4:
    cmp %$counter, 02h                                          ; do we have enough data?
    ja %%_RESTART_4
    jb %%_END

    ASM_LOAD_LAST_2_YUV_PIXELS_8U_AVX2 %$src, %$row_parity
    ASM_CONVERT_8_YUV_TO_BGR_AVX2
    ASM_PACK_2_BGRA_PIXELS_AVX2 %$dst, %$alpha_buffer

    add %$dst, 08h                                              ; advance the destination pointer
    sub %$counter, 02h                                          ; decrement the counter

ALIGN 10h
%%_END:

    %pop
%endmacro ; ASM_REMAIN_CONVERT_YUV422_TO_BGRA_8U_AVX2


%endif ; __DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_YUV_TO_BGR_INC

