
#pragma once

#ifndef __DIMANCHE_BASIC_ASSERT_H
#define __DIMANCHE_BASIC_ASSERT_H

#include <dimanche/basic/platform.h>

/* defines for automatic debugging while code developing.
   file covers differences between platforms. */

#ifdef _WINDOWS

#include <crtdbg.h>

#define di_assert(expr) \
    _ASSERTE(expr)

#else /* !_WINDOWS */

#include <assert.h>

#define di_assert(expr) \
    assert(expr)

#endif /* _WINDOWS */

#ifdef _DEBUG

/* show simple warnings in debug configuration */
#define di_warning(message) \
    { \
        int message = 0; \
        di_assert(message); \
    }

#else /* !_DEBUG */

/* no warning messages in release configuration */
#define di_warning(message)

#endif /* _DEBUG */

#endif /* __DIMANCHE_BASIC_ASSERT_H */
