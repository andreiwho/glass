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

int main() {
    assert(gp::init());
    gp::WindowSpec spec{};
    
    spec.Size = { 1280, 720 };
    spec.Title = "Hello, Glass";
    spec.EventCallback = onWindowEvent;

    gp::Window* window = gp::createWindow(spec);
    gfx::Context* context = gfx::createContext({ window });

    while (gp::pollEvents()) {
        gfx::present(context);
    }

    gp::shutdown();
}