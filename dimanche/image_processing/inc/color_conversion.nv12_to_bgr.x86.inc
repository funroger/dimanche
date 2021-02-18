; copyright (c) 2013 Victor Cherepanov
; distributed under BSD 3-clause license

%ifndef __DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_NV12_TO_BGR_INC
%xdefine __DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_NV12_TO_BGR_INC

%include "dimanche/image_processing/inc/color_conversion.yuv_to_bgr.x86.inc"

;---------------------------------------------------------------
; Conversion from NV12 to BGRA
;---------------------------------------------------------------

; Macro loads 2 YUV pixels, filter odd values and scales the data.
; The data are loaded into the following registers:
; xmm0 - U values
; xmm1 - Y values
; xmm2 - V values

%macro ASM_LOAD_2_YUV_PIXELS_8U_SSSE3 1-2
    %push
    %xdefine %$src %1
    %xdefine %$row_parity %2

    pxor xmm3, xmm3                                             ; clear register
    pxor xmm2, xmm2                                             ; clear register
    pinsrw xmm3, word [src_luma], 000b                      ; load two Y values
    add %$src, 02h                                                ; advance source Y pointer
    pxor xmm1, xmm1                                             ; clear register
    movd xmm2, dword [src_chroma]                           ; load UV values
    %ifidni %$row_parity, odd
    movd xmm1, dword [src_chroma + stride_chroma]           ; load UV values next row
    punpckldq xmm2, xmm1                                        ; combine UV values
    pxor xmm1, xmm1                                             ; clear register
    %endif
    add src_chroma, 02h                                         ; advance source UV pointer
    pxor xmm0, xmm0                                             ; clear register
    punpcklbw xmm1, xmm3                                        ; unpack Y to words
    punpcklbw xmm0, xmm2                                        ; unpack chrominance to words
    pshufd xmm2, xmm0, CLONE_EVEN_DWORDS                        ; copy even values
    pavgw xmm0, xmm2                                            ; filter chrominance horizontally
    %ifidni %$row_parity, odd
    pshufd xmm2, xmm0, SWAP_QWORDS                              ; access chrominance values next row
    pavgw xmm0, xmm2                                            ; filter chrominance vertically
    %endif
    pshuflw xmm2, xmm0, 10001101b                               ; copy V values
    pshuflw xmm0, xmm0, 11011000b                               ; reorder U values

    %pop
%endmacro ; ASM_LOAD_2_YUV_PIXELS_8U_SSSE3


%macro ASM_LOAD_2_YUV_PIXELS_8U_AVX2 1-2
    %push
    %xdefine %$src %1
    %xdefine %$row_parity %2

    vpxor xmm3, xmm3, xmm3                                      ; clear register
    vpxor xmm2, xmm2, xmm2                                      ; clear register
    vpinsrw xmm3, xmm3, word [src_luma], 000b               ; load two Y values
    add %$src, 02h                                                ; advance source Y pointer
    vpxor xmm1, xmm1, xmm1                                      ; clear register
    vmovd xmm2, dword [src_chroma]                          ; load UV values
    %ifidni %$row_parity, odd
    vpinsrd xmm2, xmm2, dword [src_chroma + stride_chroma], 01b ; load UV values next row
    %endif
    add src_chroma, 02h                                         ; advance source UV pointer
    vpxor xmm0, xmm0, xmm0                                      ; clear register
    vpunpcklbw xmm1, xmm1, xmm3                                 ; unpack Y to words
    vpunpcklbw xmm0, xmm0, xmm2                                 ; unpack chrominance to words
    vpshufd xmm2, xmm0, CLONE_EVEN_DWORDS                       ; copy even values
    vpavgw xmm0, xmm0, xmm2                                     ; filter chrominance horizontally
    %ifidni %$row_parity, odd
    vpshufd xmm2, xmm0, SWAP_QWORDS                             ; access chrominance values next row
    vpavgw xmm0, xmm0, xmm2                                     ; filter chrominance vertically
    %endif
    vpshuflw xmm2, xmm0, 10001101b                              ; copy V values
    vpshuflw xmm0, xmm0, 11011000b                              ; reorder U values

    %pop
%endmacro ; ASM_LOAD_2_YUV_PIXELS_8U_AVX2


