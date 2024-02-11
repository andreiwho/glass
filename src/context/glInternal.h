#pragma once

#include "glass/glass.h"

#include "glad/glad.h"

namespace glass::gfx {
    static constexpr GLenum toGLBufferType(EBufferType type) {
        switch (type) {
            case EBT_Unknown:
                return 0;
            case EBT_Vertex:
                return GL_ARRAY_BUFFER;
            case EBT_Element:
                return GL_ELEMENT_ARRAY_BUFFER;
            case EBT_Uniform:
                return GL_UNIFORM_BUFFER;
            case EBT_Storage:
                return GL_SHADER_STORAGE_BUFFER;
        }

        return 0;
    }

    static constexpr GLenum toGLBufferUsage(EBufferUsage usage, EBufferMutability mutability) {
        switch (usage) {
            case EBU_Draw: {
                switch (mutability) {
                    case EBM_Static:
                        return GL_STATIC_DRAW;
                    case EBM_Dynamic:
                        return GL_DYNAMIC_DRAW;
                    case EBM_Stream:
                        return GL_STREAM_DRAW;
                }
                return 0;
            } break;
            case EBU_Copy: {
                switch (mutability) {
                    case EBM_Static:
                        return GL_STATIC_COPY;
                    case EBM_Dynamic:
                        return GL_DYNAMIC_COPY;
                    case EBM_Stream:
                        return GL_STREAM_COPY;
                }
                return 0;
            } break;
            case EBU_Read: {
                switch (mutability) {
                    case EBM_Static:
                        return GL_STATIC_READ;
                    case EBM_Dynamic:
                        return GL_DYNAMIC_READ;
                    case EBM_Stream:
                        return GL_STREAM_READ;
                }
                return 0;
            } break;
        }
        return 0;
    }
} // namespace glass::gfx