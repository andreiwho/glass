#pragma once

#include "glass/glass.h"

namespace glass::gfx {
    union BufferHandle {
        BufferHandle(ResourceID id)
            : ID(static_cast<uint64_t>(id)) {}

        uint64_t ID;
        struct {
            uint16_t VAOID;
            EBufferType BufferType;
            uint32_t BufferID;
        };
    };

    inline uint32_t getBufferID(ResourceID id) {
        return BufferHandle(id).BufferID;
    }

    inline EBufferType getBufferType(ResourceID id) {
        return BufferHandle(id).BufferType;
    }

    inline uint32_t getVAOId(ResourceID id) {
        return BufferHandle(id).VAOID;
    }

}