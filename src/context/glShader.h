#pragma once

#include "glass/glass.h"

namespace glass::gfx {
    class Shader {
    public:
        Shader(uint32_t shader, EShaderType type);
        ~Shader();

        inline uint32_t getId() const { return m_Id; }

    private:
        uint32_t m_Id{};
        EShaderType m_Type{};
    };

    class ShaderProgram {
    public:
        ShaderProgram(uint32_t id);
        ~ShaderProgram();

        inline uint32_t getId() const { return m_Id; }
        int32_t getUniformLocation(const char* name) const;

    private:
        uint32_t m_Id{};
        mutable std::unordered_map<uint64_t, int32_t> m_UniformLocations{};
    };

    void terminateShaderLibrary();
} // namespace glass::gfx