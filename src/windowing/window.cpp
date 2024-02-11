#include "window.h"
#include "cassert"
#include "print"

namespace glass::platform {

    Window::Window(const WindowSpec& spec, bool isMain)
        : m_Spec(spec)
        , m_IsMain(isMain) {
#ifndef NDEBUG
        glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
#endif

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLASS_CONTEXT_VERSION_MAJOR);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLASS_CONTEXT_VERSION_MINOR);
        glfwWindowHint(GLFW_RESIZABLE, spec.Resizable ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, spec.Visible ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_FOCUSED, spec.Focused ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_MAXIMIZED, spec.Maximized ? GLFW_TRUE : GLFW_FALSE);

        m_Window = glfwCreateWindow(
            spec.Size.Width,
            spec.Size.Height,
            spec.Title.c_str(),
            spec.Fullscreen ? glfwGetPrimaryMonitor() : nullptr,
            spec.SharedContext ? spec.SharedContext->m_Window : nullptr);

        if (!m_Window) {
            const char* error;
            glfwGetError(&error);
            std::println("GLASS: Failed to create GLFW window: {}", error);
            assert(false && "Failed to create window. See error message in log.");
        }

        if (spec.EventCallback) {
            setEventCallback(spec.EventCallback);
        } else {
            setCloseCallback();
        }

        glfwSetWindowUserPointer(m_Window, this);
    }

    Window::~Window() {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
        }
    }

    void Window::show() const {
        glfwShowWindow(m_Window);
    }

    void Window::hide() const {
        glfwHideWindow(m_Window);
    }

    static Window* getWindow(GLFWwindow* window) {
        return static_cast<Window*>(glfwGetWindowUserPointer(window));
    }

    void Window::setEventCallback(GLASS_PFN_WindowEventCallback callback) {
        m_Spec.EventCallback = callback;

        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int w, int h) {
            auto myWindow = getWindow(window);
            WindowResizeEvent event{};

            event.EventWindow = myWindow;
            event.NewSize = { w, h };
            event.Type = EWindowEventType::WindowResize;

            myWindow->executeCallback(event);
        });

        glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int x, int y) {
            auto myWindow = getWindow(window);
            WindowMoveEvent event{};

            event.EventWindow = myWindow;
            event.PosX = x;
            event.PosY = y;
            event.Type = EWindowEventType::WindowMove;

            myWindow->executeCallback(event);
        });

        glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximized) {
            auto myWindow = getWindow(window);

            if (maximized) {
                WindowMaximizeEvent event{};
                event.EventWindow = myWindow;
                event.Type = EWindowEventType::WindowMaximize;
                myWindow->executeCallback(event);
            } else {
                WindowRestoreEvent event{};
                event.EventWindow = myWindow;
                event.Type = EWindowEventType::WindowRestore;
                myWindow->executeCallback(event);
            }
        });

        glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* window, int iconified) {
            auto myWindow = getWindow(window);

            if (iconified) {
                WindowMinimizeEvent event{};
                event.EventWindow = myWindow;
                event.Type = EWindowEventType::WindowMinimize;
                myWindow->executeCallback(event);
            } else {
                WindowRestoreEvent event{};
                event.EventWindow = myWindow;
                event.Type = EWindowEventType::WindowRestore;
                myWindow->executeCallback(event);
            }
        });

        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused) {
            auto myWindow = getWindow(window);

            if (focused) {
                WindowFocusEvent event{};
                event.EventWindow = myWindow;
                event.Type = EWindowEventType::WindowFocus;
                myWindow->executeCallback(event);
            } else {
                WindowUnfocusEvent event{};
                event.EventWindow = myWindow;
                event.Type = EWindowEventType::WindowUnfocus;
                myWindow->executeCallback(event);
            }
        });

        glfwSetCursorEnterCallback(m_Window, [](GLFWwindow* window, int entered) {
            auto myWindow = getWindow(window);

            if (entered) {
                MouseEnterWindowEvent event{};
                event.EventWindow = myWindow;
                event.Type = EWindowEventType::MouseEnterWindow;
                myWindow->executeCallback(event);
            } else {
                MouseLeaveWindowEvent event{};
                event.EventWindow = myWindow;
                event.Type = EWindowEventType::MouseLeaveWindow;
                myWindow->executeCallback(event);
            }
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y) {
            auto myWindow = getWindow(window);
            MouseMoveEvent event{};

            event.EventWindow = myWindow;
            event.X = x;
            event.Y = y;
            event.Type = EWindowEventType::MouseMove;

            myWindow->executeCallback(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int) {
            auto myWindow = getWindow(window);
            if (action == GLFW_PRESS) {
                MouseButtonPressEvent event{};
                event.EventWindow = myWindow;
                event.Button = static_cast<EMouseButton>(button);
                event.Type = EWindowEventType::MouseButtonPress;
                myWindow->executeCallback(event);
            } else {
                MouseButtonReleaseEvent event{};
                event.EventWindow = myWindow;
                event.Button = static_cast<EMouseButton>(button);
                event.Type = EWindowEventType::MouseButtonRelease;
                myWindow->executeCallback(event);
            }
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int, int action, int) {
            auto myWindow = getWindow(window);

            switch (action) {
                case GLFW_PRESS: {
                    KeyPressEvent event{};
                    event.EventWindow = myWindow;
                    event.Type = EWindowEventType::KeyPress;
                    event.KeyCode = static_cast<EKeyCode>(key);
                    myWindow->executeCallback(event);
                } break;
                case GLFW_RELEASE: {
                    KeyReleaseEvent event{};
                    event.EventWindow = myWindow;
                    event.Type = EWindowEventType::KeyRelease;
                    event.KeyCode = static_cast<EKeyCode>(key);
                    myWindow->executeCallback(event);
                } break;
                case GLFW_REPEAT: {
                    KeyRepeatEvent event;
                    event.EventWindow = myWindow;
                    event.Type = EWindowEventType::KeyRepeat;
                    event.KeyCode = static_cast<EKeyCode>(key);
                    myWindow->executeCallback(event);
                }
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32_t character) {
            auto myWindow = getWindow(window);
            KeyTypeCharEvent event{};
            event.EventWindow = myWindow;
            event.Char = character;
            event.Type = EWindowEventType::KeyTypeChar;
            myWindow->executeCallback(event);
        });

        setCloseCallback();
    }

    void Window::executeCallback(const WindowEvent& event) {
        if (m_Spec.EventCallback) {
            m_Spec.EventCallback(event);
        }
    }

    void Window::setCloseCallback() {
        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            auto myWindow = getWindow(window);

            WindowCloseEvent event{};
            event.EventWindow = myWindow;
            event.Type = EWindowEventType::WindowClose;

            myWindow->executeCallback(event);

            // Send event to platfom
            destroyWindow(myWindow);
        });
    }

} // namespace glass::platform