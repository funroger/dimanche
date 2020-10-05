
#include <dimanche/vm/file.h>

#include <dimanche/basic/assert.h>

#include <memory>

#if defined(_WINDOWS)

#include <windows.h>

#else // !defined(_WINDOWS)

#endif // defined(_WINDOWS)

namespace dimanche {
namespace file {

namespace {

#if defined(_WINDOWS)

template <typename char_t, typename function_t>
eResult open(handle &hFile, const char_t *pFile,
    const eAccess accessType, const eAccess shareMode,
    const eDisposition creationDisposition, const eFlag flags,
    function_t function)
{
    HANDLE h = nullptr;
    DWORD dwDesiredAccess = 0;
    DWORD dwShareMode = 0;
    DWORD dwCreationDisposition = 0;
    DWORD dwFlagsAndAttributes = 0;

    // set file access type
    if (eAccess::None != (accessType & eAccess::Read)) {
        dwDesiredAccess |= GENERIC_READ;
    }
    if (eAccess::None != (accessType & eAccess::Write)) {
        dwDesiredAccess |= GENERIC_WRITE;
    }

    // set file share mode
    if (eAccess::None != (shareMode & eAccess::Read)) {
        dwShareMode |= FILE_SHARE_READ;
    }
    if (eAccess::None != (shareMode & eAccess::Write)) {
        dwShareMode |= FILE_SHARE_WRITE;
    }

    // set creation type
    if (eDisposition::CreateNew == creationDisposition) {
        dwCreationDisposition = CREATE_NEW;
    } else if (eDisposition::CreateAlways == creationDisposition) {
        dwCreationDisposition = CREATE_ALWAYS;
    } else if (eDisposition::OpenExisting == creationDisposition) {
        dwCreationDisposition = OPEN_EXISTING;
    } else if (eDisposition::OpenAlways == creationDisposition) {
        dwCreationDisposition = OPEN_ALWAYS;
    }

    // set file opening flags
    if (eFlag::None != (flags & eFlag::Normal)) {
        dwFlagsAndAttributes |= FILE_ATTRIBUTE_NORMAL;
    }
    if (eFlag::None != (flags & eFlag::ReadOnly)) {
        dwFlagsAndAttributes |= FILE_ATTRIBUTE_READONLY;
    }
    if (eFlag::None != (flags & eFlag::WriteThrough)) {
        dwFlagsAndAttributes |= FILE_FLAG_WRITE_THROUGH;
    }
    if (eFlag::None != (flags & eFlag::Overlapped)) {
        dwFlagsAndAttributes |= FILE_FLAG_OVERLAPPED;
    }
    if (eFlag::None != (flags & eFlag::NoBuffering)) {
        dwFlagsAndAttributes |= FILE_FLAG_NO_BUFFERING;
    }
    if (eFlag::None != (flags & eFlag::DeleteOnClose)) {
        dwFlagsAndAttributes |= FILE_FLAG_DELETE_ON_CLOSE;
    }

    // try to open the file
    h = function(pFile, dwDesiredAccess, dwShareMode, nullptr,
        dwCreationDisposition, dwFlagsAndAttributes, nullptr);
    if (INVALID_HANDLE_VALUE == h) {
        return eResult::ERR_BAD_PATH;
    }
    hFile = (handle) h;

    return eResult::OK;

} // eResult open(handle &hFile, const wchar_t *pwFile,

#else // !defined(_WINDOWS)

char *sl_get_file_name_w(const wchar_t *pwFile)
{
    size_t written = 0, size = 128;
    char *cFile = nullptr;

    // check error(s)
    if (nullptr == pwFile) {
        return nullptr;
    }

    do
    {
        const wchar_t *pw = pwFile;

        // deallocate the previously allocated path
        if (cFile) {
            free(cFile);
        }

        // allocate file path
        size *= 2;
        cFile = malloc(size);
        if (nullptr == cFile) {
            return nullptr;
        }
        written = wcsrtombs(cFile, &pw, size, nullptr);

    } while (written >= size);

    return cFile;

} // char *sl_get_file_name_w(const wchar_t *pwFile)

int sl_test_file_c(const char *pcFile, const uint32_t fileAccessType)
{
    // check error(s)
    if (nullptr == pcFile) {
        return 0;
    }

    if (eAccess::Read & fileAccessType)
    {
        FILE *f;

        // try to open the file
        f = fopen(pcFile, "rb");
        if (nullptr == f) {
            return 0;
        }

        // release resources
        fclose(f);
    }
    else
    {
        FILE *f;
        uint64_t pos;

        // try to open the file
        f = fopen(pcFile, "ab");
        if (nullptr == f) {
            return 0;
        }

        // get the position
        pos = ftell(f);

        // release resources
        fclose(f);

        // file was not exist before
        if (0 == pos) {
            return 0;
        }
    }

    return 1;

} // int sl_test_file_c(const char *pcFile, const uint32_t fileAccessType)

#endif // defined(_WINDOWS)

} // namespace


eResult open(handle &hFile, const wchar_t *pwFile,
    const eAccess accessType, const eAccess shareMode,
    const eDisposition creationDisposition, const eFlag flags)
{
    // check error(s)
    if (nullptr == pwFile) {
        return eResult::ERR_NULLPTR;
    }

#if defined(_WINDOWS)

    return open(hFile, pwFile, accessType, shareMode,
        creationDisposition, flags, CreateFileW);

#else // !defined(_WINDOWS)

    {
        char *cFile = sl_get_file_name_w(pwFile);
        eResult res;

        // check error(s)
        if (nullptr == cFile) {
            return eResult::ERR_BAD_PATH;
        }

        // open the file
        res = open(phFile, cFile, fileAccessType, fileShareMode,
            fileCreationDisposition, fileFlags);
        // deallocate the file path
        free(cFile);
        if (eStatus::OK != res) {
            return res;
        }
    }

    return eResult::OK;

#endif // defined(_WINDOWS)

} // eResult open(handle &hFile, const wchar_t *pwFile,

eResult open(handle &hFile, const char *pcFile,
    const eAccess accessType, const eAccess shareMode,
    const eDisposition creationDisposition, const eFlag flags)
{
    // check error(s)
    if (nullptr == pcFile) {
        return eResult::ERR_NULLPTR;
    }

#if defined(_WINDOWS)

    return open(hFile, pcFile, accessType, shareMode,
        creationDisposition, flags, CreateFileA);

#else // !defined(_WINDOWS)

    UNUSED(fileShareMode);

    {
        const char *mode = nullptr;
        const int bExist = sl_test_file_c(pcFile, fileAccessType);
        FILE *hFile;

        // set the access mode
        switch (accessType)
        {
        case eAccess::Read:
            mode = "rb";
            break;

        case eAccess::Write:
            if ((eDisposition::CreateNew == creationDisposition) && (bExist)) {
                return eResult::ERR_FAILED;
            }
            mode = "wb";
            break;

        case eAccess::Read | eAccess::Write:
            if (eDisposition::CreateNew == creationDisposition) {
                if (bExist) {
                    return eResult::ERR_FAILED;
                }
                mode = "w+b";
            } else if (eDisposition::CreateAlways == creationDisposition) {
                mode = "w+b";
            } else if (eDisposition::OpenExisting == creationDisposition) {
                if (!bExist) {
                    return eResult::ERR_FAILED;
                }
                mode = "r+b";
            } else if (eDisposition::OpenAlways == creationDisposition) {
                mode = (bExist) ? ("r+b") : ("w+b");
            }
            break;

        default:
            break;
        }

        // mode is not set
        if (nullptr == mode) {
            return eResult::ERR_FAILED;
        }

        // open the file
        hFile = fopen(pcFile, mode);
        if (nullptr == hFile) {
            return eResult::ERR_BAD_PATH;
        }
        // disable buffering if any
        if (eFlag::NoBuffering & flags) {
            setbuf(hFile, nullptr);
        }
        *phFile = (handle) hFile;
    }

    return eResult::OK;

#endif // defined(_WINDOWS)

} // eResult open(handle &hFile, const char *pcFile,

eResult initialize_overlapped_control(OVRLPD *(&pOverlapped))
{
#if defined(_WINDOWS)

    // reset the structure
    std::unique_ptr<OVERLAPPED> p(new (std::nothrow) OVERLAPPED());
    if (nullptr == p) {
        return eResult::ERR_ALLOC;
    }
    memset(p.get(), 0, sizeof(OVERLAPPED));

    // initialize members of the overlapped structure
    p->hEvent = CreateEventA(nullptr, FALSE, FALSE, nullptr);
    if (nullptr == p->hEvent) {
        return eResult::ERR_ALLOC;
    }
    ((OVERLAPPED *(&)) pOverlapped) = p.release();

    return eResult::OK;

#else // !defined(_WINDOWS)

    UNUSED(pOverlapped);

    return eResult::ERR_NOT_IMPL;

#endif // defined(_WINDOWS)

} // eResult initialize_overlapped_control(OVRLPD *(&pOverlapped))

eResult read(handle hFile, void *pBuffer,
    const size_t bytesToRead, size_t *pReadBytes,
    OVRLPD * const pOverlapped)
{
    // check error(s)
    if (nullptr == pBuffer) {
        return eResult::ERR_NULLPTR;
    }
    if (UINT_MAX < bytesToRead) {
        return eResult::ERR_BAD_SIZE;
    }

#if defined(_WINDOWS)

    DWORD nWritten;
    BOOL bRes;

    // start reading operation
    bRes = ReadFile((HANDLE) hFile,
        (LPVOID) pBuffer, (DWORD) bytesToRead,
        &nWritten, (LPOVERLAPPED) pOverlapped);
    // check return status
    if (FALSE == bRes) {
        return eResult::ERR_FAILED;
    }
    // it was a regular reading
    if (nullptr == pOverlapped) {
        // return number of read bytes
        if (pReadBytes) {
            *pReadBytes = nWritten;
        }
        if (0 == nWritten) {
            return eResult::ERR_END_OF_STREAM;
        }
    } else {
        // it was an overlapped reading

        // check reading state
        switch (GetLastError())
        {
            // everything is just fine, reading was started
        case ERROR_IO_PENDING:
            break;

        default:
            return eResult::ERR_FAILED;
        }
    }

#else // !defined(_WINDOWS)

    UNUSED(pOverlapped);

    {
        const size_t written = fread(pBuffer, 1, bytesToRead, (FILE *) hFile);
        const int res = ferror((FILE *) hFile);
        if (res) {
            return eResult::ERR_FAILED;
        }

        // return number of read bytes
        if (pReadBytes) {
            *pReadBytes = written;
        }
        if (0 == written) {
            return eResult::ERR_END_OF_STREAM;
        }
    }

#endif // defined(_WINDOWS)

    return eResult::OK;

} // eResult read(handle hFile, void *pBuffer,

eResult write(handle hFile, const void *pBuffer,
    const size_t bytesToWrite, size_t *pWrittenBytes,
    OVRLPD * const pOverlapped)
{
    // check error(s)
    if (nullptr == pBuffer) {
        return eResult::ERR_NULLPTR;
    }
    if (UINT_MAX < bytesToWrite) {
        return eResult::ERR_BAD_SIZE;
    }

#if defined(_WINDOWS)

    DWORD nWritten;
    BOOL bRes;

    // start reading operation
    bRes = WriteFile((HANDLE) hFile,
        (LPCVOID) pBuffer, (DWORD) bytesToWrite,
        &nWritten, (LPOVERLAPPED) pOverlapped);
    // it was a regular reading
    if (nullptr == pOverlapped) {
        // check return status
        if (FALSE == bRes) {
            return eResult::ERR_FAILED;
        }
        // return number of written bytes
        if (pWrittenBytes) {
            *pWrittenBytes = nWritten;
        }
        if (bytesToWrite != (size_t) nWritten) {
            return eResult::ERR_FAILED;
        }
    } else {
        // it was an overlapped reading

        // check reading state
        switch (GetLastError())
        {
            // everything is just fine, reading was started
        case ERROR_IO_PENDING:
            break;

        default:
            return eResult::ERR_FAILED;
        }
    }

#else // !defined(_WINDOWS)

    UNUSED(pOverlapped);

    {
        const size_t written = fwrite(pBuffer, bytesToWrite, 1, (FILE *) hFile);
        const int res = ferror((FILE *) hFile);
        if (res) {
            return eResult::ERR_FAILED;
        }

        // return number of written bytes
        if (pWrittenBytes) {
            *pWrittenBytes = written;
        }
        if (bytesToWrite != (size_t) written) {
            return eResult::ERR_FAILED;
        }
    }

#endif // defined(_WINDOWS)

    return eResult::OK;

} // eResult write(handle hFile, const void *pBuffer,

#if defined(_WINDOWS)
typedef union
{
    // 64-bit position value
    int64_t position;

    struct
    {
        // lower 32-bit of the position
        LONG posLow;
        // higher 32-bit of the position
        LONG posHigh;

    } parts;

} FILE_POSITION;
#endif // defined(_WINDOWS)

eResult set_pointer(handle hFile, const int64_t distanceToMove,
    uint64_t *pNewPosition, const ePosition moveMethod)
{
#if defined(_WINDOWS)

    DWORD dwMoveMethod;
    FILE_POSITION pos;

    // select move method
    switch(moveMethod)
    {
    case ePosition::Begin:
        dwMoveMethod = FILE_BEGIN;
        break;

    case ePosition::Current:
        dwMoveMethod = FILE_CURRENT;
        break;

    default:
        dwMoveMethod = FILE_END;
        break;
    }

    // move the file pointer
    pos.position = distanceToMove;
    pos.parts.posLow = SetFilePointer((HANDLE) hFile, pos.parts.posLow,
        &pos.parts.posHigh, dwMoveMethod);
    if (INVALID_SET_FILE_POINTER == pos.parts.posLow)
    {
        DWORD dwRes;

        dwRes = GetLastError();
        if (ERROR_SUCCESS != dwRes) {
            return eResult::ERR_FAILED;
        }
    }

    // save the new position
    if (pNewPosition) {
        *pNewPosition = pos.position;
    }

#else // !defined(_WINDOWS)

    int dwMoveMethod;
    int res;

    // select move method
    switch(moveMethod)
    {
    case ePosition::Begin:
        dwMoveMethod = SEEK_SET;
        break;

    case ePosition::Current:
        dwMoveMethod = SEEK_CUR;
        break;

    default:
        dwMoveMethod = SEEK_END;
        break;
    }

    // move the file pointer
    res = fseek((FILE *) hFile, distanceToMove, dwMoveMethod);
    if (res) {
        return eResult::ERR_FAILED;
    }

    // save the new position
    if (pNewPosition) {
        *pNewPosition = ftell((FILE *) hFile);
    }

#endif // defined(_WINDOWS)

    return eResult::OK;

} // eResult set_pointer(handle hFile, const int64_t distanceToMove,

eResult wait_io(handle hFile, OVRLPD * const pOverlapped,
    size_t &processedBytes)
{

#if defined(_WINDOWS)

    DWORD nWritten;
    BOOL bRes;

    // get the result of the operation
    bRes = GetOverlappedResult((HANDLE) hFile,
        (LPOVERLAPPED) pOverlapped, &nWritten, TRUE);
    if (FALSE == bRes)
    {
        switch (GetLastError())
        {
            // we have reached end of the file
        case ERROR_HANDLE_EOF:
            break;

        default:
            return eResult::ERR_FAILED;
        }
    }

    // return number of processed bytes
    processedBytes = (size_t) nWritten;

    return eResult::OK;

#else // !defined(_WINDOWS)

    UNUSED(hFile);

    return eResult::ERR_NOT_SUPPORTED;

#endif // defined(_WINDOWS)

} // eResult wait_io(handle hFile, OVRLPD * const pOverlapped,

eResult get_size(handle hFile, uint64_t &fileSize)
{
#if defined(_WINDOWS)

    BOOL bRes;

    // get the size
    bRes = GetFileSizeEx((HANDLE) hFile, (PLARGE_INTEGER) &fileSize);
    if (FALSE == bRes) {
        return eResult::ERR_FAILED;
    }

#else // !defined(_WINDOWS)

    fpos_t curPos;
    int res;

    // get the current position
    res = fgetpos((FILE *) hFile, &curPos);
    if (res) {
        return eResult::ERR_FAILED;
    }

    // rewind the file and query the size
    res = fseek((FILE *) hFile, 0, SEEK_END);
    if (res) {
        return eResult::ERR_FAILED;
    }
    fileSize = ftell((FILE *) hFile);

    // restore the current position
    fsetpos((FILE *) hFile, &curPos);

#endif // defined(_WINDOWS)

    return eResult::OK;

} // eResult get_size(handle hFile, uint64_t &fileSize)

eResult get_size(const wchar_t *pwFile, uint64_t &fileSize)
{
    handle hFile = nullptr;
    eResult res;

    // open the file
    res = open(hFile, pwFile,
        eAccess::Read, eAccess::Read,
        eDisposition::OpenExisting, eFlag::Normal);
    if (eResult::OK != res) {
        return res;
    }

    // query the size
    res = get_size(hFile, fileSize);

    // close the file
    close(hFile);

    return res;

} // eResult get_size(const wchar_t *pwFile, uint64_t &fileSize)

eResult get_size(const char *pcFile, uint64_t &fileSize)
{
    handle hFile = nullptr;
    eResult res;

    // open the file
    res = open(hFile, pcFile,
        eAccess::Read, eAccess::Read,
        eDisposition::OpenExisting, eFlag::Normal);
    if (eResult::OK != res) {
        return res;
    }

    // query the size
    res = get_size(hFile, fileSize);

    // close the file
    close(hFile);

    return res;

} // eResult get_size(const char *pcFile, uint64_t &fileSize)

#if defined(_WINDOWS)

namespace {

enum
{
    FILE_NAME_BUFFER_SIZE = 2048
};

template <typename char_t, typename function0_t, typename function1_t>
eResult get_sector_size(const char_t *pFile, size_t &sectorSize,
    function0_t get_full_path_name, function1_t get_disk_free_space)
{
    // check error(s)
    if (nullptr == pFile) {
        return eResult::ERR_NULLPTR;
    }

    BOOL bRes;
    DWORD sectorsPerCluster, bytesPerSector; 
    DWORD numberOfFreeClusters, totalNumberOfClusters;
    char_t cBuf[FILE_NAME_BUFFER_SIZE];

    const size_t path_len = get_full_path_name(pFile,
        FILE_NAME_BUFFER_SIZE - 1, cBuf, (char_t **) 0);
    if (0 == path_len) {
        return eResult::ERR_FAILED;
    }

    // found a share name
    if ('\\' == cBuf[0]) {
        size_t i = 0;
        // skip machine name
        while ((path_len > i) && ('\\' == cBuf[i])) {i += 1;}
        while ((path_len > i) && ('\\' != cBuf[i]) && (cBuf[i])) {i += 1;}

        // skip share name
        while ((path_len > i) && ('\\' == cBuf[i]) && (cBuf[i])) {i += 1;}
        while ((path_len > i) && ('\\' != cBuf[i]) && (cBuf[i])) {i += 1;}
        if (path_len > i) {
            cBuf[i + 1] = 0;
        }
    } else {
        // found a drive name
        cBuf[3] = 0;
    }

    // get cluster size
    bRes = get_disk_free_space(cBuf,
        &sectorsPerCluster, &bytesPerSector,
        &numberOfFreeClusters, &totalNumberOfClusters);
    if (FALSE == bRes) {
        return eResult::ERR_FAILED;
    }

    // save the value
    sectorSize = bytesPerSector;

    return eResult::OK;

} // eResult get_sector_size(const char_t *pFile, size_t &sectorSize,

} // namespace

#endif // defined(_WINDOWS)

eResult get_sector_size(const wchar_t *pwFile, size_t &sectorSize)
{
    eResult res = eResult::OK;

    // check error(s)
    if (nullptr == pwFile) {
        return eResult::ERR_NULLPTR;
    }

#if defined(_WINDOWS)

    res = get_sector_size(pwFile, sectorSize,
        GetFullPathNameW, GetDiskFreeSpaceW);

#else // !defined(_WINDOWS)

    sectorSize = 4096;

#endif // defined(_WINDOWS)

    return res;

} // eResult get_sector_size(const wchar_t *pwFile, size_t &sectorSize)

eResult get_sector_size(const char *pcFile, size_t &sectorSize)
{
    eResult res = eResult::OK;

    // check error(s)
    if (nullptr == pcFile) {
        return eResult::ERR_NULLPTR;
    }

#if defined(_WINDOWS)

    res = get_sector_size(pcFile, sectorSize,
        GetFullPathNameA, GetDiskFreeSpaceA);

#else // !defined(_WINDOWS)

    sectorSize = 4096;

#endif // defined(_WINDOWS)

    return res;

} // eResult sector_size(const char *pcFile, size_t sectorSize)

eResult cancel_io(handle hFile)
{
#if defined(_WINDOWS)

    CancelIo((HANDLE) hFile);

    return eResult::OK;

#else // !defined(_WINDOWS)

    UNUSED(hFile);

    return eResult::ERR_NOT_SUPPORTED;

#endif // defined(_WINDOWS)

} // eResult cancel_io(handle hFile)

eResult close_overlapped_control(OVRLPD * const pOverlapped)
{
    // check error(s)
    if (nullptr == pOverlapped) {
        return eResult::ERR_NULLPTR;
    }

#if defined(_WINDOWS)

    OVERLAPPED * const p = (OVERLAPPED *) pOverlapped;

    // close the associated event
    CloseHandle(p->hEvent);
    delete(p);

    return eResult::OK;

#else // !defined(_WINDOWS)

    return eResult::ERR_NOT_SUPPORTED;

#endif // defined(_WINDOWS)

} // eResult close_overlapped_control(OVRLPD * const pOverlapped)

eResult close(handle hFile)
{
#if defined(_WINDOWS)

    CloseHandle((HANDLE) hFile);

#else // !defined(_WINDOWS)

    fclose((FILE *) hFile);

#endif // defined(_WINDOWS)

    return eResult::OK;

} // eResult close(handle hFile)

} // namespace file
} // namespace dimanche
