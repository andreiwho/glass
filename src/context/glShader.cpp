#include "glass/glass.h"
#include "glad/glad.h"

#include "glShader.h"
#include "glInternal.h"

#include "fstream"
#include "sstream"
#include "iostream"
#include "cassert"
#include "vector"
#include "glTexture.h"
#include "glBuffer.h"

namespace glass::gfx {
    static std::string readShaderSource(const std::string& path) {
        std::ifstream shaderSource{ path };
        assert(shaderSource.is_open());

        std::stringstream source;
        source << shaderSource.rdbuf();
        return source.str();
    }

    struct ShaderRegistry {
        std::unordered_map<std::string, std::shared_ptr<Shader>> Shaders;
        std::unordered_map<uint64_t, std::shared_ptr<ShaderProgram>> Programs;
    };

    static std::unique_ptr<ShaderRegistry> GShaderRegistry = std::make_unique<ShaderRegistry>();

    Shader* getOrCreateShader(const std::string& path, EShaderType type) {
        if (GShaderRegistry->Shaders.contains(path)) {
            return GShaderRegistry->Shaders.at(path).get();
        }

        std::string source = readShaderSource(path);
        const char* csource = source.c_str();

        uint32_t shader = glCreateShader(toGLShaderType(type));
        glShaderSource(shader, 1, &csource, nullptr);
        glCompileShader(shader);

        int status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        if (!status) {
            int len;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
            std::string logInfo;
            logInfo.resize(len);
            glGetShaderInfoLog(shader, len, &len, logInfo.data());

            std::cout << std::format("GLASS: Failed to compile shader: {}", logInfo);
            return nullptr;
        }

        std::shared_ptr<Shader> outShader = std::make_shared<Shader>(shader, type);
        GShaderRegistry->Shaders[path] = outShader;
        return outShader.get();
    }

    Shader::Shader(uint32_t shader, EShaderType type)
        : m_Id(shader)
        , m_Type(type) {
    }

    Shader::~Shader() {
        if (m_Id) {
            glDeleteShader(m_Id);
        }
    }

    void terminateShaderLibrary() {
        GShaderRegistry.reset();
    }

    ShaderProgram::ShaderProgram(uint32_t id)
        : m_Id(id) {
    }

    ShaderProgram::~ShaderProgram() {
        if (m_Id) {
            glDeleteProgram(m_Id);
        }
    }

    int32_t ShaderProgram::getUniformLocation(const char* name) const {
        uint64_t hash = hash::hash64(name, strlen(name));
        const auto iter = m_UniformLocations.find(hash);
        if (iter != m_UniformLocations.end()) {
            return iter->second;
        }

        int32_t location = glGetUniformLocation(m_Id, name);
        if (location == -1) {
            std::cout << std::format("GLASS warning: Failed to find uniform location with name: {}", name);
            return location;
        }

        m_UniformLocations[hash] = location;
        return location;
    }

    int32_t ShaderProgram::getUniformBlockBinding(const char* name) const {
        uint64_t hash = hash::hash64(name, strlen(name));
        if (const auto iter = m_UniformBlockBindings.find(hash); iter != m_UniformBlockBindings.end()) {
            return iter->second;
        }

        const uint32_t index = glGetUniformBlockIndex(m_Id, name);
        if (index != GL_INVALID_INDEX) {
            const auto binding = m_NextBlockBinding++;
            glUniformBlockBinding(m_Id, index, binding);
            m_UniformBlockBindings[hash] = binding;
            return binding;
        } else {
            std::cout << std::format("GLASS warning: Failed to find uniform block index with name: {}", name);
            return -1;
        }
    }

    static uint64_t calculateShaderProgramHash(const ProgramSpec& spec) {
        uint64_t specHash = 0x13ee5;
        hash::hashCombine(specHash, spec.VertexShader);
        hash::hashCombine(specHash, spec.FragmentShader);
        hash::hashCombine(specHash, spec.ComputeShader);
        hash::hashCombine(specHash, spec.GeometryShader);
        hash::hashCombine(specHash, spec.TesellationControlShader);
        hash::hashCombine(specHash, spec.TesellationEvaluationShader);
        return specHash;
    }

    static std::vector<uint32_t> getUniqueShaders(const ProgramSpec& spec) {
        std::vector<uint32_t> out{};
        union {
            ProgramSpec pSpec{};
            Shader* shaders[sizeof(ProgramSpec) / sizeof(Shader*)];
        };

        pSpec = spec;
        for (const Shader* shader : shaders) {
            if (shader) {
                out.push_back(shader->getId());
            }
        }

        return out;
    }

