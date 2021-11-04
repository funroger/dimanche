
#include <dimanche/basic/container_type.h>

namespace dimanche {
namespace container {

eType GetTypeByPath(std::wstring && path)
{
    eType type = eType::UNK;
    if (path.empty()) {return type;}

    // get the file extension
    const auto dotPos = path.find('.');
    if (std::wstring::npos == dotPos) {return type;}

    const std::wstring ext = path.substr(dotPos);

    // analyze the extension
    if (ext == std::wstring(L".avi")) {
        type = eType::AVI;
    } else if (ext == std::wstring(L".mov")) {
        type = eType::MOV;
    } else if (ext == std::wstring(L".mp4")) {
        type = eType::MP4;
    } else if (ext == std::wstring(L".y4m")) {
        type = eType::Y4M;
    } else if (ext == std::wstring(L".yuv")) {
        type = eType::YUV;
    } else if (ext == std::wstring(L".wav")) {
        type = eType::WAV;
    } else if (ext == std::wstring(L".ivf")) {
        type = eType::IVF;
    }

    return type;

} // eType GetTypeByPath(std::wstring && path)

} // namespace container
} // namespace dimanche
