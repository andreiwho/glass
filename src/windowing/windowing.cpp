#include "glass/glass.h"

#include "window.h"

#include "GLFW/glfw3.h"
#include "cassert"

#include "memory"
#include "vector"
#include "bitset"

namespace glass::platform {
    struct PlatformInfo {
        std::vector<std::shared_ptr<Window>> Windows;
        void (*WindowEventCallback)(const WindowEvent&){};
        std::bitset<(size_t)EKeyCode::Menu + 1> KeyStates;
        std::bitset<(size_t)EMouseButton::X2 + 1> MouseButtonStates;
        double MousePosition[2]{};
        double MouseOffset[2]{};

        // @todo(andrey): this is probably incorrect for the first frame
        double DeltaTime{0.02};
        double LastTime{};

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
        static bool isFirstFrame = true;
        const double oldTime = GPlatformInfo->LastTime;

        GPlatformInfo->LastTime = glfwGetTime();
        if (isFirstFrame) {
            isFirstFrame = false;
        } else {
            GPlatformInfo->DeltaTime = oldTime - GPlatformInfo->LastTime;
        }

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

    void internal_onKeyState(EKeyCode key, bool state) {
        GPlatformInfo->KeyStates[static_cast<size_t>(key)] = state;
    }

    void internal_onMouseButtonState(EMouseButton button, bool state) {
        GPlatformInfo->MouseButtonStates[static_cast<size_t>(button)] = state;
    }

    void internal_onMouseMove(double x, double y) {
        static bool isFirstFrame = true;
        double oldMouse[2]{};
        if (isFirstFrame) {
            oldMouse[0] = x;
            oldMouse[1] = y;
            isFirstFrame = false;
        } else {
            oldMouse[0] = GPlatformInfo->MousePosition[0];
            oldMouse[1] = GPlatformInfo->MousePosition[1];
        }

        GPlatformInfo->MousePosition[0] = x;
        GPlatformInfo->MousePosition[1] = y;

        GPlatformInfo->MouseOffset[0] = oldMouse[0] - GPlatformInfo->MousePosition[0];
        GPlatformInfo->MouseOffset[1] = oldMouse[1] - GPlatformInfo->MousePosition[1];
    }

    bool isKeyDown(EKeyCode code) {
        return GPlatformInfo->KeyStates[static_cast<size_t>(code)];
    }

    bool isMouseButtonDown(EMouseButton button) {
        return GPlatformInfo->MouseButtonStates[static_cast<size_t>(button)];
    }

    MouseVec2 getMousePosition() {
        return {
            GPlatformInfo->MousePosition[0],
            GPlatformInfo->MousePosition[1]
        };
    }

    MouseVec2 getMouseOffset() {
        return {
            GPlatformInfo->MouseOffset[0],
            GPlatformInfo->MouseOffset[1]
        };
    }

    double getDeltaTime() {
        return GPlatformInfo->DeltaTime;
    }

    double getTime() {
        return glfwGetTime();
    }
} // namespace glass::platform