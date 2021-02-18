; copyright (c) 2013 Victor Cherepanov
; distributed under BSD 3-clause license

%ifndef __DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_NV12_INC
%define __DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_NV12_INC

%include "dimanche/basic/asm_defs.x86.inc"
%include "dimanche/basic/image.x86.inc"

%ifdef _X86

    ; x86 definition section

    %define temp eax
    %define table eax
    %define src_chroma ebx
    %define counter ecx
    %define stride_chroma edx
    %define src_luma esi
    %define dst edi
    %define stack esp
    %define alpha_buffer esp

    %define LOCAL_VARS PTRDIFF_T,dstInc, PTRDIFF_T,srcInc, DWORD,imageWidth, \
        DWORD,imageHeight, PTRDIFF_T, chromaStride
    %define USED_REGS ebx, esi, edi

    %macro LOAD_POINTERS 1-2
        %push
        %assign %$dst_bytes_per_pixel %1
        %define %$change_direction %2

        ; create increments
        mov temp, pDstImage                                     ; load pointer to the destination image
        mov counter, [temp + IMAGE_format_dim_height]           ; load image's height
        mov imageHeight, counter                                ; save image's height
        mov counter, [temp + IMAGE_format_dim_width]            ; load image's width
        mov imageWidth, counter                                 ; save image's width

        %ifempty %$change_direction

        neg counter                                             ; create the negative width for substraction
        mov dst, [temp + IMAGE_planes_stride(0)]                ; load destination step
        %if %$dst_bytes_per_pixel != 3
        lea dst, [dst + counter * %$dst_bytes_per_pixel]         ; create the destination pointer increment
        %else
        lea dst, [dst + counter * 2]                            ; create the destination pointer increment
        lea dst, [dst + counter]                                ; create the destination pointer increment
        %endif
        mov dstInc, dst                                         ; store destination pointer increment
        mov dst, [temp + IMAGE_planes_p(0)]                     ; load destination pointer

        %else ; change destination image direction

        mov dst, [temp + IMAGE_planes_stride(0)]                ; load destination step
        %if %$dst_bytes_per_pixel != 3
        lea dst, [dst + counter * %$dst_bytes_per_pixel]         ; create the destination pointer increment
        %else
        lea dst, [dst + counter * 2]                            ; create the destination pointer increment
        lea dst, [dst + counter]                                ; create the destination pointer increment
        %endif
        neg dst                                                 ; make step negative
        mov dstInc, dst                                         ; store destination pointer increment
        mov dst, [temp + IMAGE_planes_p(0)]                     ; load destination pointer
        mov temp, [temp + IMAGE_planes_stride(0)]               ; load destination step
        mov counter, imageHeight                                ; load image height
        sub counter, 01h                                        ; decrement the hight to set the last row
        imul temp, counter                                      ; calculate the destination pointer's advance
        add dst, temp                                           ; advance destination pointer

        %endif

        mov counter, imageWidth                                 ; load image's width
        neg counter                                             ; create the negative width for substraction

        mov temp, pSrcImage                                     ; load pointer to the source image
        mov src_luma, [temp + IMAGE_planes_stride(0)]           ; load source step
        add src_luma, counter                                   ; create the source pointer increment
        mov srcInc, src_luma                                    ; store source pointer increment
        mov stride_chroma, [temp + IMAGE_planes_stride(1)]      ; load chrominance stride
        mov chromaStride, stride_chroma                         ; save chrominance stride
        mov src_luma, [temp + IMAGE_planes_p(0)]                ; load source pointer
        mov src_chroma, [temp + IMAGE_planes_p(1)]              ; load UV pointer

        push temp
        call di_get_yuv_to_bgr_constants
        add stack, 04h

        mov counter, imageWidth                                 ; load image's width

        %pop
    %endmacro ; LOAD_POINTERS

%elifdef _WINDOWS
    ; x64 Windows definition section

    %define temp rax
    %define table rax
    %define src_chroma rbx
    %xdefine counter rcx
    %define stride_chroma rdx
    %define src_luma rsi
    %define dst rdi
    %define stack rsp
    %define alpha_buffer rsp
    %define srcInc r8
    %define imageWidth r9d
    %define imageHeight r10d
    %define dstInc r11

    %xdefine LOCAL_VARS PTR,pConstTable, PTR,chromaStride
    %xdefine USED_REGS rbx, rsi, rdi, xmm6, xmm7

    %macro LOAD_POINTERS 1-2
        %push
        %assign %$dst_bytes_per_pixel %1
        %define %$change_direction %2

        push rcx
        mov rcx, pSrcImage
        call di_get_yuv_to_bgr_constants
        pop rcx
        mov pConstTable, temp ; save pointer to the table

        ; create increments
        mov temp, pDstImage                                     ; load pointer to the destination image
        mov imageHeight, [temp + IMAGE_format_dim_height]       ; load image's height
        mov TO_DWORD(counter), [temp + IMAGE_format_dim_width]  ; load image's width
        mov imageWidth, TO_DWORD(counter)                       ; save image's width

        %ifempty %$change_direction

        neg counter                                             ; create the negative width for substraction
        mov dst, [temp + IMAGE_planes_stride(0)]                ; load destination step
        %if %$dst_bytes_per_pixel != 3
        lea dst, [dst + counter * %$dst_bytes_per_pixel]         ; create the destination pointer increment
        %else
        lea dst, [dst + counter * 2]                            ; create the destination pointer increment
        lea dst, [dst + counter]                                ; create the destination pointer increment
        %endif
        mov dstInc, dst                                         ; store destination pointer increment
        mov dst, [temp + IMAGE_planes_p(0)]                     ; load destination pointer

        %else ; change destination image direction

        mov dst, [temp + IMAGE_planes_stride(0)]                ; load destination step
        %if %$dst_bytes_per_pixel != 3
        lea dst, [dst + counter * %$dst_bytes_per_pixel]         ; create the destination pointer increment
        %else
        lea dst, [dst + counter * 2]                            ; create the destination pointer increment
        lea dst, [dst + counter]                                ; create the destination pointer increment
        %endif
        neg dst                                                 ; make step negative
        mov dstInc, dst                                         ; store destination pointer increment
        mov dst, [temp + IMAGE_planes_p(0)]                     ; load destination pointer
        mov temp, [temp + IMAGE_planes_stride(0)]               ; load destination step
        mov TO_DWORD(counter), imageHeight                      ; load image height
        sub TO_DWORD(counter), 01h                              ; decrement the hight to set the last row
        imul temp, counter                                      ; calculate the destination pointer's advance
        add dst, temp                                           ; advance destination pointer

        %endif

        mov TO_DWORD(counter), imageWidth                       ; load image's width
        neg counter                                             ; create the negative width for substraction

        mov temp, pSrcImage                                     ; load pointer to the source image
        mov src_luma, [temp + IMAGE_planes_stride(0)]           ; load source step
        add src_luma, counter                                   ; create the source pointer increment
        mov srcInc, src_luma                                    ; store source pointer increment
        mov stride_chroma, [temp + IMAGE_planes_stride(1)]      ; load chrominance stride
        mov chromaStride, stride_chroma                         ; store chrominance stride
        mov src_luma, [temp + IMAGE_planes_p(0)]                ; load source pointer
        mov src_chroma, [temp + IMAGE_planes_p(1)]              ; load chrominance pointer

        mov TO_DWORD(counter), imageWidth                       ; load image's width
        mov table, pConstTable                                  ; load pointer to the constant table

        %pop
    %endmacro ; LOAD_POINTERS

