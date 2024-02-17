#include "glBuffer.h"

#include "glad/glad.h"
#include "glInternal.h"

#include "cassert"

namespace glass::gfx {
    static uint16_t initAsVertexArray(const BufferSpec& spec, uint32_t bufferID) {
        uint32_t outID{};
        glGenVertexArrays(1, &outID);
        glBindVertexArray(outID);
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);

        if (spec.InputLayout && spec.StrideInBytes) {
            uint32_t attribID{};
            uint64_t totalOffset = 0;
            for (auto& elem : spec.InputLayout->getElements()) {
                glEnableVertexAttribArray(attribID);
                switch (elem.Type) {
                    case EVT_Float: {
                        glVertexAttribPointer(attribID, static_cast<GLint>(elem.Count), GL_FLOAT, elem.Normalize, static_cast<GLsizei>(spec.StrideInBytes), (const void*)totalOffset);
                    } break;
                    case EVT_Int: {
                        glVertexAttribIPointer(attribID, static_cast<GLint>(elem.Count), GL_INT, static_cast<GLsizei>(spec.StrideInBytes), (const void*)totalOffset);
                    } break;
                    case EVT_UInt: {
                        glVertexAttribIPointer(attribID, static_cast<GLint>(elem.Count), GL_UNSIGNED_INT, static_cast<GLsizei>(spec.StrideInBytes), (const void*)totalOffset);
                    } break;
                    default:
                        break;
                }
                attribID++;
                totalOffset += sizeof(float) * elem.Count;
            }
        } else {
            assert(false && "Did you forget to assign input layout or stride for vertex buffer?");
            return 0;
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return static_cast<uint16_t>(outID);
    }

    ResourceID createBuffer(const BufferSpec& spec) {
        BufferHandle handle{ ResourceID::Null };
        handle.BufferType = spec.BufferType;

        uint32_t bufferID{};
        glGenBuffers(1, &bufferID);
        handle.BufferID = bufferID;

        const auto bufferType = toGLBufferType(spec.BufferType);
        glBindBuffer(bufferType, handle.BufferID);
        if (spec.InitialData) {
            assert(spec.InitialDataSize > 0);
            glBufferData(bufferType, static_cast<GLsizeiptr>(spec.InitialDataSize), spec.InitialData, toGLBufferUsage(spec.Usage, spec.Mutability));
        }

        glBindBuffer(bufferType, 0);

        if (spec.BufferType == EBT_Vertex) {
            handle.VAOID = initAsVertexArray(spec, handle.BufferID);
        }

        return static_cast<ResourceID>(handle.ID);
    }

    void writeBufferData(ResourceID buffer, const void* data, uint64_t dataSize, uint64_t offset) {
        BufferHandle handle{buffer};
        const GLenum bufferType = toGLBufferType(handle.BufferType);

        glBindBuffer(bufferType, handle.BufferID);
        glBufferSubData(bufferType, static_cast<GLintptr>(offset), static_cast<GLintptr>(dataSize), data);
        glBindBuffer(bufferType, 0);
    }

    void destroyBuffer(ResourceID buffer) {
        BufferHandle handle{buffer};

        if (handle.BufferType == EBT_Vertex) {
            uint32_t vao = handle.VAOID;
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &handle.BufferID);
        } else {
            glDeleteBuffers(1, &handle.BufferID);
        }
    }

    void bindVertexBuffer(ResourceID buffer) {
        BufferHandle handle{buffer};
        glBindVertexArray(handle.VAOID);
    }

    void bindElementBuffer(ResourceID buffer) {
        BufferHandle handle{buffer};
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle.BufferID);
    }

    uint32_t getOpenGLBufferID(ResourceID buffer) {
        return getBufferID(buffer);
    }
} // namespace glass::gfx