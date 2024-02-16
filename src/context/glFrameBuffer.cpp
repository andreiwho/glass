#include "glFrameBuffer.h"

#include "glad/glad.h"
#include "glTexture.h"

#include "print"

namespace glass::gfx {
    static std::vector<std::shared_ptr<FrameBuffer>> GFrameBufferRegistry{};

    FrameBuffer::FrameBuffer(const FrameBufferSpec& spec)
        : m_Spec(spec) {
        glGenFramebuffers(1, &m_Id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);

        GLuint drawBuffers[MAX_COLOR_ATTACHMENTS]{};

        for (int32_t index = 0; index < MAX_COLOR_ATTACHMENTS; ++index) {
            if (spec.ColorAttachmentFormats[index] == EPF_Undefined) {
                break;
            }

            // Create framebuffer textures.
            TextureSpec texSpec{};
            texSpec.Type = ETT_Texture2D;
            texSpec.Width = getWidth();
            texSpec.Height = getHeight();
            texSpec.Format = spec.ColorAttachmentFormats[index];
            texSpec.GenerateMipmaps = false;
            texSpec.Sampler.MinFilter = ETF_Nearest;
            texSpec.Sampler.MagFilter = ETF_Nearest;

            ResourceID attachment = createTexture(texSpec);
            m_ColorAttachments.push_back(attachment);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, getTextureID(attachment), 0);
            drawBuffers[index] = GL_COLOR_ATTACHMENT0 + index;
        }

        if (!m_ColorAttachments.empty()) {
            glDrawBuffers(static_cast<GLsizei>(m_ColorAttachments.size()), drawBuffers);
        }

        // Create depth attachment
        if (spec.DepthAttachmentFormat != EPF_Undefined) {
            TextureSpec dsSpec{};
            dsSpec.Type = ETT_Texture2D;
            dsSpec.Width = getWidth();
            dsSpec.Height = getHeight();
            dsSpec.Format = spec.DepthAttachmentFormat;
            dsSpec.GenerateMipmaps = false;
            dsSpec.Sampler.MinFilter = ETF_Nearest;
            dsSpec.Sampler.MagFilter = ETF_Nearest;
        
            ResourceID dsAttachment = createTexture(dsSpec);
            m_DepthStencilTexture = dsAttachment;
            
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, getTextureID(dsAttachment), 0);
        }

        m_IsValid = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
        if (!m_IsValid) {
            std::println("GLASS error: Tried to create a framebuffer, but it is not complete. Check if it has at least color or depth attachments.");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    FrameBuffer::~FrameBuffer() {
        glDeleteFramebuffers(1, &m_Id);
        
        for (ResourceID texture : m_ColorAttachments) {
            destroyTexture(texture);
        }

        if (m_DepthStencilTexture != ResourceID::Null) {
            destroyTexture(m_DepthStencilTexture);
        }
    }

    FrameBuffer* createFrameBuffer(const FrameBufferSpec& spec) {
        return GFrameBufferRegistry.emplace_back(std::make_shared<FrameBuffer>(spec)).get();
    }

    void destroyFrameBuffer(FrameBuffer* frameBuffer) {
        auto iter = std::ranges::find_if(GFrameBufferRegistry, [frameBuffer](const std::shared_ptr<FrameBuffer>& fb) { return fb.get() == frameBuffer; });
        if (iter != GFrameBufferRegistry.end()) {
            GFrameBufferRegistry.erase(iter);
        }
    }

    void freeFramebufferRegistry() {
        GFrameBufferRegistry.clear();
    }

    ResourceID getFrameBufferColorAttachmentTexture(const FrameBuffer* fb, uint8_t index) {
        if (fb) {
            return fb->getColorAttachmentTexture(index);
        }

        return ResourceID::Null;
    }

    ResourceID getFrameBufferDepthStencilAttachment(const FrameBuffer* fb) {
        if (fb) {
            return fb->getDepthStencilTexture();
        }

        return ResourceID::Null;
    }

} // namespace glass::gfx