    ShaderProgram* getOrCreateShaderProgram(const ProgramSpec& spec) {
        const auto hash = calculateShaderProgramHash(spec);
        if (GShaderRegistry->Programs.contains(hash)) {
            return GShaderRegistry->Programs.at(hash).get();
        }

        uint32_t program = glCreateProgram();
        auto shaders = getUniqueShaders(spec);
        for (uint32_t shader : shaders) {
            glAttachShader(program, shader);
        }

        glLinkProgram(program);

        int linkStatus;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (!linkStatus) {
            int len;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
            std::string logInfo;
            logInfo.resize(len);
            glGetProgramInfoLog(program, len, &len, logInfo.data());

            std::cout << std::format("GLASS: Failed to link program: {}", logInfo);
            return nullptr;
        }

        int valStatus;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &valStatus);
        if (!valStatus) {
            int len;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
            std::string logInfo;
            logInfo.resize(len);
            glGetProgramInfoLog(program, len, &len, logInfo.data());

            std::cout << std::format("GLASS: Failed to validate program: {}", logInfo);
            return nullptr;
        }

        std::shared_ptr<ShaderProgram> outProgram = std::make_shared<ShaderProgram>(program);
        GShaderRegistry->Programs[hash] = outProgram;
        return outProgram.get();
    }

    void bindShaderProgram(const ShaderProgram* program) {
        glUseProgram(program->getId());
    }

    void setUniform(const ShaderProgram* program, const char* name, float uniform) {
        int32_t location = program->getUniformLocation(name);
        glProgramUniform1f(program->getId(), location, uniform);
    }

    void setUniform(const ShaderProgram* program, const char* name, glm::vec2 uniform) {
        int32_t location = program->getUniformLocation(name);
        glProgramUniform2f(program->getId(), location, uniform.x, uniform.y);
    }

    void setUniform(const ShaderProgram* program, const char* name, glm::vec3 uniform) {
        int32_t location = program->getUniformLocation(name);
        glProgramUniform3f(program->getId(), location, uniform.x, uniform.y, uniform.z);
    }

    void setUniform(const ShaderProgram* program, const char* name, glm::vec4 uniform) {
        int32_t location = program->getUniformLocation(name);
        glProgramUniform4f(program->getId(), location, uniform.x, uniform.y, uniform.z, uniform.w);
    }

    void setUniform(const ShaderProgram* program, const char* name, int32_t uniform) {
        int32_t location = program->getUniformLocation(name);
        glProgramUniform1i(program->getId(), location, uniform);
    }

    void setUniform(const ShaderProgram* program, const char* name, glm::ivec2 uniform) {
        int32_t location = program->getUniformLocation(name);
        glProgramUniform2i(program->getId(), location, uniform.x, uniform.y);
    }

    void setUniform(const ShaderProgram* program, const char* name, glm::ivec3 uniform) {
        int32_t location = program->getUniformLocation(name);
        glProgramUniform3i(program->getId(), location, uniform.x, uniform.y, uniform.z);
    }

    void setUniform(const ShaderProgram* program, const char* name, glm::ivec4 uniform) {
        int32_t location = program->getUniformLocation(name);
        glProgramUniform4i(program->getId(), location, uniform.x, uniform.y, uniform.z, uniform.w);
    }

    void setUniform(const ShaderProgram* program, const char* name, uint32_t uniform) {
        int32_t location = program->getUniformLocation(name);
        glProgramUniform1ui(program->getId(), location, uniform);
    }

    void setUniform(const ShaderProgram* program, const char* name, glm::uvec2 uniform) {
        int32_t location = program->getUniformLocation(name);
        glProgramUniform2ui(program->getId(), location, uniform.x, uniform.y);
    }

    void setUniform(const ShaderProgram* program, const char* name, glm::uvec3 uniform) {
        int32_t location = program->getUniformLocation(name);
        glProgramUniform3ui(program->getId(), location, uniform.x, uniform.y, uniform.z);
    }

    void setUniform(const ShaderProgram* program, const char* name, glm::uvec4 uniform) {
        int32_t location = program->getUniformLocation(name);
        glProgramUniform4ui(program->getId(), location, uniform.x, uniform.y, uniform.z, uniform.w);
    }

    void setUniform(const ShaderProgram* program, const char* name, const glm::mat3& uniform, bool transpose) {
        glProgramUniformMatrix3fv(program->getId(), program->getUniformLocation(name), 1, transpose, glm::value_ptr(uniform));
    }

    void setUniform(const ShaderProgram* program, const char* name, const glm::mat4& uniform, bool transpose) {
        glProgramUniformMatrix4fv(program->getId(), program->getUniformLocation(name), 1, transpose, glm::value_ptr(uniform));
    }

    void setUniformTexture(const ShaderProgram* program, const char* name, ResourceID id, uint32_t slot) {
        const GLenum textureType = toGLTextureType(getTextureType(id));
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(textureType, getTextureID(id));
        setUniform(program, name, static_cast<int32_t>(slot));
    }

    void setUniformBuffer(const ShaderProgram* program, const char* name, ResourceID id, uint32_t optBinding) {
        if (optBinding != INVALID_BINDING) {
            glBindBufferBase(GL_UNIFORM_BUFFER, optBinding, getBufferID(id));
        } else {
            assert(name != nullptr);
            const auto binding = program->getUniformBlockBinding(name);
            if (binding != -1) {
                glBindBufferBase(GL_UNIFORM_BUFFER, binding, getBufferID(id));
            }
        }
    }

} // namespace glass::gfx