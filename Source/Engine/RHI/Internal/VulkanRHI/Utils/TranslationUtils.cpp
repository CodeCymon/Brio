#include "TranslationUtils.h"

namespace TranslationUtils {
    VkCullModeFlags CullMode(ECullMode cullMode) {
        switch (cullMode) {
            case ECullMode::None:
                return VK_CULL_MODE_NONE;
            case ECullMode::Front:
                return VK_CULL_MODE_FRONT_BIT;
            case ECullMode::Back:
                return VK_CULL_MODE_BACK_BIT;
        }
        ASSERT(false);
    }

    VkFormat PixelFormat(EPixelFormat format) {
        switch (format) {
            case EPixelFormat::Unknown:
                return VK_FORMAT_UNDEFINED;
            case EPixelFormat::RGBA8_SRGB:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case EPixelFormat::BGRA8_SRGB:
                return VK_FORMAT_B8G8R8A8_SRGB;
            case EPixelFormat::RGB8_SRGB:
                return VK_FORMAT_R8G8B8_SRGB;
            case EPixelFormat::BGR8_SRGB:
                return VK_FORMAT_B8G8R8_SRGB;
        }
        ASSERT(false);
    }
}
