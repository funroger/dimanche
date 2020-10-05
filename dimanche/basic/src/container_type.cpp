
#include <dimanche/basic/container_type.h>

namespace dimanche {

eContainerType GetContainerTypeByPath(std::wstring && path)
{
    eContainerType type = eContainerType::UNK;
    if (path.empty()) {return type;}

    // get the file extension
    const auto dotPos = path.find('.');
    if (std::wstring::npos == dotPos) {return type;}

    const std::wstring ext = path.substr(dotPos);

    // analyze the extension
    if (ext == std::wstring(L".avi")) {
        type = eContainerType::AVI;
    } else if (ext == std::wstring(L".yuv")) {
        type = eContainerType::YUV;
    } else if (ext == std::wstring(L".y4m")) {
        type = eContainerType::Y4M;
    } else if (ext == std::wstring(L".wav")) {
        type = eContainerType::WAV;
    } else if (ext == std::wstring(L".ivf")) {
        type = eContainerType::IVF;
    }

    return type;

} // eContainerType GetContainerTypeByPath(std::wstring && path)

} // namespace dimanche
