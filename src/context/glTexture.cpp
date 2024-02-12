#include "glass/glass.h"
#include "glTexture.h"
#include "glInternal.h"
#include "cassert"

namespace glass::gfx {
    void initAs1DTexture(uint32_t id, const TextureSpec& spec) {
        glBindTexture(GL_TEXTURE_1D, id);

        if (spec.InitialData) {
            glTexImage1D(GL_TEXTURE_1D, 0, toGLInternalFormat(spec.Format), spec.Width, 0, toGLFormat(spec.Format), GL_UNSIGNED_BYTE, spec.InitialData);
        } else {
            glTexStorage1D(GL_TEXTURE_1D, 1, toGLInternalFormat(spec.Format), spec.Width);
        }

        glBindTexture(GL_TEXTURE_1D, 0);
    }

    void initAs2DTexture(uint32_t id, const TextureSpec& spec) {
        glBindTexture(GL_TEXTURE_2D, id);

        if (spec.InitialData) {
            glTexImage2D(GL_TEXTURE_2D, 0, toGLInternalFormat(spec.Format), spec.Width, spec.Height, 0, toGLFormat(spec.Format), GL_UNSIGNED_BYTE, spec.InitialData);
        } else {
            glTexStorage2D(GL_TEXTURE_2D, 1, toGLInternalFormat(spec.Format), spec.Width, spec.Height);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void initAs3DTexture(uint32_t id, const TextureSpec& spec) {
        glBindTexture(GL_TEXTURE_3D, id);

        if (spec.InitialData) {
            glTexImage3D(GL_TEXTURE_3D, 0, toGLInternalFormat(spec.Format), spec.Width, spec.Height, spec.Depth, 0, toGLFormat(spec.Format), GL_UNSIGNED_BYTE, spec.InitialData);
        } else {
            glTexStorage3D(GL_TEXTURE_3D, 1, toGLInternalFormat(spec.Format), spec.Width, spec.Height, spec.Depth);
        }

        glBindTexture(GL_TEXTURE_3D, 0);
    }

    //void initAsCubeTexture(uint32_t id, const TextureSpec& spec) {

    //}

    ResourceID createTexture(const TextureSpec& spec) {
        TextureHandle outHandle{};
        outHandle.Id = 0;
        outHandle.Type = spec.Type;
        outHandle.Format = spec.Format;

        glGenTextures(1, &outHandle.TextureID);
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
                //initAsCubeTexture(outHandle.TextureID, spec);
                assert(false && "Cube textures are currently unsupported");
                return 0;
                break;
        }

        glBindTexture(textureType, outHandle.TextureID);
        glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, toGLFilter(spec.Sampler.MinFilter));
        glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, toGLFilter(spec.Sampler.MagFilter));
        glTexParameteri(textureType, GL_TEXTURE_WRAP_S, toGLWrapMode(spec.Sampler.WrapModeS));

        if (spec.Type > ETT_Texture1D) {
            glTexParameteri(textureType, GL_TEXTURE_WRAP_T, toGLWrapMode(spec.Sampler.WrapModeT));
        }

        if (spec.Type == ETT_Texture3D) {
            glTexParameteri(textureType, GL_TEXTURE_WRAP_R, toGLWrapMode(spec.Sampler.WrapModeU));
        }
        
        if (spec.GenerateMipmaps) {
            glGenerateMipmap(textureType);
        }
        glBindTexture(textureType, 0);

        return outHandle.Id;
    }
}