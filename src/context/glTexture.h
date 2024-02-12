#pragma once

namespace glass::gfx {
    union TextureHandle {
        uint64_t Id;
        struct {
            ETextureType Type;
            EPixelFormat Format;
            uint32_t TextureID;
        };
    };
}