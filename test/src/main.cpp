#include "glass/glass.h"
#include "assert.h"
#include "print"

namespace gp = glass::platform;

void onWindowEvent(const gp::WindowEvent& event) {
    std::println("{}", event.toString());
}

int main() {
    assert(gp::init());

    gp::Window* window = gp::createWindow({ { 1280, 720 }, "Hello Glass", nullptr, onWindowEvent });
    while (gp::isMainWindowAlive()) {
        gp::pollEvents();
    }

    glass::platform::shutdown();
}