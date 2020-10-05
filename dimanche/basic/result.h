
#pragma once

#if !defined(__DIMANCHE_BASIC_RESULT_H)
#define __DIMANCHE_BASIC_RESULT_H

#include <dimanche/basic/types.h>

namespace dimanche {

enum class eResult : int32_t {
    OK = 0,

    // warnings are positive values
    WRN_NOT_READY = 2,
    WRN_NEED_MORE_DATA = 1,

    // errors are negative values
    ERR_NULLPTR = -1,
    ERR_BAD_FORMAT = -2,
    ERR_INV_PARAM = -3,
    ERR_EXCEPTION = -4,
    ERR_FAILED = -5,
    ERR_NOT_IMPL = -6,
    ERR_BAD_PATH = -7,
    ERR_ALLOC = -8,
    ERR_BAD_SIZE = -9,
    ERR_END_OF_STREAM = -10,
    ERR_NOT_SUPPORTED = -11
};

} // namespace dimanche

#endif // __DIMANCHE_BASIC_RESULT_H
