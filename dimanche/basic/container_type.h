
#pragma once

#if !defined(__DIMANCHE_BASIC_CONTAINER_TYPES_H)
#define __DIMANCHE_BASIC_CONTAINER_TYPES_H

#include <dimanche/basic/types.h>

#include <string>

namespace dimanche {
namespace container {

enum class eType : uint32_t
{
    // the container has unrecognized type
    UNK = 0,
    // audio video interleaved (AVI) container file
    AVI,
    // apple QuickTime container file
    MOV,
    // MPEG4 container file
    MP4,
    // MPEG2 program stream
    PS,
    // pure DV/DVCPRO/DVCPRO50/DVCPRO100 video in the file
    PURE_DV,
    // pure H264 video in the file
    PURE_H264,
    // MPEG2 transport stream
    TS,
    // raw YCbCr data file
    Y4M,
    // raw YCbCr data file
    YUV,
    // pure AAC ADIF file
    PURE_AAC_ADIF,
    // pure AAC ADTS file
    PURE_AAC_ADTS,
    // waveform (WAV) file
    WAV,
    // pure HEVC video in the file
    PURE_HEVC,
    //  Indeo Video Format
    IVF,

    NUM
};

eType GetTypeByPath(std::wstring &path);

} // container
} // namespace dimanche

#endif // !defined(__DIMANCHE_BASIC_CONTAINER_TYPES_H)
