#pragma once

#include "glass/glass.h"
#include "GLFW/glfw3.h"

namespace glass::platform {
    class GLASS_API Window {
    public:
        Window(const WindowSpec& spec, bool isMain = false);
        ~Window();

        void show() const;
        void hide() const;
        
        inline WindowSize getSize() const { return m_Spec.Size; }
        inline bool isMain() const { return m_IsMain; }
        void setEventCallback(GLASS_PFN_WindowEventCallback callback);
        void setCloseCallback();


        void executeCallback(const WindowEvent& event);

    private:
        WindowSpec m_Spec{};
        GLFWwindow* m_Window{};
        bool m_IsMain{};
    };
} // namespace glass::platform