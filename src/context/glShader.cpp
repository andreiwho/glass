#include "glass/glass.h"
#include "glad/glad.h"

#include "glShader.h"
#include "glInternal.h"

#include "fstream"
#include "sstream"
#include "print"
#include "cassert"
#include "vector"

namespace glass::gfx {    
    static std::string readShaderSource(const std::string& path) {
        std::ifstream shaderSource {path};
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

            std::println("GLASS: Failed to compile shader: {}", logInfo);
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

    static uint64_t calculateShaderProgramHash(const ProgramSpec& spec) {
        uint64_t specHash = 0x13ee5;
        hashCombine(specHash, spec.VertexShader);
        hashCombine(specHash, spec.FragmentShader);
        hashCombine(specHash, spec.ComputeShader);
        hashCombine(specHash, spec.GeometryShader);
        hashCombine(specHash, spec.TesellationControlShader);
        hashCombine(specHash, spec.TesellationEvaluationShader);
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

            std::println("GLASS: Failed to link program: {}", logInfo);
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

            std::println("GLASS: Failed to validate program: {}", logInfo);
            return nullptr;
        }
        
        std::shared_ptr<ShaderProgram> outProgram = std::make_shared<ShaderProgram>(program);
        GShaderRegistry->Programs[hash] = outProgram;
        return outProgram.get();
    }

    void bindShaderProgram(const ShaderProgram* program) {
        glUseProgram(program->getId());
    }
}