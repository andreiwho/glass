#include "glass/glass.h"
#include "glTexture.h"
#include "glInternal.h"
#include "cassert"

namespace glass::gfx {
    void initAs1DTexture(uint32_t id, const TextureSpec& spec) {
        GLCALL(glBindTexture(GL_TEXTURE_1D, id));
        GLCALL(glTexImage1D(GL_TEXTURE_1D, 0, toGLInternalFormat(spec.Format), spec.Width, 0, toGLFormat(spec.Format), toGLDataTypeFromFormat(spec.Format), spec.InitialData));
        GLCALL(glBindTexture(GL_TEXTURE_1D, 0));
    }

    void initAs2DTexture(uint32_t id, const TextureSpec& spec) {
        GLCALL(glBindTexture(GL_TEXTURE_2D, id));
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, toGLInternalFormat(spec.Format), spec.Width, spec.Height, 0, toGLFormat(spec.Format), toGLDataTypeFromFormat(spec.Format), spec.InitialData));
        GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void initAs3DTexture(uint32_t id, const TextureSpec& spec) {
        GLCALL(glBindTexture(GL_TEXTURE_3D, id));
        GLCALL(glTexImage3D(GL_TEXTURE_3D, 0, toGLInternalFormat(spec.Format), spec.Width, spec.Height, spec.Depth, 0, toGLFormat(spec.Format), toGLDataTypeFromFormat(spec.Format), spec.InitialData));
        GLCALL(glBindTexture(GL_TEXTURE_3D, 0));
    }

    // void initAsCubeTexture(uint32_t id, const TextureSpec& spec) {

    //}

    ResourceID createTexture(const TextureSpec& spec) {
        TextureHandle outHandle{ ResourceID::Null };
        outHandle.Id = 0;
        outHandle.Type = spec.Type;
        outHandle.Format = spec.Format;

        GLCALL(glGenTextures(1, &outHandle.TextureID));
        const GLenum textureType = toGLTextureType(spec.Type);

        switch (spec.Type) {
            case ETT_Texture1D:
                initAs1DTexture(outHandle.TextureID, spec);
                break;
            case ETT_Texture2D:
                initAs2DTexture(outHandle.TextureID, spec);
                break;
            case ETT_Texture3D:
                initAs3DTexture(outHandle.TextureID, spec);
                break;
            case ETT_TextureCube:
                // initAsCubeTexture(outHandle.TextureID, spec);
                assert(false && "Cube textures are currently unsupported");
                return ResourceID::Null;
                break;
        }

        GLCALL(glBindTexture(textureType, outHandle.TextureID));
        GLCALL(glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, toGLFilter(spec.Sampler.MinFilter)));
        GLCALL(glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, toGLFilter(spec.Sampler.MagFilter)));
        GLCALL(glTexParameteri(textureType, GL_TEXTURE_WRAP_S, toGLWrapMode(spec.Sampler.WrapModeS)));

        if (spec.Type > ETT_Texture1D) {
            GLCALL(glTexParameteri(textureType, GL_TEXTURE_WRAP_T, toGLWrapMode(spec.Sampler.WrapModeT)));
        }

        if (spec.Type == ETT_Texture3D) {
            GLCALL(glTexParameteri(textureType, GL_TEXTURE_WRAP_R, toGLWrapMode(spec.Sampler.WrapModeU)));
        }

        if (spec.GenerateMipmaps) {
            GLCALL(glGenerateMipmap(textureType));
        }
        GLCALL(glBindTexture(textureType, 0));

        return static_cast<ResourceID>(outHandle.Id);
    }

    void destroyTexture(ResourceID id) {
        uint32_t texID = getTextureID(id);
        if (texID != 0) {
            GLCALL(glDeleteTextures(1, &texID));
        }
    }

    uint32_t getOpenGLTextureID(ResourceID texture) {
        return getTextureID(texture);
    }
} // namespace glass::gfx