%elifdef _LINUX
    ; x64 Linux definition section

    %define temp rax
    %define table rax
    %define src_chroma rbx
    %xdefine counter rcx
    %define stride_chroma rdx
    %define src_luma rsi
    %define dst rdi
    %define stack rsp
    %define alpha_buffer rsp
    %define srcInc r8
    %define imageWidth r9d
    %define imageHeight r10d
    %define dstInc r11

    %xdefine LOCAL_VARS PTR,pConstTable, PTR,chromaStride
    %xdefine USED_REGS rbx

    %macro LOAD_POINTERS 1-2
        %push
        %assign %$dst_bytes_per_pixel %1
        %define %$change_direction %2

        push rdi
        mov rdi, pSrcImage
        call di_get_yuv_to_bgr_constants
        pop rdi
        mov pConstTable, temp ; save pointer to the table

        ; create increments
        mov temp, pDstImage                                     ; load pointer to the destination image
        mov imageHeight, [temp + IMAGE_format_dim_height]       ; load image's height
        mov TO_DWORD(counter), [temp + IMAGE_format_dim_width]  ; load image's width
        mov imageWidth, TO_DWORD(counter)                       ; save image's width

        %ifempty %$change_direction

        neg counter                                             ; create the negative width for substraction
        mov dst, [temp + IMAGE_planes_stride(0)]                ; load destination step
        %if %$dst_bytes_per_pixel != 3
        lea dst, [dst + counter * %$dst_bytes_per_pixel]         ; create the destination pointer increment
        %else
        lea dst, [dst + counter * 2]                            ; create the destination pointer increment
        lea dst, [dst + counter]                                ; create the destination pointer increment
        %endif
        mov dstInc, dst                                         ; store destination pointer increment
        mov dst, [temp + IMAGE_planes_p(0)]                     ; load destination pointer

        %else ; change destination image direction

        mov dst, [temp + IMAGE_planes_stride(0)]                ; load destination step
        %if %$dst_bytes_per_pixel != 3
        lea dst, [dst + counter * %$dst_bytes_per_pixel]         ; create the destination pointer increment
        %else
        lea dst, [dst + counter * 2]                            ; create the destination pointer increment
        lea dst, [dst + counter]                                ; create the destination pointer increment
        %endif
        neg dst                                                 ; make step negative
        mov dstInc, dst                                         ; store destination pointer increment
        mov dst, [temp + IMAGE_planes_p(0)]                     ; load destination pointer
        mov temp, [temp + IMAGE_planes_stride(0)]               ; load destination step
        mov TO_DWORD(counter), imageHeight                      ; load image height
        sub TO_DWORD(counter), 01h                              ; decrement the hight to set the last row
        imul temp, counter                                      ; calculate the destination pointer's advance
        add dst, temp                                           ; advance destination pointer

        %endif

        mov TO_DWORD(counter), imageWidth                       ; load image's width
        neg counter                                             ; create the negative width for substraction

        mov temp, pSrcImage                                     ; load pointer to the source image
        mov src_luma, [temp + IMAGE_planes_stride(0)]           ; load source step
        add src_luma, counter                                   ; create the source pointer increment
        mov srcInc, src_luma                                    ; store source pointer increment
        mov stride_chroma, [temp + IMAGE_planes_stride(1)]      ; load chrominance stride
        mov chromaStride, stride_chroma                         ; store chrominance stride
        mov src_luma, [temp + IMAGE_planes_p(0)]                ; load source pointer
        mov src_chroma, [temp + IMAGE_planes_p(1)]              ; load chrominance pointer

        mov TO_DWORD(counter), imageWidth                       ; load image's width
        mov table, pConstTable                                  ; load pointer to the constant table

        %pop
    %endmacro ; LOAD_POINTERS

%endif ; _X86

%endif ; __DIMANCHE_IMAGE_PROCESSING_COLOR_CONVERSION_NV12_INC
