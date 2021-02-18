; copyright (c) 2013 Victor Cherepanov
; distributed under BSD 3-clause license

%ifndef __COLOR_CONVERSION_YUV_TO_BGR_TABLES_X86_INC
%define __COLOR_CONVERSION_YUV_TO_BGR_TABLES_X86_INC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; constant tables for YUV to BGR conversions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

extern di_get_yuv_to_bgr_constants_sse2_asm
extern di_get_yuv_to_bgr_constants_avx2_asm

%ifndef SIMD

;
; SSE constant table declaration
;

%xdefine di_get_yuv_to_bgr_constants di_get_yuv_to_bgr_constants_sse2_asm

%xdefine const_upper_byte_mask [table + 00h]
%xdefine const_y_for_bgr [table + 10h]
%xdefine const_u_for_b [table + 20h]
%xdefine const_v_for_r [table + 30h]
%xdefine const_u_for_g [table + 40h]
%xdefine const_v_for_g [table + 50h]
%xdefine const_for_blue [table + 60h]
%xdefine const_for_red [table + 70h]
%xdefine const_for_green [table + 080h]
%xdefine const_shuffle_chrominance_uyvy [table + 090h]
%xdefine const_shuffle_chrominance_yuy2 [table + 0a0h]
%xdefine const_shuffle_chrominance_yvyu [table + 0b0h]
%xdefine const_filter_chrominance [table + 0c0h]
;%xdefine const_shuffle_bg_0 oword [table + 0d0h]
;%xdefine const_shuffle_bg_1 oword [table + 0e0h]
;%xdefine const_shuffle_bg_2 oword [table + 0f0h]
;%xdefine const_shuffle_r_0 oword [table + 100h]
;%xdefine const_shuffle_r_1 oword [table + 110h]
;%xdefine const_shuffle_r_2 oword [table + 120h]
;%xdefine const_shuffle_luminance_y16 oword [table + 130h]
;%xdefine const_shuffle_chrominance_lower_y16 oword [table + 140h]
;%xdefine const_shuffle_chrominance_upper_y16 oword [table + 150h]
;%xdefine const_shuffle_chrominance_last_y16 oword [table + 160h]
;%xdefine const_shuffle_luminance_y41p oword [table + 170h]
;%xdefine const_shuffle_cb_7_0_y41p oword [table + 180h]
;%xdefine const_filter_chrominance_y41p oword [table + 190h]
;%xdefine const_shuffle_cr_7_0_y41p oword [table + 1a0h]
;%xdefine const_shuffle_cb_f_8_y41p oword [table + 1b0h]
;%xdefine const_shuffle_cr_f_8_y41p oword [table + 1c0h]
;%xdefine const_shuffle_chrominance_y422 oword [table + 1d0h]
;%xdefine const_shuffle_chrominance_lower_y422 const_shuffle_chrominance_lower_y16 ; the same as for Y16
;%xdefine const_shuffle_chrominance_upper_y422 oword [table + 1e0h]
%xdefine const_shuffle_chrominance_nv12 [table + 1f0h]

%elifidni SIMD, AVX2

;
; AVX constant table declaration
;

%xdefine di_get_yuv_to_bgr_constants di_get_yuv_to_bgr_constants_avx2_asm

%xdefine const_upper_byte_mask oword [table + 00h]
%xdefine const_y_for_bgr oword [table + 20h]
%xdefine const_u_for_b oword [table + 40h]
%xdefine const_v_for_r oword [table + 60h]
%xdefine const_u_for_g oword [table + 080h]
%xdefine const_v_for_g oword [table + 0a0h]
%xdefine const_for_blue oword [table + 0c0h]
%xdefine const_for_red oword [table + 0e0h]
%xdefine const_for_green oword [table + 100h]
%xdefine const_shuffle_chrominance_uyvy oword [table + 120h]
%xdefine const_shuffle_chrominance_yuy2 oword [table + 140h]
%xdefine const_shuffle_chrominance_yvyu oword [table + 160h]
%xdefine const_filter_chrominance oword [table + 180h]
;%xdefine const_shuffle_chrominance_lower_y422 oword [table + 1a0h]
;%xdefine const_shuffle_chrominance_upper_y422 oword [table + 1c0h]
%xdefine const_shuffle_chrominance_nv12 oword [table + 1e0h]

%xdefine const_upper_byte_mask_avx2 yword [table + 00h]
%xdefine const_y_for_bgr_avx2 yword [table + 20h]
%xdefine const_u_for_b_avx2 yword [table + 40h]
%xdefine const_v_for_r_avx2 yword [table + 60h]
%xdefine const_u_for_g_avx2 yword [table + 080h]
%xdefine const_v_for_g_avx2 yword [table + 0a0h]
%xdefine const_for_blue_avx2 yword [table + 0c0h]
%xdefine const_for_red_avx2 yword [table + 0e0h]
%xdefine const_for_green_avx2 yword [table + 100h]
%xdefine const_shuffle_chrominance_uyvy_avx2 yword [table + 120h]
%xdefine const_shuffle_chrominance_yuy2_avx2 yword [table + 140h]
%xdefine const_shuffle_chrominance_yvyu_avx2 yword [table + 160h]
%xdefine const_filter_chrominance_avx2 yword [table + 180h]
;%xdefine const_shuffle_chrominance_lower_y422_avx2 yword [table + 1a0h]
;%xdefine const_shuffle_chrominance_upper_y422_avx2 yword [table + 1c0h]
%xdefine const_shuffle_chrominance_nv12_avx2 yword [table + 1e0h]

%else

%fatal unknown SIMD type

%endif

%endif ; __COLOR_CONVERSION_YUV_TO_BGR_TABLES_X86_INC
