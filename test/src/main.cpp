#include "glass/glass.h"
#include "assert.h"
#include "print"

namespace gp = glass::platform;
namespace gfx = glass::gfx;

void onMouseMove(const gp::MouseMoveEvent& event) {
    std::println("Mouse moved: {}, {}", event.X, event.Y);
}

void onKeyPress(const gp::KeyPressEvent& event) {
    std::println("Key pressed: {}", gp::toString(event.KeyCode));
}

void onWindowEvent(const gp::WindowEvent& event) {
    gp::EventDispatcher disp(event);
    disp.dispatch(&onMouseMove);
    disp.dispatch(&onKeyPress);
}

struct Vertex {
    float pos[3];
    float col[3];
};

int main() {
    assert(gp::init());
    gp::WindowSpec spec{};

    spec.Size = { 1280, 720 };
    spec.Title = "Hello, Glass";
    spec.EventCallback = onWindowEvent;

    gp::Window* window = gp::createWindow(spec);
    gfx::Context* context = gfx::createContext({ window });

    gfx::ResourceID vbo{};
    {
        Vertex vertices[] = {
            { { 0.0f, 0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
            { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
            { { 0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
        };

        gfx::BufferInputLayout layout{};
        layout
            .add(gfx::EVT_Float, 3)
            .add(gfx::EVT_Float, 3);

        vbo = gfx::createStaticVertexBuffer(vertices, &layout);
    }

    gfx::ResourceID ibo{};
    {
        uint32_t indices[] = {
            0, 1, 2
        };

        ibo = gfx::createStaticElementBuffer(indices);
    }

    gfx::ProgramSpec programSpec {
        .VertexShader = gfx::getOrCreateShader("shaders/test.vert", gfx::EST_VertexShader),
        .FragmentShader = gfx::getOrCreateShader("shaders/test.frag", gfx::EST_FragmentShader)
    };
    gfx::ShaderProgram* program = gfx::getOrCreateShaderProgram(programSpec);

    while (gp::pollEvents()) {

        gfx::bindVertexBuffer(vbo);
        gfx::bindElementBuffer(ibo);

        gfx::bindShaderProgram(program);
        gfx::drawElements(gfx::EPT_Triangles, 3);
        gfx::present(context);
    }

    gp::shutdown();
}