; Macro loads 2 YUV pixels and scales the data.
; The data are loaded into the following registers:
; xmm0 - U values
; xmm1 - Y values
; xmm2 - V values

%macro ASM_LOAD_LAST_2_YUV_PIXELS_8U_SSSE3 1-2
    %push
    %xdefine %$src %1
    %xdefine %$row_parity %2

    pxor xmm3, xmm3                                             ; clear register
    pxor xmm2, xmm2                                             ; clear register
    pinsrw xmm3, word [src_luma], 000b                      ; load two Y values
    add %$src, 02h                                                ; advance source Y pointer
    pxor xmm1, xmm1                                             ; clear register
    pinsrw xmm2, word [src_chroma], 000b                    ; load UV values
    %ifidni %$row_parity, odd
    pinsrw xmm2, word [src_chroma + stride_chroma], 010b    ; load UV values next row
    %endif
    add src_chroma, 02h                                         ; advance source UV pointer
    pxor xmm0, xmm0                                             ; clear register
    punpcklbw xmm1, xmm3                                        ; unpack Y to words
    punpcklbw xmm0, xmm2                                        ; unpack chrominance to words
    pshufd xmm0, xmm0, CLONE_EVEN_DWORDS                        ; copy even values
    %ifidni %$row_parity, odd
    pshufd xmm2, xmm0, SWAP_QWORDS                              ; access chrominance values next row
    pavgw xmm0, xmm2                                            ; filter chrominance vertically
    %endif
    pshuflw xmm2, xmm0, 10001101b                               ; copy V values
    pshuflw xmm0, xmm0, 11011000b                               ; reorder U values

    %pop
%endmacro ; ASM_LOAD_LAST_2_YUV_PIXELS_8U_SSSE3


%macro ASM_LOAD_LAST_2_YUV_PIXELS_8U_AVX2 1-2
    %push
    %xdefine %$src %1
    %xdefine %$row_parity %2

    vpxor xmm3, xmm3, xmm3                                      ; clear register
    vpxor xmm2, xmm2, xmm2                                      ; clear register
    vpinsrw xmm3, xmm3, word [src_luma], 000b               ; load two Y values
    add %$src, 02h                                                ; advance source Y pointer
    vpxor xmm1, xmm1, xmm1                                      ; clear register
    vpinsrw xmm2, xmm2, word [src_chroma], 000b             ; load UV values
    %ifidni %$row_parity, odd
    vpinsrw xmm2, xmm2, word [src_chroma + stride_chroma], 010b ; load UV values next row
    %endif
    add src_chroma, 02h                                         ; advance source UV pointer
    vpxor xmm0, xmm0, xmm0                                      ; clear register
    vpunpcklbw xmm1, xmm1, xmm3                                 ; unpack Y to words
    vpunpcklbw xmm0, xmm0, xmm2                                 ; unpack chrominance to words
    vpshufd xmm0, xmm0, CLONE_EVEN_DWORDS                       ; copy even values
    %ifidni %$row_parity, odd
    vpshufd xmm2, xmm0, SWAP_QWORDS                             ; access chrominance values next row
    vpavgw xmm0, xmm0, xmm2                                     ; filter chrominance vertically
    %endif
    vpshuflw xmm2, xmm0, 10001101b                              ; copy V values
    vpshuflw xmm0, xmm0, 11011000b                              ; reorder U values

    %pop
%endmacro ; ASM_LOAD_LAST_2_YUV_PIXELS_8U_AVX2


; Macro loads 4 YUV pixels, filter odd values and scales the data.
; The data are loaded into the following registers:
; xmm0 - U values
; xmm1 - Y values
; xmm2 - V values

