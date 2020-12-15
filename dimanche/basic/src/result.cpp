
#include <dimanche/basic/result.h>

namespace dimanche {

namespace {

#define DECL(result, desc) \
    {result, L###result L" - " desc}

const
struct
{
    eResult res;
    const wchar_t * const pDescription;

} ErrorDescriptions[] =
{
    DECL(eResult::ERR_NOT_SUPPORTED, L"Operational mode or feature is not supported."),
    DECL(eResult::ERR_END_OF_STREAM, L"End of media/file stream is reached."),
    DECL(eResult::ERR_BAD_SIZE, L"Provided size value is invalid."),
    DECL(eResult::ERR_ALLOC, L"Can't allocate required memory/resource."),
    DECL(eResult::ERR_BAD_PATH, L"File path is incorrect."),
    DECL(eResult::ERR_NOT_IMPL, L"Some methods or interfaces are not implemented yet."),
    DECL(eResult::ERR_FAILED, L"Unknown error or function call wasn't successfull."),
    DECL(eResult::ERR_EXCEPTION, L"An exception was thrown inside the function."),
    DECL(eResult::ERR_INV_PARAM, L"Parameter value is invalid."),
    DECL(eResult::ERR_BAD_FORMAT, L"The format provided is invalid."),
    DECL(eResult::ERR_NULLPTR, L"One or more parameters is nullptr pointer. Real data is expected."),

    DECL(eResult::OK, L"Everything is just fine"),

    DECL(eResult::WRN_NEED_MORE_DATA, L"Provided data is not enough for encode/decode. More data is required."),
    DECL(eResult::WRN_NOT_READY, L"Processing is in progress."),
    DECL(eResult::WRN_BUSY, L"Resource is busy."),
    DECL(eResult::WRN_UNKNOWN, L"Function call is not complete, or function is not called yet.")
};

#undef DECL

} // namespace

const std::wstring GetErrorDescription(const eResult result)
{
    const wchar_t *pDescription = L"description is not available";

    // try to find the status in description array
    for (auto errorDesc : ErrorDescriptions) {
        if (errorDesc.res == result) {
            pDescription = errorDesc.pDescription;
            break;
        }
    }

    return pDescription;
}

} // namespace dimanche
