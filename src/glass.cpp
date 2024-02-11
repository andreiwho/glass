#include "glass/glass.h"

namespace glass::gfx {

    BufferInputLayout& BufferInputLayout::add(EValueType type, uint32_t componentCount, uint64_t offset, EBufferDataRate dataRate /*= EBDR_PerVertex*/) {
        m_Elements.push_back({ type, componentCount, offset, dataRate });
        return *this;
    }

} // namespace glass::gfx