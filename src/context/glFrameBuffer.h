#pragma once

#include "glass/glass.h"

namespace glass::gfx {
    class FrameBuffer {
    public:
        FrameBuffer(const FrameBufferSpec& spec);
        ~FrameBuffer();

        inline uint32_t getWidth() const { return m_Spec.Width; }
        inline uint32_t getHeight() const { return m_Spec.Height; }
        inline EPixelFormat getColorAttachmentPixelFormat(uint8_t attachmentId) const { return m_Spec.ColorAttachmentFormats[attachmentId]; }
        inline EPixelFormat getDepthStencilPixelFormat() const { return m_Spec.DepthAttachmentFormat; }

        inline ResourceID getColorAttachmentTexture(uint8_t attachmentId) const {
            return m_ColorAttachments.at(attachmentId);
        }

        inline ResourceID getDepthStencilTexture() const { return m_DepthStencilTexture; }

        inline bool isValid() const { return m_IsValid; }

        inline uint32_t getId() const { return m_Id; }

    private:
        uint32_t m_Id{};
        FrameBufferSpec m_Spec{};
        bool m_HasColorAttachments{};
        bool m_HasDepthStencil{};
        bool m_IsValid{};
        std::vector<ResourceID> m_ColorAttachments{};
        ResourceID m_DepthStencilTexture{ResourceID::Null};
    };

    void freeFramebufferRegistry();
} // namespace glass::gfx