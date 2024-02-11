#include "glass/glass.h"

#include "window.h"

#include "GLFW/glfw3.h"
#include "cassert"

#include "memory"
#include "vector"

namespace glass::platform {
    struct PlatformInfo {
        std::vector<std::shared_ptr<Window>> Windows;
        void (*WindowEventCallback)(const WindowEvent&){};

        void addWindow(std::shared_ptr<Window> window) {
            Windows.push_back(window);
        }

        void removeWindow(const Window* window) {
            auto iter = std::ranges::find_if(Windows, [window](const std::shared_ptr<Window>& win) { return win.get() == window; });
            if (iter != Windows.end()) {
                Windows.erase(iter);
            }
        }
    };

    static PlatformInfo* GPlatformInfo;

    bool init() {
        GPlatformInfo = new PlatformInfo;
        int status = glfwInit();
        return status != 0;
    }

    void shutdown() {
        delete GPlatformInfo;

        glfwTerminate();
    }

    bool pollEvents() {
        glfwPollEvents();

        if (!isAnyWindowAlive()) {
            return false;
        }

        return true;
    }

    Window* createWindow(const WindowSpec& spec) {
        bool isMain = GPlatformInfo->Windows.empty();
        auto window = std::make_shared<Window>(spec, isMain);
        GPlatformInfo->addWindow(window);
        return window.get();
    }

    void destroyWindow(const Window* window) {
        GPlatformInfo->removeWindow(window);
    }

    void showWindow(const Window* window) {
        if (window) {
            window->show();
        }
    }

    void hideWindow(const Window* window) {
        if (window) {
            window->hide();
        }
    }

    WindowSize getWindowSize(const Window* window) {
        if (window) {
            return window->getSize();
        }

        return { -1, -1 };
    }

    bool isMainWindowAlive() {
        return !GPlatformInfo->Windows.empty() && GPlatformInfo->Windows.at(0)->isMain();
    }

    bool isAnyWindowAlive() {
        return !GPlatformInfo->Windows.empty();
    }

    void setWindowEventCallback(Window* window, GLASS_PFN_WindowEventCallback callback) {
        window->setEventCallback(callback);
    }
} // namespace glass::platform