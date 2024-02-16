#pragma once

#include "glass/glass.h"

namespace glass::gfx {
    union TextureHandle {
        TextureHandle(ResourceID id) : Id(static_cast<uint64_t>(id)) {}
        
        uint64_t Id;
        struct {
            ETextureType Type;
            EPixelFormat Format;
            uint32_t TextureID;
        };
    };

    inline uint32_t getTextureID(ResourceID id) {
        return TextureHandle(id).TextureID;
    }

    inline ETextureType getTextureType(ResourceID id) {
        return TextureHandle(id).Type;
    }

    inline EPixelFormat getTexturePixelFormat(ResourceID id) {
        return TextureHandle(id).Format;
    }
}