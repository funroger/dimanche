
#pragma once

#if !defined(__DIMANCHE_VM_FILE_H)
#define __DIMANCHE_VM_FILE_H

#include <dimanche/basic/result.h>
#include <dimanche/basic/types.h>

namespace dimanche {
namespace file {

// file access types
enum class eAccess : uint32_t
{
    None = 0,

    Read = 1,
    Write = 2
};

inline
eAccess operator & (const eAccess left, const eAccess right) {
    return (eAccess) ((uint32_t) left & (uint32_t) right);
}

// file creation dispositions
enum class eDisposition : uint32_t
{
    None = 0,

    // creates a new file, only if it does not already exist. If the specified
    // file exists, the function fails.
    CreateNew,

    // creates a new file, always. If the specified file exists and is writable,
    // the function overwrites the file.
    CreateAlways,

    // opens a file or device, only if it exists. If the specified file does
    // not exist, the function fails.
    OpenExisting,

    // opens a file, always. If the specified file exists, the function succeeds.
    // if the specified file does not exist and is a valid path to a writable
    // location, the function creates a file.
    OpenAlways
};

// file flags
enum class eFlag : uint32_t
{
    None = 0,
    Normal = 0x0001,
    ReadOnly = 0x0002,
    WriteThrough = 0x0004,
    Overlapped = 0x0008,
    NoBuffering = 0x0010,
    DeleteOnClose = 0x0020
};

inline
eFlag operator & (const eFlag left, const eFlag right) {
    return (eFlag) ((uint32_t) left & (uint32_t) right);
}

// types of moving file pointer methods
enum class ePosition : uint32_t
{
    Begin,
    Current,
    End
};

// forward declaration of overlapped structure
struct OVRLPD;

// create or open a file
eResult open(handle &h, const wchar_t *pwFile,
    const eAccess accessType, const eAccess shareMode,
    const eDisposition creationDisposition, const eFlag flags);
eResult open(handle &h, const char *pcFile,
    const eAccess accessType, const eAccess shareMode,
    const eDisposition creationDisposition, const eFlag flags);

// initialize an overlapped control unit
eResult initialize_overlapped_control(OVRLPD *(&pOverlapped));

// reads data from a file, and starts at the position
// that the file pointer indicates
eResult read(handle h, void *pBuffer,
    const size_t bytesToRead, size_t *pReadBytes = nullptr,
    OVRLPD * const pOverlapped = nullptr);

// writes data to the specified file at the position
// that specified by the file pointer
eResult write(handle h, const void *pBuffer,
    const size_t bytesToWrite, size_t *pWrittenBytes = nullptr,
    OVRLPD * const pOverlapped = nullptr);

// moves the file pointer of the specified file
eResult set_pointer(handle h, int64_t distanceToMove,
    uint64_t *pNewPosition, const ePosition moveMethod);

// wait for the end of I/O operation
eResult wait_io(handle h, OVRLPD * const pOverlapped,
    size_t &processedBytes);

// retrieves the size of the file specified
eResult get_size(handle h, uint64_t &fileSize);
eResult get_size(const wchar_t *pwFile, uint64_t &fileSize);
eResult get_size(const char *pcFile, uint64_t &fileSize);

// retrieves information about the specified disk, which holds the given file
eResult get_sector_size(const wchar_t *pwFile, size_t &sectorSize);
eResult get_sector_size(const char *pcFile, size_t &sectorSize);

// cancel all I/O operations with this file
eResult cancel_io(handle h);

// close the overlapped control
eResult close_overlapped_control(OVRLPD * const pOverlapped);

// close the file
eResult close(handle h);

struct closer
{
    void operator () (handle h) const {
        close(h);
    }
};

} // namespace file
} // namespace dimanche

#endif // !defined(__DIMANCHE_VM_FILE_H)
