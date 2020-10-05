
#include <dimanche/basic/image.h>
#include <dimanche/basic/resource.h>

#include <atomic>
#include <memory>

namespace dimanche {
namespace image {

class Allocator;

} // namespace image

class CImage : public Image
{
public:
    // constructors
    CImage() = default;

    // access data
    virtual
    const image::Format & Format() const override;

    // lock image planes, prepare them for using.
    // return value is self object if everything is OK, otherwise nullptr.
    virtual
    Image * const Lock() override;

    // access C-style format
    virtual
    const IMAGE c_image() const override;

    // unlock the image planes, using has been completed
    virtual
    void Unlock() override;

    // release the image
    virtual
    void Release() override;

private:
    // destructor
    virtual
    ~CImage() = default;

    image::Format m_format;

    std::atomic<int32_t> m_locked = 0;
    std::array<image::PLANE, image::MAX_NUM_PLANES> m_planes = {};

    image::Allocator *m_pAllocator = nullptr;
};

const image::Format & CImage::Format() const {
    return m_format;
}

Image * const CImage::Lock() {
    if (image::eColorFormat::NONE == Format().ColorFormat()) {
        return nullptr;
    }
    ++m_locked;
    return this;
}

const IMAGE CImage::c_image() const {
    IMAGE image;

    if (0 < m_locked) {
        image.format = Format().c_format();
        const auto numPlanes = Format().NumPlanes();
        for (size_t i = 0; i < numPlanes; ++i) {
            image.planes[i] = m_planes[i];
        }
        for (size_t i = numPlanes; i < image::MAX_NUM_PLANES; ++i) {
            image.planes[i] = {};
        }
    } else {
        image = {};
    }

    return image;
}

void CImage::Unlock() {
    if (image::eColorFormat::NONE == Format().ColorFormat()) {
        return;
    }
    --m_locked;
}

void CImage::Release() {
    // return to allocator
}

} // namespace dimanche
