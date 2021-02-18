; copyright (c) 2013 Victor Cherepanov
; distributed under BSD 3-clause license

%ifndef __DIMANCHE_BASIC_IMAGE_X86_INC
%define __DIMANCHE_BASIC_IMAGE_X86_INC

%include "dimanche/basic/asm_defs.x86.inc"

%assign bgra_bpp 4

struc DIMENSION
    .width: resd 1
    .height: resd 1
endstruc


struc FORMAT
    .colorFormat: resd 1
    .dim: resb DIMENSION_size
    .colorSpace: resw 1
    .bitDepthMinus8: resb 1
    .bitDepthChromaMinus8: resb 1
endstruc


struc PLANE
    .p: resptr 1
    .stride: resptr 1
endstruc


struc IMAGE
    .format: resb FORMAT_size
    .planes: resb PLANE_size * 4
endstruc


%define IMAGE_format_colorFormat \
    IMAGE.format + FORMAT.colorFormat
%define IMAGE_format_dim_width \
    IMAGE.format + FORMAT.dim + DIMENSION.width
%define IMAGE_format_dim_height \
    IMAGE.format + FORMAT.dim + DIMENSION.height
%define IMAGE_format_colorSpace \
    IMAGE.format + FORMAT.colorSpace
%define IMAGE_planes_p(idx) \
    IMAGE.planes + PLANE.p + (idx) * PTR_size * 2
%define IMAGE_planes_stride(idx) \
    IMAGE.planes + PLANE.stride + (idx) * PTR_size * 2


%endif ; __DIMANCHE_BASIC_IMAGE_X86_INC
