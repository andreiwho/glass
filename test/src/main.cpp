#include "glass/glass.h"
#include "assert.h"
#include "print"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace gp = glass::platform;
namespace gfx = glass::gfx;

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
};

int main() {
    assert(gp::init());
    gp::WindowSpec spec{};

    spec.Size = { 1280, 720 };
    spec.Title = "Hello, Glass";
    spec.Resizable = true;

    gp::Window* window = gp::createWindow(spec);
    gfx::Context* context = gfx::createContext({ window });

    gfx::ResourceID vbo{};
    {
        Vertex vertices[] = {
            // Front
            { { -0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f } },
            { { -0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f } },
            { { +0.5f, -0.5f, +0.5f }, { 1.0f, 0.0f } },
            { { +0.5f, +0.5f, +0.5f }, { 1.0f, 1.0f } },

            // Left
            { { -0.5f, +0.5f, -0.5f }, { 0.0f, 1.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
            { { -0.5f, -0.5f, +0.5f }, { 1.0f, 0.0f } },
            { { -0.5f, +0.5f, +0.5f }, { 1.0f, 1.0f } },

            // Right
            { { +0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f } },
            { { +0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f } },
            { { +0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
            { { +0.5f, +0.5f, -0.5f }, { 1.0f, 1.0f } },

            // Back
            { { +0.5f, +0.5f, -0.5f }, { 0.0f, 1.0f } },
            { { +0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
            { { -0.5f, +0.5f, -0.5f }, { 1.0f, 1.0f } },

            // Top
            { { -0.5f, +0.5f, -0.5f }, { 0.0f, 1.0f } },
            { { -0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f } },
            { { +0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f } },
            { { +0.5f, +0.5f, -0.5f }, { 1.0f, 1.0f } },

            // Bottom
            { { -0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
            { { +0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
            { { +0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f } },
        };

        gfx::BufferInputLayout layout{};
        layout
            .add(gfx::EVT_Float, 3)
            .add(gfx::EVT_Float, 2);

        vbo = gfx::createStaticVertexBuffer(vertices, &layout);
    }

    // clang-format off
    uint32_t indices[] {
        // Front
        0, 1, 3, 1, 2, 3,

        // Left
        4, 5, 7, 5, 6, 7,

        // Right
        8, 9, 11, 9, 10, 11,

        // Back
        12 ,13, 15, 13, 14, 15,

        // Top
        16, 17, 19, 17, 18, 19,

        // Bottom
        20, 21, 23, 21, 22, 23
    };
    // clang-format on

    gfx::ResourceID ibo = gfx::createStaticElementBuffer(indices);

    gfx::ProgramSpec programSpec{
        .VertexShader = gfx::getOrCreateShader("shaders/test.vert", gfx::EST_VertexShader),
        .FragmentShader = gfx::getOrCreateShader("shaders/test.frag", gfx::EST_FragmentShader)
    };
    gfx::ShaderProgram* program = gfx::getOrCreateShaderProgram(programSpec);

    int w, h, bpp;
    const stbi_uc* texData = stbi_load("res/container.jpg", &w, &h, &bpp, 4);

    gfx::TextureSpec textureSpec{};
    textureSpec.Width = w;
    textureSpec.Height = h;
    textureSpec.Type = gfx::ETT_Texture2D;
    textureSpec.Format = gfx::EPF_RGBA8;
    textureSpec.InitialData = texData;

    gfx::ResourceID texture = gfx::createTexture(textureSpec);

    while (gp::pollEvents()) {
        glm::vec4 clearColor{ 0.2f, 0.31f, 0.12f, 1.0f };
        gfx::clearViewport(gfx::ECF_All, &clearColor);
        gfx::setViewport();

        gfx::bindVertexBuffer(vbo);
        gfx::bindElementBuffer(ibo);

        gfx::bindShaderProgram(program);
        gfx::setUniformTexture(program, "uTexture", texture);

        // Set projection matrix
        const gp::WindowSize size = gp::getWindowSize(window);
        const glm::mat4 projection = glm::perspectiveFov(glm::radians(45.0f), (float)size.Width, (float)size.Height, 0.1f, 100.0f);

        const glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)gp::getTime(), glm::vec3(1.0f, 1.0f, 1.0f));

        gfx::setUniform(program, "uViewProjection", projection * view);
        gfx::setUniform(program, "uModel", model);

        gfx::enableDepthTest();
        gfx::setFillMode(gfx::EFM_Solid);
        gfx::setCullMode(gfx::ECM_Back);

        gfx::drawElements(gfx::EPT_Triangles, (uint32_t)std::size(indices));
        gfx::present(context);
    }

    gp::shutdown();
}