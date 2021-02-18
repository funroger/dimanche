// copyright (c) 2013 Victor Cherepanov
// distributed under BSD 3-clause license

#include <dimanche/image_processing/inc/color_conversion_internal.h>

//using namespace SoftLabSoftware::color_conversion::yuv_to_bgr;

namespace dimanche {

#define CREATE_BGRA_PIXEL(index, blue, green, red) \
    CREATE_BGRX_PIXEL(index, blue, green, red, 255)

#define CREATE_BGRX_PIXEL(index, blue, green, red, alpha) \
{ \
    pDst[(index) * 4 + 0] = (uint8_t) (blue); \
    pDst[(index) * 4 + 1] = (uint8_t) (green); \
    pDst[(index) * 4 + 2] = (uint8_t) (red); \
    pDst[(index) * 4 + 3] = (uint8_t) (alpha); \
}

using color_conversion::yuv_to_bgr::Converter;

namespace {

void convert_nv12_to_bgra_even_row_c(uint8_t *pDst,
    const uint8_t *pSrcL,  const uint8_t *pSrcUV,
    const size_t width, const Converter c)
{
    size_t x;
    for (x = 0; x + 2 < width; x += 2) {
        int32_t b, g, r;

        uint32_t luma = pSrcL[x + 0];
        const uint32_t u = pSrcUV[x + 0];
        const uint32_t v = pSrcUV[x + 1];
 
        b = c.B(0, luma, u);
        g = c.G(0, luma, u, v);
        r = c.R(0, luma, v);
        CREATE_BGRA_PIXEL(x + 0, b, g, r);

        luma = pSrcL[x + 1];
        const uint32_t u1 = pSrcUV[x + 2];
        const uint32_t v1 = pSrcUV[x + 3];

        b = c.B(1, luma, u + u1);
        g = c.G(1, luma, u + u1, v + v1);
        r = c.R(1, luma, v + v1);
        CREATE_BGRA_PIXEL(x + 1, b, g, r);
    }

    // process last 2 pixels
    if (x < width) {
        int32_t b, g, r;

        uint32_t luma = pSrcL[x + 0];
        const uint32_t u = pSrcUV[x + 0];
        const uint32_t v = pSrcUV[x + 1];

        b = c.B(0, luma, u);
        g = c.G(0, luma, u, v);
        r = c.R(0, luma, v);
        CREATE_BGRA_PIXEL(x + 0, b, g, r);

        luma = pSrcL[x + 1];

        b = c.B(0, luma, u);
        g = c.G(0, luma, u, v);
        r = c.R(0, luma, v);
        CREATE_BGRA_PIXEL(x + 1, b, g, r);
    }

} // void convert_nv12_to_bgra_row(uint8_t *pDst,

void convert_nv12_to_bgra_odd_row_c(uint8_t *pDst,
    const uint8_t *pSrcL,  const uint8_t *pSrcUV, const ptrdiff_t srcStrideUV,
    const size_t width, const Converter c)
{
    const uint8_t *pSrcUVNext = image::Advance(pSrcUV, srcStrideUV);

    size_t x;
    for (x = 0; x + 2 < width; x += 2) {
        int32_t b, g, r;

        uint32_t luma = pSrcL[x + 0];
        const uint32_t u = pSrcUV[x + 0];
        const uint32_t v = pSrcUV[x + 1];
        const uint32_t u2 = pSrcUVNext[x + 0];
        const uint32_t v2 = pSrcUVNext[x + 1];

        b = c.B(1, luma, u + u2);
        g = c.G(1, luma, u + u2, v + v2);
        r = c.R(1, luma, v + v2);
        CREATE_BGRA_PIXEL(x + 0, b, g, r);

        luma = pSrcL[x + 1];
        const uint32_t u1 = pSrcUV[x + 2];
        const uint32_t v1 = pSrcUV[x + 3];
        const uint32_t u3 = pSrcUVNext[x + 2];
        const uint32_t v3 = pSrcUVNext[x + 3];

        b = c.B(2, luma, u + u1 + u2 + u3);
        g = c.G(2, luma, u + u1 + u2 + u3, v + v1 + v2 + v3);
        r = c.R(2, luma, v + v1 + v2 + v3);
        CREATE_BGRA_PIXEL(x + 1, b, g, r);
    }

    // process last 2 pixels
    if (x < width) {
        int32_t b, g, r;

        uint32_t luma = pSrcL[x + 0];
        const uint32_t u = pSrcUV[x + 0];
        const uint32_t v = pSrcUV[x + 1];
        const uint32_t u2 = pSrcUVNext[x + 0];
        const uint32_t v2 = pSrcUVNext[x + 1];

        b = c.B(1, luma, u + u2);
        g = c.G(1, luma, u + u2, v + v2);
        r = c.R(1, luma, v + v2);
        CREATE_BGRA_PIXEL(x + 0, b, g, r);

        luma = pSrcL[x + 1];

        b = c.B(1, luma, u + u2);
        g = c.G(1, luma, u + u2, v + v2);
        r = c.R(1, luma, v + v2);
        CREATE_BGRA_PIXEL(x + 1, b, g, r);
    }

} // void convert_nv12_to_bgra_odd_row_c(uint8_t *pDst,

} // namespace

extern "C"
void di_convert_nv12_to_bgra_c(COLOR_CONVERSION_FORMAL_PARAMS)
{
    Converter c(color_conversion::yuv_to_bgr::GetConstants(pSrcImage->format));

    const Dimension dim = pDstImage->format.dim;

    const uint8_t *pSrcL = (const uint8_t *) pSrcImage->planes[0].p;
    const ptrdiff_t srcStrideL = pSrcImage->planes[0].stride;
    const uint8_t *pSrcUV = (const uint8_t *) pSrcImage->planes[1].p;
    const ptrdiff_t srcStrideUV = pSrcImage->planes[1].stride;

    // change the destination image's orientation
    const ptrdiff_t dstStride = -pDstImage->planes[0].stride;
    uint8_t *pDst = (uint8_t *) pDstImage->planes[0].p -
        (dim.Height() - 1) * dstStride;

    for (uint32_t y = 0; y + 2 < dim.Height(); y += 2)
    {
        convert_nv12_to_bgra_even_row_c(pDst, pSrcL, pSrcUV, dim.Width(), c);

        pSrcL = image::Advance(pSrcL, srcStrideL);
        pDst = image::Advance(pDst, dstStride);

        convert_nv12_to_bgra_odd_row_c(pDst, pSrcL, pSrcUV, srcStrideUV,
            dim.Width(), c);

        pSrcL = image::Advance(pSrcL, srcStrideL);
        pSrcUV = image::Advance(pSrcUV, srcStrideUV);
        pDst = image::Advance(pDst, dstStride);
    }

    // process last 2 rows
    if (2 <= dim.Height())
    {
        convert_nv12_to_bgra_even_row_c(pDst, pSrcL, pSrcUV, dim.Width(), c);

        pSrcL = image::Advance(pSrcL, srcStrideL);
        pDst = image::Advance(pDst, dstStride);

        convert_nv12_to_bgra_even_row_c(pDst, pSrcL, pSrcUV, dim.Width(), c);

        // advance pointers
        pSrcL = image::Advance(pSrcL, srcStrideL);
        pSrcUV = image::Advance(pSrcUV, srcStrideUV);
        pDst = image::Advance(pDst, dstStride);
    }

} // void di_convert_nv12_to_bgra_c(COLOR_CONVERSION_FORMAL_PARAMS)

} // namespace dimanche
