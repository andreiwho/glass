#include "glass/glass.h"
#include "assert.h"

namespace gp = glass::platform;

int main() {
    assert(gp::init());

    gp::Window* window = gp::createWindow({ 1280, 720, "Hello GLass", nullptr });
    while (gp::isMainWindowAlive()) {
        gp::pollEvents();
    }

    glass::platform::shutdown();
}