%macro ASM_LOAD_4_YUV_PIXELS_8U_SSSE3 1-2
    %push
    %xdefine %$src %1
    %xdefine %$row_parity %2

    ; load luminance
    pxor xmm1, xmm1                                             ; clear register
    movd xmm3, dword [%$src + 00h]                            ; load Y values
    add %$src, 04h                                                ; advance source Y pointer

    ; load chrominance
    movd xmm2, dword [src_chroma + 00h]                     ; load UV values
    pinsrw xmm2, word [src_chroma + 04h], 010b              ; load UV values
    %ifidni %$row_parity, odd
    movd xmm6, dword [src_chroma + stride_chroma + 00h]     ; load UV values next row
    pinsrw xmm6, word [src_chroma + stride_chroma + 04h], 010b ; load UV values next row
    %endif
    add src_chroma, 04h                                         ; advance source UV pointer

    pxor xmm0, xmm0                                             ; clear register
    punpcklbw xmm0, xmm2                                        ; unpack chrominance to words
    %ifidni %$row_parity, odd
    pxor xmm4, xmm4                                             ; clear register
    punpcklbw xmm4, xmm6                                        ; unpack chrominance to words
    pavgw xmm0, xmm4                                            ; filter chrominance vertically
    %endif

    punpcklbw xmm1, xmm3                                        ; unpack Y to words

    pshuflw xmm2, xmm0, CLONE_ODD_WORDS                         ; reorder V
    pshuflw xmm0, xmm0, CLONE_EVEN_WORDS                        ; reorder U
    movdqa xmm4, xmm0
    pshufhw xmm6, xmm2, CLONE_ODD_WORDS
    psrldq xmm4, 02h                                            ; prepare U values for filtering
    psrldq xmm6, 02h                                            ; prepare V values for filtering
    pavgw xmm0, xmm4                                            ; filter chrominance values
    pavgw xmm2, xmm6                                            ; filter chrominance values

    %pop
%endmacro ; ASM_LOAD_4_YUV_PIXELS_8U_SSSE3


%macro ASM_LOAD_4_YUV_PIXELS_8U_AVX2 1-2
    %push
    %xdefine %$src %1
    %xdefine %$row_parity %2

    ; load luminance
    vpxor xmm1, xmm1, xmm1                                      ; clear register
    vmovd xmm3, dword [%$src + 00h]                           ; load Y values
    add %$src, 04h                                                ; advance source Y pointer

    ; load chrominance
    vmovd xmm2, dword [src_chroma + 00h]                    ; load UV values
    vpinsrw xmm2, xmm2, word [src_chroma + 04h], 010b       ; load UV values
    %ifidni %$row_parity, odd
    vmovd xmm6, dword [src_chroma + stride_chroma + 00h]    ; load UV values next row
    vpinsrw xmm6, xmm6, word [src_chroma + stride_chroma + 04h], 010b ; load UV values next row
    %endif
    add src_chroma, 04h                                         ; advance source UV pointer

    vpxor xmm0, xmm0, xmm0                                      ; clear register
    vpunpcklbw xmm0, xmm0, xmm2                                 ; unpack chrominance to words
    %ifidni %$row_parity, odd
    vpxor xmm4, xmm4, xmm4                                      ; clear register
    vpunpcklbw xmm4, xmm4, xmm6                                 ; unpack chrominance to words
    vpavgw xmm0, xmm0, xmm4                                     ; filter chrominance vertically
    %endif

    vpunpcklbw xmm1, xmm1, xmm3                                 ; unpack Y to words

    vpshuflw xmm2, xmm0, CLONE_ODD_WORDS                        ; reorder V
    vpshuflw xmm0, xmm0, CLONE_EVEN_WORDS                       ; reorder U
    vpshufhw xmm6, xmm2, CLONE_ODD_WORDS
    vpsrldq xmm4, xmm0, 02h                                     ; prepare U values for filtering
    vpsrldq xmm6, xmm6, 02h                                     ; prepare V values for filtering
    vpavgw xmm0, xmm0, xmm4                                     ; filter chrominance values
    vpavgw xmm2, xmm2, xmm6                                     ; filter chrominance values

    %pop
%endmacro ; ASM_LOAD_4_YUV_PIXELS_8U_AVX2


; Macro loads 16 YUV pixels, filter odd values and scales the data.
; The data are loaded into the following registers:
; xmm0, xmm4 - U values
; xmm1, xmm5 - Y values
; xmm2, xmm6 - V values

