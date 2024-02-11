#include "glass/glass.h"
#include "glad/glad.h"

#include "glShader.h"

#include "fstream"
#include "sstream"
#include "cassert"

namespace glass::gfx {
    static std::string readShaderSource(const std::string& path) {
        std::ifstream shaderSource {path};
        assert(shaderSource.is_open());

        std::stringstream source;
        source << shaderSource.rdbuf();
        return source.str();
    }

    Shader* createShader(const std::string& path, EShaderType type) {
        std::string source = readShaderSource(path);

        // TODO:
        return nullptr;
    }
}