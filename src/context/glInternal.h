#pragma once

#include "glass/glass.h"

#include "glad/glad.h"
#include "type_traits"
#include "hashHelpers.h"

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

    static constexpr GLenum toGLTextureType(ETextureType type) {
        switch (type) {
            case ETT_Texture1D:
                return GL_TEXTURE_1D;
                break;
            case ETT_Texture2D:
                return GL_TEXTURE_2D;
                break;
            case ETT_Texture3D:
                return GL_TEXTURE_3D;
                break;
            case ETT_TextureCube:
                return GL_TEXTURE_CUBE_MAP;
                break;
        }

        return 0;
    }

    static constexpr GLenum toGLFormat(EPixelFormat format) {
        switch (format) {
            case EPF_RGB8:
                return GL_RGB;
            case EPF_RGBA8:
                return GL_RGBA;
            case EPF_R11G11B10F:
                return GL_RGB;
            case EPF_DepthStencil:
                return GL_DEPTH_STENCIL;
            default:
                break;
        }

        return 0;
    }

    static constexpr GLenum toGLInternalFormat(EPixelFormat format) {
        switch (format) {
            case EPF_RGB8:
                return GL_RGB;
            case EPF_RGBA8:
                return GL_RGBA8;
            case EPF_R11G11B10F:
                return GL_R11F_G11F_B10F;
            case EPF_DepthStencil:
                return GL_DEPTH24_STENCIL8;
        }

        return 0;
    }

    static constexpr GLenum toGLWrapMode(ETextureWrapMode wrapMode) {
        switch (wrapMode) {
            case ETWM_Repeat:
                return GL_REPEAT;
                break;
            case ETWM_MirroredRepeat:
                return GL_MIRRORED_REPEAT;
                break;
            case ETWM_ClampToEdge:
                return GL_CLAMP_TO_EDGE;
                break;
            case ETWM_ClampToBorder:
                return GL_CLAMP_TO_BORDER;
                break;
        }
        return 0;
    }

    static constexpr GLenum toGLFilter(ETextureFilter filter) {
        switch (filter) {
            case ETF_Linear:
                return GL_LINEAR;
            case ETF_Nearest:
                return GL_NEAREST;            
        }
        return 0;
    }
} // namespace glass::gfx