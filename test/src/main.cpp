#include "glass/glass.h"
#include "assert.h"
#include "print"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace gp = glass::platform;
namespace gfx = glass::gfx;

void onMouseMove(const gp::MouseMoveEvent& event) {
    std::println("Mouse moved: {}, {}", event.X, event.Y);
}

void onKeyPress(const gp::KeyPressEvent& event) {
    std::println("Key pressed: {}", gp::toString(event.KeyCode));
}

void onWindowEvent(const gp::WindowEvent& event) {
    std::println("{}", event.toString());
}

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
};

int main() {
    assert(gp::init());
    gp::WindowSpec spec{};

    spec.Size = { 1280, 720 };
    spec.Title = "Hello, Glass";
    spec.EventCallback = onWindowEvent;
    spec.Resizable = true;

    gp::Window* window = gp::createWindow(spec);
    gfx::Context* context = gfx::createContext({ window });

    gfx::ResourceID vbo{};
    {
        Vertex vertices[] = {
            { { -0.5f, +0.5f, 0.0f }, { 0.0f, 1.0f } },
            { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } },
            { { +0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } },
            { { +0.5f, +0.5f, 0.0f }, { 1.0f, 1.0f } }
        };

        gfx::BufferInputLayout layout{};
        layout
            .add(gfx::EVT_Float, 3)
            .add(gfx::EVT_Float, 2);

        vbo = gfx::createStaticVertexBuffer(vertices, &layout);
    }

    gfx::ResourceID ibo{};
    {
        uint32_t indices[] = {
            0, 1, 3,
            1, 2, 3
        };

        ibo = gfx::createStaticElementBuffer(indices);
    }

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

        gfx::bindVertexBuffer(vbo);
        gfx::bindElementBuffer(ibo);

        gfx::bindShaderProgram(program);
        gfx::setUniformTexture(program, "uTexture", texture);

        gfx::drawElements(gfx::EPT_Triangles, 6);
        gfx::present(context);
    }

    gp::shutdown();
}