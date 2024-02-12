#pragma once

#include "glass/glass.h"

#include "glad/glad.h"
#include "type_traits"

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

    static constexpr GLenum toGLShaderType(EShaderType type) {
        switch (type) {
            case EST_VertexShader:
                return GL_VERTEX_SHADER;
            case EST_FragmentShader:
                return GL_FRAGMENT_SHADER;
#if GLASS_CONTEXT_VERSION_MAJOR >= 4 && GLASS_CONTEXT_VERSION_MINOR >= 3
            case EST_ComputeShader:
                return GL_COMPUTE_SHADER;
            case EST_GeometryShader:
                return GL_GEOMETRY_SHADER;
#endif
            case EST_TesellationControlShader:
                return GL_TESS_CONTROL_SHADER;
            case EST_TesellationEvaluationShader:
                return GL_TESS_EVALUATION_SHADER;
        }

        return 0;
    }

    template <class T>
    inline void hashCombine(uint64_t& seed, const T& v) {
        std::hash<T> Hasher;
        seed ^= Hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
} // namespace glass::gfx