%macro ASM_LOAD_16_YUV_PIXELS_8U_SSSE3 2-3
    %push
    %xdefine %$load_instr %1
    %xdefine %$src %2
    %xdefine %$row_parity %3

    ; load UV values
    movdqa xmm7, const_filter_chrominance                       ; load the const
    pxor xmm4, xmm4                                             ; clean register
    %$load_instr xmm0, oword [src_chroma + 00h]                 ; load UV values
    pinsrw xmm4, word [src_chroma + 10h], 000b                  ; load UV values
    %ifidni %$row_parity, odd
    pxor xmm5, xmm5                                             ; clean register
    %$load_instr xmm1, oword [src_chroma + stride_chroma + 00h] ; load UV values
    pinsrw xmm5, word [src_chroma + stride_chroma + 10h], 000b  ; load UV values
    %endif
    add src_chroma, 10h                                         ; advance source UV pointer

    palignr xmm4, xmm0, 08h                                     ; combine next data
    palignr xmm2, xmm0, 01h                                     ; access V data
    pshufb xmm0, const_shuffle_chrominance_nv12                 ; blend chrominance bytes
    palignr xmm6, xmm4, 01h                                     ; access V data
    pshufb xmm4, const_shuffle_chrominance_nv12                 ; blend chrominance bytes
    pmaddubsw xmm0, xmm7                                        ; filter chrominance values
    pshufb xmm2, const_shuffle_chrominance_nv12                 ; blend chrominance bytes
    pmaddubsw xmm4, xmm7                                        ; filter chrominance values
    psllw xmm0, 07h                                             ; scale chrominance values
    pshufb xmm6, const_shuffle_chrominance_nv12                 ; blend chrominance bytes
    pmaddubsw xmm2, xmm7                                        ; filter chrominance values
    psllw xmm4, 07h                                             ; scale chrominance values
    pmaddubsw xmm6, xmm7                                        ; filter chrominance values
    psllw xmm2, 07h                                             ; scale chrominance values
    psllw xmm6, 07h                                             ; scale chrominance values

    %ifidni %$row_parity, odd
    palignr xmm5, xmm1, 08h                                     ; combine next data
    palignr xmm3, xmm1, 01h                                     ; access V data
    pshufb xmm1, const_shuffle_chrominance_nv12                 ; blend chrominance bytes
    pshufb xmm3, const_shuffle_chrominance_nv12                 ; blend chrominance bytes
    pmaddubsw xmm1, xmm7                                        ; filter chrominance values
    pmaddubsw xmm3, xmm7                                        ; filter chrominance values
    psllw xmm1, 07h                                             ; scale chrominance values
    psllw xmm3, 07h                                             ; scale chrominance values
    pavgw xmm0, xmm1                                            ; filter chrominance vertically
    palignr xmm1, xmm5, 01h                                     ; access V data
    pshufb xmm5, const_shuffle_chrominance_nv12                 ; blend chrominance bytes
    pavgw xmm2, xmm3                                            ; filter chrominance vertically
    pshufb xmm1, const_shuffle_chrominance_nv12                 ; blend chrominance bytes
    pmaddubsw xmm5, xmm7                                        ; filter chrominance values
    pmaddubsw xmm1, xmm7                                        ; filter chrominance values
    psllw xmm5, 07h                                             ; scale chrominance values
    psllw xmm1, 07h                                             ; scale chrominance values
    pavgw xmm4, xmm5                                            ; filter chrominance vertically
    pavgw xmm6, xmm1                                            ; filter chrominance vertically
    %endif

    ; load luminance
    pxor xmm1, xmm1                                             ; clear register
    %$load_instr xmm3, oword [%$src + 00h]                      ; load Y values
    add %$src, 10h                                              ; advance the source pointer
    pxor xmm5, xmm5                                             ; clear register
    punpcklbw xmm1, xmm3                                        ; unpack & scale Y values
    punpckhbw xmm5, xmm3                                        ; unpack & scale Y values

    %pop
%endmacro ; ASM_LOAD_16_YUV_PIXELS_8U_SSSE3


