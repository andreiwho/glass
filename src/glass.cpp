#include "glass/glass.h"

namespace glass::gfx {

    BufferInputLayout& BufferInputLayout::add(EValueType type, uint32_t componentCount, bool normalize, EBufferDataRate dataRate /*= EBDR_PerVertex*/) {
        m_Elements.push_back({ type, componentCount, normalize, dataRate });
        return *this;
    }
} // namespace glass::gfx