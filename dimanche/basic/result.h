
#pragma once

#if !defined(__DIMANCHE_BASIC_RESULT_H)
#define __DIMANCHE_BASIC_RESULT_H

#include <dimanche/basic/types.h>

#include <string>

namespace dimanche {

enum class eResult : int32_t {
    OK = 0,

    // errors are negative values
    ERR_CALL_CONTEXT = -20,
    ERR_ALIGNMENT = -12,
    ERR_NOT_SUPPORTED = -11,
    ERR_END_OF_STREAM = -10,
    ERR_BAD_SIZE = -9,
    ERR_ALLOC = -8,
    ERR_BAD_PATH = -7,
    ERR_NOT_IMPL = -6,
    ERR_FAILED = -5,
    ERR_EXCEPTION = -4,
    ERR_INV_PARAM = -3,
    ERR_BAD_FORMAT = -2,
    ERR_NULLPTR = -1,

    // warnings are positive values
    WRN_NEED_MORE_DATA = 1,
    WRN_NOT_READY = 2,
    WRN_BUSY = 3,
    WRN_UNKNOWN = 4

};

const std::wstring GetErrorDescription(const eResult result);

} // namespace dimanche

// declare the library name
#if defined(_WINDOWS)
#pragma comment (lib, "basic")
#endif // defined(_WINDOWS)

#endif // __DIMANCHE_BASIC_RESULT_H
