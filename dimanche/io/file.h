
#pragma once

#if !defined(__DIMANCHE_IO_FILE_H)
#define __DIMANCHE_IO_FILE_H

#include <dimanche/basic/result.h>
#include <dimanche/basic/types.h>

#include <memory>
#include <tuple>

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
eFlag operator | (const eFlag left, const eFlag right) {
    return (eFlag) ((uint32_t) left | (uint32_t) right);
}
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

// close the file
void close(handle h);

struct closer
{
    void operator () (handle h) const {
        close(h);
    }
};

using unique_file_t = std::unique_ptr<_handle_t, closer>;

// create or open a file
unique_file_t open(const std::wstring &path,
    const eAccess accessType, const eAccess shareMode,
    const eDisposition creationDisposition, const eFlag flags);
unique_file_t open(const std::string &path,
    const eAccess accessType, const eAccess shareMode,
    const eDisposition creationDisposition, const eFlag flags);

// close the overlapped control
void close_overlapped_control(OVRLPD * const pOverlapped);

struct overlapped_control_closer
{
    void operator () (OVRLPD * const pOverlapped) const {
        close_overlapped_control(pOverlapped);
    }
};

// initialize an overlapped control unit
std::unique_ptr<OVRLPD, overlapped_control_closer> initialize_overlapped_control();

// reads data from a file, starts at the position
// that the file pointer indicates. returns the number of bytes read
std::tuple<eResult, size_t> read(handle h, void *pBuffer,
    const size_t bytesToRead, OVRLPD * const pOverlapped = nullptr);

// writes data to the specified file at the position
// that specified by the file pointer. returns the number of written bytes
std::tuple<eResult, size_t> write(handle h, const void *pBuffer,
    const size_t bytesToWrite, OVRLPD * const pOverlapped = nullptr);

// moves the file pointer of the specified file, return the current position
std::tuple<eResult, uint64_t> set_pointer(handle h, int64_t distanceToMove,
    const ePosition moveMethod);

// wait for the end of I/O operation
eResult wait_io(handle h, OVRLPD * const pOverlapped,
    size_t &processedBytes);

// retrieves the size of the file specified
std::tuple<eResult, uint64_t> get_size(handle h);
std::tuple<eResult, uint64_t> get_size(const std::wstring &path);
std::tuple<eResult, uint64_t> get_size(const std::string &path);

// retrieves information about the specified disk, which holds the given file
std::tuple<eResult, size_t> get_sector_size(const std::wstring &path);
std::tuple<eResult, size_t> get_sector_size(const std::string &path);

// cancel all I/O operations with this file
eResult cancel_io(handle h);

} // namespace file
} // namespace dimanche

// declare the library name
#if defined(_WINDOWS)
#pragma comment (lib, "io")
#endif // defined(_WINDOWS)

#endif // !defined(__DIMANCHE_IO_FILE_H)