%macro ASM_LOAD_32_YUV_PIXELS_8U_AVX2 2-3
    %push
    %xdefine %$load_instr %1
    %xdefine %$src %2
    %xdefine %$row_parity %3

    ; load UV values
    vbroadcasti128 ymm7, const_filter_chrominance               ; load the const
    %$load_instr ymm0, yword [src_chroma + 00h]                 ; load UV values
    vpbroadcastw xmm4, word [src_chroma + 20h]                  ; load UV values
    vperm2i128 ymm4, ymm0, ymm4, 00100001b                      ; combine data
    %ifidni %$row_parity, odd
    %$load_instr ymm1, yword [src_chroma + stride_chroma + 00h] ; load UV values
    vpbroadcastw xmm5, word [src_chroma + stride_chroma + 20h]  ; load UV values
    vperm2i128 ymm5, ymm1, ymm5, 00100001b                      ; combine data
    %endif
    add src_chroma, 20h                                         ; advance source UV pointer

    vpalignr ymm4, ymm4, ymm0, 08h                              ; combine next data
    vpalignr ymm2, ymm0, ymm0, 01h                              ; access V data
    vpshufb ymm0, ymm0, const_shuffle_chrominance_nv12_avx2     ; blend chrominance bytes
    vpalignr ymm6, ymm4, ymm4, 01h                              ; access V data
    vpshufb ymm4, ymm4, const_shuffle_chrominance_nv12_avx2     ; blend chrominance bytes
    vpmaddubsw ymm0, ymm0, ymm7                                 ; filter chrominance values
    vpshufb ymm2, ymm2, const_shuffle_chrominance_nv12_avx2     ; blend chrominance bytes
    vpmaddubsw ymm4, ymm4, ymm7                                 ; filter chrominance values
    vpsllw ymm0, ymm0, 07h                                      ; scale chrominance values
    vpshufb ymm6, ymm6, const_shuffle_chrominance_nv12_avx2     ; blend chrominance bytes
    vpmaddubsw ymm2, ymm2, ymm7                                 ; filter chrominance values
    vpsllw ymm4, ymm4, 07h                                      ; scale chrominance values
    vpmaddubsw ymm6, ymm6, ymm7                                 ; filter chrominance values
    vpsllw ymm2, ymm2, 07h                                      ; scale chrominance values
    vpsllw ymm6, ymm6, 07h                                      ; scale chrominance values

    %ifidni %$row_parity, odd
    vpalignr ymm5, ymm5, ymm1, 08h                              ; combine next data
    vpalignr ymm3, ymm3, ymm1, 01h                              ; access V data
    vpshufb ymm1, ymm1, const_shuffle_chrominance_nv12_avx2     ; blend chrominance bytes
    vpshufb ymm3, ymm3, const_shuffle_chrominance_nv12_avx2     ; blend chrominance bytes
    vpmaddubsw ymm1, ymm1, ymm7                                 ; filter chrominance values
    vpmaddubsw ymm3, ymm3, ymm7                                 ; filter chrominance values
    vpsllw ymm1, ymm1, 07h                                      ; scale chrominance values
    vpsllw ymm3, ymm3, 07h                                      ; scale chrominance values
    vpavgw ymm0, ymm0, ymm1                                     ; filter chrominance vertically
    vpalignr ymm1, ymm1, ymm5, 01h                              ; access V data
    vpshufb ymm5, ymm5, const_shuffle_chrominance_nv12_avx2     ; blend chrominance bytes
    vpavgw ymm2, ymm2, ymm3                                     ; filter chrominance vertically
    vpshufb ymm1, ymm1, const_shuffle_chrominance_nv12_avx2     ; blend chrominance bytes
    vpmaddubsw ymm5, ymm5, ymm7                                 ; filter chrominance values
    vpmaddubsw ymm1, ymm1, ymm7                                 ; filter chrominance values
    vpsllw ymm5, ymm5, 07h                                      ; scale chrominance values
    vpsllw ymm1, ymm1, 07h                                      ; scale chrominance values
    vpavgw ymm4, ymm4, ymm5                                     ; filter chrominance vertically
    vpavgw ymm6, ymm6, ymm1                                     ; filter chrominance vertically
    %endif

    ; load luminance
    vpxor xmm5, xmm5, xmm5                                      ; clear register
    %$load_instr ymm3, yword [%$src + 00h]                      ; load Y values
    add %$src, 20h                                              ; advance the source pointer
    vpunpcklbw ymm1, ymm5, ymm3                                 ; unpack & scale Y values
    vpunpckhbw ymm5, ymm5, ymm3                                 ; unpack & scale Y values

    %pop
%endmacro ; ASM_LOAD_32_YUV_PIXELS_8U_AVX2


%endif ; __DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_NV12_TO_BGR_INC
