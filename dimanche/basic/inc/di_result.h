
#pragma once

#if !defined(__DI_RESULT_H)
#define __DI_RESULT_H

#include <di_types.h>

namespace dimanche {

enum class eResult : int32_t {
    OK = 0,

    // warnings are positive values
    WRN_NEED_MORE_DATA = 1,

    // errors are negative values
    ERR_NULLPTR = -1,
    ERR_BAD_FORMAT = -2,
    ERR_INV_PARAM = -3,
    ERR_EXCEPTION = -4,
    ERR_FAILED = -5
};

} // namespace dimanche

#endif // __DI_RESULT_H
