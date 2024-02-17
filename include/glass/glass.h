#pragma once

#include "cstdint"
#include "string"
#include "string_view"
#include "variant"
#include "format"
#include "functional"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#ifdef GLASS_SHARED
    #ifdef _MSC_VER
        #ifdef GLASS_BUILD_SHARED
            #define GLASS_API __declspec(dllexport)
        #else
            #define GLASS_API __declspec(dllimport)
        #endif
    #else
        #define GLASS_API
    #endif
#else
    #define GLASS_API
#endif

namespace glass {
    namespace platform {
        /**
         * @brief Initialize windowing system.
         * Note: Do not use this if you are using something other than GLFW
         */
        GLASS_API bool init();

        /**
         * @brief Terminate windowing system.
         */
        GLASS_API void shutdown();

        /**
         * @brief Poll platform events.
         */
        GLASS_API bool pollEvents();

        /**
         * @brief
         */
        GLASS_API void requestExit();

        class Window;

        struct WindowSize {
            /** Width of the window */
            int32_t Width{};

            /** Height of the window */
            int32_t Height{};
        };

        struct WindowEvent;
        using GLASS_PFN_WindowEventCallback = void (*)(const WindowEvent&);

        struct WindowSpec {
            /** Size of the window */
            WindowSize Size{};

            /** Title of the window (displayed in window titlebar) */
            std::string Title{};

            /** Another window to share context data with. Used for multi-windowed rendering. Omit if only single window will be used. */
            Window* SharedContext{};

            /** Window event callback */
            GLASS_PFN_WindowEventCallback EventCallback{};

            /** Should window be fullscreen */
            bool Fullscreen : 1 = false;

            /** Should window be maximized */
            bool Maximized : 1 = false;

            /** Should window be resizable */
            bool Resizable : 2 = false;

            /** Should window be created in visible state. */
            bool Visible : 2 = true;

            /** Should window be focused on create */
            bool Focused : 2 = true;
        };

        enum class EWindowEventType {
            WindowResize,
            WindowMove,
            WindowMaximize,
            WindowMinimize,
            WindowRestore,
            WindowFocus,
            WindowUnfocus,
            MouseEnterWindow,
            MouseLeaveWindow,
            MouseMove,
            MouseButtonPress,
            MouseButtonRelease,
            MouseScroll,
            KeyPress,
            KeyRelease,
            KeyRepeat,
            KeyTypeChar,
            WindowClose,
        };

        enum class EMouseButton {
            Undefined = -1,
            Left,
            Right,
            Middle,
            X1,
            X2,
        };

        static constexpr std::string_view toString(EMouseButton button) {
            switch (button) {
                case EMouseButton::Undefined:
                    return "Undefined";
                case EMouseButton::Left:
                    return "Left";
                case EMouseButton::Right:
                    return "Right";
                case EMouseButton::Middle:
                    return "Middle";
                case EMouseButton::X1:
                    return "X1";
                case EMouseButton::X2:
                    return "X2";
            }

            return "Undefined";
        }

        enum class EKeyCode {
            Undefined = 0,
            Space = 32,
            Apostrophe = 39,
            Comma = 44,
            Minus = 45,
            Period = 46,
            Slash = 47,
            Key0 = 48,
            Key1 = 49,
            Key2 = 50,
            Key3 = 51,
            Key4 = 52,
            Key5 = 53,
            Key6 = 54,
            Key7 = 55,
            Key8 = 56,
            Key9 = 57,
            Semicolon = 59,
            Equal = 61,
            A = 65,
            B = 66,
            C = 67,
            D = 68,
            E = 69,
            F = 70,
            G = 71,
            H = 72,
            I = 73,
            J = 74,
            K = 75,
            L = 76,
            M = 77,
            N = 78,
            O = 79,
            P = 80,
            Q = 81,
            R = 82,
            S = 83,
            T = 84,
            U = 85,
            V = 86,
            W = 87,
            X = 88,
            Y = 89,
            Z = 90,
            LeftBracket = 91,
            BackSlash = 92,
            RightBracket = 93,
            GraveAccent = 96,
            World1 = 161,
            World2 = 162,
            Escape = 256,
            Enter = 257,
            Tab = 258,
            Backspace = 259,
            Insert = 260,
            Delete = 261,
            Right = 262,
            Left = 263,
            Down = 264,
            Up = 265,
            PageUp = 266,
            PageDown = 267,
            Home = 268,
            End = 269,
            CapsLock = 280,
            ScrollLock = 281,
            NumLock = 282,
            PrintScreen = 283,
            Pause = 284,
            F1 = 290,
            F2 = 291,
            F3 = 292,
            F4 = 293,
            F5 = 294,
            F6 = 295,
            F7 = 296,
            F8 = 297,
            F9 = 298,
            F10 = 299,
            F11 = 300,
            F12 = 301,
            F13 = 302,
            F14 = 303,
            F15 = 304,
            F16 = 305,
            F17 = 306,
            F18 = 307,
            F19 = 308,
            F20 = 309,
            F21 = 310,
            F22 = 311,
            F23 = 312,
            F24 = 313,
            F25 = 314,
            Num0 = 320,
            Num1 = 321,
            Num2 = 322,
            Num3 = 323,
            Num4 = 324,
            Num5 = 325,
            Num6 = 326,
            Num7 = 327,
            Num8 = 328,
            Num9 = 329,
            NumDecimal = 330,
            NumDivide = 331,
            NumMultiply = 332,
            NumSubtract = 333,
            NumAdd = 334,
            NumEnter = 335,
            NumEqual = 336,
            LeftShift = 340,
            LeftControl = 341,
            LeftAlt = 342,
            LeftSuper = 343,
            RightShift = 344,
            RightControl = 345,
            RightAlt = 346,
            RightSuper = 347,
            Menu = 348,
        };

#define INTERNAL_GLASS_KEYCODE_TO_STRING(code) \
    case EKeyCode::##code:                     \
        return (#code)

        static constexpr std::string_view toString(EKeyCode code) {
            switch (code) {
                INTERNAL_GLASS_KEYCODE_TO_STRING(Space);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Apostrophe);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Comma);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Minus);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Period);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Slash);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Key0);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Key1);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Key2);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Key3);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Key4);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Key5);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Key6);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Key7);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Key8);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Key9);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Semicolon);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Equal);
                INTERNAL_GLASS_KEYCODE_TO_STRING(A);
                INTERNAL_GLASS_KEYCODE_TO_STRING(B);
                INTERNAL_GLASS_KEYCODE_TO_STRING(C);
                INTERNAL_GLASS_KEYCODE_TO_STRING(D);
                INTERNAL_GLASS_KEYCODE_TO_STRING(E);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F);
                INTERNAL_GLASS_KEYCODE_TO_STRING(G);
                INTERNAL_GLASS_KEYCODE_TO_STRING(H);
                INTERNAL_GLASS_KEYCODE_TO_STRING(I);
                INTERNAL_GLASS_KEYCODE_TO_STRING(J);
                INTERNAL_GLASS_KEYCODE_TO_STRING(K);
                INTERNAL_GLASS_KEYCODE_TO_STRING(L);
                INTERNAL_GLASS_KEYCODE_TO_STRING(M);
                INTERNAL_GLASS_KEYCODE_TO_STRING(N);
                INTERNAL_GLASS_KEYCODE_TO_STRING(O);
                INTERNAL_GLASS_KEYCODE_TO_STRING(P);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Q);
                INTERNAL_GLASS_KEYCODE_TO_STRING(R);
                INTERNAL_GLASS_KEYCODE_TO_STRING(S);
                INTERNAL_GLASS_KEYCODE_TO_STRING(T);
                INTERNAL_GLASS_KEYCODE_TO_STRING(U);
                INTERNAL_GLASS_KEYCODE_TO_STRING(V);
                INTERNAL_GLASS_KEYCODE_TO_STRING(W);
                INTERNAL_GLASS_KEYCODE_TO_STRING(X);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Y);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Z);
                INTERNAL_GLASS_KEYCODE_TO_STRING(LeftBracket);
                INTERNAL_GLASS_KEYCODE_TO_STRING(BackSlash);
                INTERNAL_GLASS_KEYCODE_TO_STRING(RightBracket);
                INTERNAL_GLASS_KEYCODE_TO_STRING(GraveAccent);
                INTERNAL_GLASS_KEYCODE_TO_STRING(World1);
                INTERNAL_GLASS_KEYCODE_TO_STRING(World2);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Escape);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Enter);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Tab);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Backspace);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Insert);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Delete);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Right);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Left);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Down);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Up);
                INTERNAL_GLASS_KEYCODE_TO_STRING(PageUp);
                INTERNAL_GLASS_KEYCODE_TO_STRING(PageDown);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Home);
                INTERNAL_GLASS_KEYCODE_TO_STRING(End);
                INTERNAL_GLASS_KEYCODE_TO_STRING(CapsLock);
                INTERNAL_GLASS_KEYCODE_TO_STRING(ScrollLock);
                INTERNAL_GLASS_KEYCODE_TO_STRING(NumLock);
                INTERNAL_GLASS_KEYCODE_TO_STRING(PrintScreen);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Pause);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F1);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F2);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F3);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F4);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F5);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F6);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F7);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F8);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F9);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F10);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F11);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F12);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F13);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F14);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F15);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F16);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F17);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F18);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F19);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F20);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F21);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F22);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F23);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F24);
                INTERNAL_GLASS_KEYCODE_TO_STRING(F25);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Num0);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Num1);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Num2);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Num3);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Num4);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Num5);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Num6);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Num7);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Num8);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Num9);
                INTERNAL_GLASS_KEYCODE_TO_STRING(NumDecimal);
                INTERNAL_GLASS_KEYCODE_TO_STRING(NumDivide);
                INTERNAL_GLASS_KEYCODE_TO_STRING(NumMultiply);
                INTERNAL_GLASS_KEYCODE_TO_STRING(NumSubtract);
                INTERNAL_GLASS_KEYCODE_TO_STRING(NumAdd);
                INTERNAL_GLASS_KEYCODE_TO_STRING(NumEnter);
                INTERNAL_GLASS_KEYCODE_TO_STRING(NumEqual);
                INTERNAL_GLASS_KEYCODE_TO_STRING(LeftShift);
                INTERNAL_GLASS_KEYCODE_TO_STRING(LeftControl);
                INTERNAL_GLASS_KEYCODE_TO_STRING(LeftAlt);
                INTERNAL_GLASS_KEYCODE_TO_STRING(LeftSuper);
                INTERNAL_GLASS_KEYCODE_TO_STRING(RightShift);
                INTERNAL_GLASS_KEYCODE_TO_STRING(RightControl);
                INTERNAL_GLASS_KEYCODE_TO_STRING(RightAlt);
                INTERNAL_GLASS_KEYCODE_TO_STRING(RightSuper);
                INTERNAL_GLASS_KEYCODE_TO_STRING(Menu);
            }
            return "Unknown";
        }

        struct GLASS_API WindowEvent {
            Window* EventWindow{};
            EWindowEventType Type{};

            inline virtual std::string toString() const { return "WindowEvent"; }
        };

        struct GLASS_API WindowResizeEvent final : public WindowEvent {
            WindowSize NewSize{};

            inline virtual std::string toString() const final { return std::format("WindowResizeEvent: [w: {}, h: {}]", NewSize.Width, NewSize.Height); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::WindowResize; }
        };

        struct GLASS_API WindowMoveEvent final : public WindowEvent {
            int32_t PosX{};
            int32_t PosY{};

            inline virtual std::string toString() const final { return std::format("WindowMoveEvent: [x: {}, y: {}]", PosX, PosY); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::WindowMove; }
        };

        struct GLASS_API WindowMaximizeEvent final : public WindowEvent {
            inline virtual std::string toString() const final { return std::format("WindowMaximizeEvent"); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::WindowMaximize; }
        };

        struct GLASS_API WindowCloseEvent final : public WindowEvent {
            inline virtual std::string toString() const final { return std::format("WindowCloseEvent"); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::WindowClose; }
        };

        struct GLASS_API WindowMinimizeEvent final : public WindowEvent {
            inline virtual std::string toString() const final { return std::format("WindowMinimizeEvent"); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::WindowMinimize; }
        };

        struct GLASS_API WindowRestoreEvent final : public WindowEvent {
            inline virtual std::string toString() const final { return std::format("WindowRestoreEvent"); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::WindowRestore; }
        };

        struct GLASS_API WindowFocusEvent final : public WindowEvent {
            inline virtual std::string toString() const final { return std::format("WindowFocusEvent"); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::WindowFocus; }
        };

        struct GLASS_API WindowUnfocusEvent final : public WindowEvent {
            inline virtual std::string toString() const final { return std::format("WindowUnfocusEvent"); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::WindowUnfocus; }
        };

        struct GLASS_API MouseEnterWindowEvent final : public WindowEvent {
            inline virtual std::string toString() const final { return std::format("MouseEnterWindowEvent"); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::MouseEnterWindow; }
        };

        struct GLASS_API MouseLeaveWindowEvent final : public WindowEvent {
            inline virtual std::string toString() const final { return std::format("MouseLeaveWindowEvent"); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::MouseLeaveWindow; }
        };

        struct GLASS_API MouseMoveEvent final : public WindowEvent {
            double X{};
            double Y{};
            inline virtual std::string toString() const final { return std::format("MouseMoveEvent: [x: {}, y: {}]", X, Y); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::MouseMove; }
        };

        struct GLASS_API MouseButtonPressEvent final : public WindowEvent {
            EMouseButton Button{};
            inline virtual std::string toString() const final { return std::format("MouseButtonPressEvent: [x: {}]", platform::toString(Button)); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::MouseButtonPress; }
        };

        struct GLASS_API MouseButtonReleaseEvent final : public WindowEvent {
            EMouseButton Button{};
            inline virtual std::string toString() const final { return std::format("MouseButtonReleaseEvent: [x: {}]", platform::toString(Button)); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::MouseButtonRelease; }
        };

        struct GLASS_API MouseScrollEvent final : public WindowEvent {
            double Horizontal{};
            double Vertical{};
            inline virtual std::string toString() const final { return std::format("MouseScrollEvent: [h: {}, v: {}]", Horizontal, Vertical); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::MouseScroll; }
        };

        struct GLASS_API KeyPressEvent final : public WindowEvent {
            EKeyCode KeyCode{};

            inline virtual std::string toString() const final { return std::format("KeyPressEvent: [key: {}]", platform::toString(KeyCode)); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::KeyPress; }
        };

        struct GLASS_API KeyReleaseEvent final : public WindowEvent {
            EKeyCode KeyCode{};

            inline virtual std::string toString() const final { return std::format("KeyReleaseEvent: [key: {}]", platform::toString(KeyCode)); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::KeyRelease; }
        };

        struct GLASS_API KeyRepeatEvent final : public WindowEvent {
            EKeyCode KeyCode{};

            inline virtual std::string toString() const final { return std::format("KeyRepeatEvent: [key: {}]", platform::toString(KeyCode)); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::KeyRepeat; }
        };

        struct GLASS_API KeyTypeCharEvent final : public WindowEvent {
            uint32_t Char{};

            inline virtual std::string toString() const final { return std::format("KeyTypeCharEven: [char: {}]", static_cast<char>(Char)); }
            static constexpr EWindowEventType getStaticType() { return EWindowEventType::KeyTypeChar; }
        };

        struct EventDispatcher {
            const WindowEvent& Event;

            inline explicit EventDispatcher(const WindowEvent& event)
                : Event(event) {}

            template <typename EventType>
            void dispatch(void (*callback)(const EventType&)) {
                if (EventType::getStaticType() == Event.Type) {
                    callback(static_cast<const EventType&>(Event));
                }
            }

            template <typename EventType, typename HandlerType>
            void dispatch(HandlerType* handler, void (HandlerType::*callback)(const EventType&)) {
                if (EventType::getStaticType() == Event.Type) {
                    (handler->*callback)(static_cast<const EventType&>(Event));
                }
            }
        };

        /**
         * @brief Creates a platform window (this is a GLFWwindow wrapper for a little bit of simplification)
         * @return A valid handle to a window
         * @param windowSpec The specification of the window.
         */
        GLASS_API Window* createWindow(const WindowSpec& windowSpec);

        /**
         * @brief Destroys the window
         * @param window The window that is going to be destroyed
         */
        GLASS_API void destroyWindow(const Window* window);

        /**
         * @brief Show the window if it was hidden on create or just hidden.
         */
        GLASS_API void showWindow(const Window* window);

        /**
         * @brief Hide the window.
         */
        GLASS_API void hideWindow(const Window* window);

        /**
         * @brief Gets window client area size
         */
        GLASS_API WindowSize getWindowSize(const Window* window);

        /**
         * @brief Checks if main window is alive
         */
        GLASS_API bool isMainWindowAlive();

        /**
         * @brief Checks if there are any windows alive
         */
        GLASS_API bool isAnyWindowAlive();

        /**
         * @brief Sets new event callback
         */
        GLASS_API void setWindowEventCallback(Window* window, GLASS_PFN_WindowEventCallback callback);

        /** @brief Get key pressed state. */
        GLASS_API bool isKeyDown(EKeyCode key);

        /** @brief Get mouse button pressed state */
        GLASS_API bool isMouseButtonDown(EMouseButton button);

        struct MouseVec2 {
            double X;
            double Y;
        };

        /** Gets mouse position. */
        GLASS_API MouseVec2 getMousePosition();

        /** Gets mouse offset from previous frame */
        GLASS_API MouseVec2 getMouseOffset();

        /** Get time elapsed since previous call to pollEvents() */
        GLASS_API double getDeltaTime();

        /** Get current application time in seconds. */
        GLASS_API double getTime();

        /** Enable or disable cursor */
        GLASS_API void enableCursor(const Window* window);
        GLASS_API void disableCursor(const Window* window);
    } // namespace platform

    namespace gfx {
        GLASS_API extern bool GUsesGFX;

        enum class ResourceID : uint64_t {
            Null = 0
        };

        static constexpr ResourceID NULL_RESOURCE = ResourceID::Null;
        static constexpr uint32_t INVALID_BINDING = UINT32_MAX;

        struct ContextSpec {
            /** Window to create the context for */
            const platform::Window* ContextWindow;

            /** Should vertical sync be enabled */
            bool EnableVSync{};
        };

        /** Graphics context. This holds a window and manages context resources. */
        class Context;

        /**
         * @brief Create a graphics context handle.
         * @param spec The specification of the graphics context
         * @return OpenGL context handle.
         */
        GLASS_API Context* createContext(const ContextSpec& spec);

        /**
         * @brief Destroy graphics context and uninitialize it's resources.
         * @param context A graphics context handle. Can be null.
         */
        GLASS_API void destroyContext(const Context* context);

        /**
         * @brief Get associated context by window handle.
         * @return A valid context handle if context for the window exists.
         */
        GLASS_API Context* getContextForWindow(const platform::Window* window);

        /**
         * @brief Set the context to be the main context for drawing and graphics operations
         * @param context A graphics context handle
         */
        GLASS_API void makeContextCurrent(const Context* context);

        enum EClearFlags {
            ECF_Color = 1 << 0,
            ECF_Depth = 1 << 1,
            ECF_Stencil = 1 << 2,
            ECF_DepthStencil = ECF_Depth | ECF_Stencil,
            ECF_All = ECF_Color | ECF_DepthStencil,
        };
        using EClearFlagsMask = uint32_t;

        /**
         * @brief Clear context viewport.
         * @param clearFlags A valid mask of EClearFlags components (e.g. ECF_Color | ECF_Depth)
         * @param clearColor A color to use for clearing the buffer. May be null.
         */
        GLASS_API void clearViewport(EClearFlagsMask clearFlags, const glm::vec4* clearColor = nullptr);

        /** Viewport struct to determin draw area. */
        struct Viewport2D {
            /** Top left corner X of draw area */
            int32_t X{ 0 };

            /** Top left corner Y of draw area */
            int32_t Y{ 0 };

            /** Width of draw area. Leave at 0 to deduce from window */
            int32_t Width{ 0 };

            /** Height of draw area. Leave at 0 to deduce from window */
            int32_t Height{ 0 };
        };

        /**
         * @brief Set viewport data
         */
        GLASS_API void setViewport(const Viewport2D& viewport = {});

        /**
         * @brief Set depth test enabled or disabled
         */
        GLASS_API void enableDepthTest();
        GLASS_API void disableDepthTest();

        /** Poligon winding */
        enum EPoligonWinding {
            EPW_Clockwise,
            EPW_CounterClockwise,
        };

        /**
         * @brief Set poligon winding
         */
        GLASS_API void setPoligonWinding(EPoligonWinding winding);

        /** Culling mode */
        enum ECullMode {
            ECM_None,
            ECM_Back,
            ECM_Front,
            ECM_BackAndFront,
        };

        /**
         * @brief Set culling enabled or disabled
         */
        GLASS_API void setCullMode(ECullMode mode);

        /** Polygon fill mode */
        enum EFillMode {
            EFM_Solid,
            EFM_Wireframe,
        };

        /**
         * @brief Set fill mode
         */
        void setFillMode(EFillMode mode);

        /**
         * @brief Swap context buffers.
         */
        GLASS_API void present(const Context* context);

        /**
         * Resources API
         */

        /** How the buffer is going to be used */
        enum EBufferUsage {
            /** The buffer will be used for drawing */
            EBU_Draw,

            /** The buffer will be used for copying data to other resources. */
            EBU_Copy,

            /** The buffer will be using for readback from GPU. */
            EBU_Read,
        };

        /** How often the buffer is going to be updated */
        enum EBufferMutability {
            /** Buffer contents are going to be assigned once and never or rarely updated. */
            EBM_Static,

            /** Buffer contents are going to be updated frequently (e.g. once per frame) */
            EBM_Dynamic,

            /** Buffer is created for data streaming and will be updated rarely or never. */
            EBM_Stream,
        };

        /** What type of buffer this is */
        enum EBufferType : uint16_t {
            /** Invalid case */
            EBT_Unknown,

            /** The buffer is a vertex buffer */
            EBT_Vertex,

            /** The buffer is an index buffer */
            EBT_Element,

            /** The buffer is a uniform buffer */
            EBT_Uniform,

            /** The buffer is a storage buffer */
            EBT_Storage,
        };

        /** Value type */
        enum EValueType {
            EVT_Float,
            EVT_Int,
            EVT_UInt,
        };

        enum EBufferDataRate {
            EBDR_PerVertex,
            EBDR_PerInstance,
        };

        /** Buffer input element. */
        struct BufferInputElement {
            /** Use this value to automatically generate the input layout */
            EValueType Type{};
            uint32_t Count{};
            bool Normalize{ false };
            EBufferDataRate DataRate{ EBDR_PerVertex };
        };

        /** Buffer input layout */
        class GLASS_API BufferInputLayout {
        public:
            BufferInputLayout& add(EValueType type, uint32_t componentCount, bool normalize = false, EBufferDataRate dataRate = EBDR_PerVertex);
            inline const std::vector<BufferInputElement>& getElements() const { return m_Elements; }

        private:
            std::vector<BufferInputElement> m_Elements{};
        };

        /** Specification of the buffer */
        struct BufferSpec {
            /** The size of allocated memory for buffer (in bytes) */
            uint64_t SizeInBytes{};

            /** The size of a single element of the buffer (e.g. Vertex size for vertex buffer or element size for storage buffer) */
            uint64_t StrideInBytes{};

            /** How the buffer is going to be used. */
            EBufferUsage Usage{ EBU_Draw };

            /** How often the buffer is going to be modified */
            EBufferMutability Mutability{ EBM_Static };

            /** What kind of buffer this is. */
            EBufferType BufferType{ EBT_Unknown };

            /** Used for vertex buffer. The vertex input layout. */
            const BufferInputLayout* InputLayout{};

            /** Used to initialize buffer with data. */
            const void* InitialData{};

            /** Used to initialize buffer with data */
            uint64_t InitialDataSize{};
        };

        /**
         * @brief Creates buffer.
         * @param spec Buffer specification.
         * @return ResourceID the id of the buffer.
         */
        GLASS_API ResourceID createBuffer(const BufferSpec& spec);

        GLASS_API void destroyBuffer(ResourceID buffer);

        /**
         * Buffer helpers
         */
        template <typename VertexType>
        ResourceID createStaticVertexBuffer(uint64_t size, const VertexType* data, const BufferInputLayout* inputLayout) {
            gfx::BufferSpec spec{};
            spec.BufferType = gfx::EBT_Vertex;
            spec.StrideInBytes = sizeof(VertexType);
            spec.SizeInBytes = size;
            spec.InitialData = data;
            spec.InitialDataSize = data ? size : 0;
            spec.InputLayout = inputLayout;
            return createBuffer(spec);
        }

        template <typename VertexType, uint64_t size>
        ResourceID createStaticVertexBuffer(VertexType (&data)[size], const BufferInputLayout* inputLayout) {
            gfx::BufferSpec spec{};
            spec.BufferType = gfx::EBT_Vertex;
            spec.StrideInBytes = sizeof(VertexType);
            spec.SizeInBytes = size * sizeof(VertexType);
            spec.InitialData = data;
            spec.InitialDataSize = size * sizeof(VertexType);
            spec.InputLayout = inputLayout;
            return createBuffer(spec);
        }

        template <typename VertexType>
        ResourceID createDynamicVertexBuffer(uint64_t size, const VertexType* data, const BufferInputLayout* inputLayout) {
            gfx::BufferSpec spec{};
            spec.BufferType = gfx::EBT_Vertex;
            spec.Mutability = gfx::EBM_Dynamic;
            spec.StrideInBytes = sizeof(VertexType);
            spec.SizeInBytes = size;
            spec.InitialData = data;
            spec.InitialDataSize = data ? size : 0;
            spec.InputLayout = inputLayout;
            return createBuffer(spec);
        }

        template <typename IndexType, uint64_t size>
        ResourceID createStaticElementBuffer(IndexType (&data)[size]) {
            gfx::BufferSpec spec{};
            spec.BufferType = gfx::EBT_Element;
            spec.StrideInBytes = sizeof(IndexType);
            spec.SizeInBytes = size * sizeof(IndexType);
            spec.InitialData = data;
            spec.InitialDataSize = size * sizeof(IndexType);
            return createBuffer(spec);
        }

        template <typename IndexType>
        ResourceID createDynamicElementBuffer(uint64_t size, const IndexType* data) {
            gfx::BufferSpec spec{};
            spec.BufferType = gfx::EBT_Element;
            spec.Mutability = gfx::EBM_Dynamic;
            spec.StrideInBytes = sizeof(IndexType);
            spec.SizeInBytes = size;
            spec.InitialData = data;
            spec.InitialDataSize = size;
            return createBuffer(spec);
        }

        template <typename StructType>
        ResourceID createUniformBuffer(const StructType& data) {
            gfx::BufferSpec spec{};
            spec.BufferType = gfx::EBT_Uniform;
            spec.Mutability = gfx::EBM_Dynamic;
            spec.StrideInBytes = sizeof(StructType);
            spec.SizeInBytes = sizeof(StructType);
            spec.InitialData = &data;
            spec.InitialDataSize = sizeof(StructType);
            return createBuffer(spec);
        }

        template <typename StructType>
        ResourceID createUniformBuffer() {
            gfx::BufferSpec spec{};
            spec.BufferType = gfx::EBT_Uniform;
            spec.Mutability = gfx::EBM_Dynamic;
            spec.StrideInBytes = sizeof(StructType);
            spec.SizeInBytes = sizeof(StructType);
            return createBuffer(spec);
        }

        /**
         * @brief Writes buffer data
         * @param buffer the buffer to update
         * @param data Data to write to the buffer
         * @param dataSize Size of the data written to the buffer
         * @param offset offset in bytes from buffer start
         */
        GLASS_API void writeBufferData(ResourceID buffer, const void* data, uint64_t dataSize, uint64_t offset = 0);

        /** Bind vertex buffer to the pipeline */
        GLASS_API void bindVertexBuffer(ResourceID buffer);

        /** Bind element buffer to the pipeline */
        GLASS_API void bindElementBuffer(ResourceID buffer);

        /** Type of texture (dimensions) */
        enum ETextureType : uint16_t {
            /** 1D texture (x only) */
            ETT_Texture1D,

            /** 2D texture (xy) */
            ETT_Texture2D,

            /** 3D texture (xyz) */
            ETT_Texture3D,

            /** Cube texture (made of 6 2D planes) */
            ETT_TextureCube,
        };

        /** Pixel format of the texture */
        enum EPixelFormat : uint16_t {
            /** Invalid pixel format */
            EPF_Undefined,

            /** RGB 24 bit format */
            EPF_RGB8,

            /** RGBA 32 bit format */
            EPF_RGBA8,

            /** RGB 32bit format with HDR support */
            EPF_R11G11B10F,

            /** D24S8 bit format for handling depth stencil components */
            EPF_DepthStencil,
        };

        /** Texture filtering mode */
        enum ETextureFilter {
            /** Linear interpolation between pixels, smoother colors */
            ETF_Linear,

            /** Non-interpolated mode. More pixelated look. */
            ETF_Nearest,
        };

        /** Wrapping mode for the texture. */
        enum ETextureWrapMode {
            /**  The default behavior for textures. Repeats the texture image. */
            ETWM_Repeat,

            /** Same as Repeat but mirrors the image with each repeat */
            ETWM_MirroredRepeat,

            /**  Clamps the coordinates between 0 and 1. The result is that higher coordinates become clamped to the edge, resulting in a stretched edge pattern. */
            ETWM_ClampToEdge,

            /** Coordinates outside the range are now given a user-specified border color. */
            ETWM_ClampToBorder,
        };

        /** Specification for texture sampler */
        struct SamplerSpec {
            ETextureFilter MinFilter{ ETF_Linear };
            ETextureFilter MagFilter{ ETF_Linear };
            ETextureWrapMode WrapModeS{ ETWM_ClampToEdge };
            ETextureWrapMode WrapModeT{ ETWM_ClampToEdge };
            ETextureWrapMode WrapModeU{ ETWM_ClampToEdge };
        };

        /** Specification for texture creation */
        struct TextureSpec {
            ETextureType Type{ ETT_Texture2D };
            EPixelFormat Format{ EPF_RGBA8 };
            int32_t Width{ 1 };
            int32_t Height{ 1 };
            int32_t Depth{ 1 };
            bool GenerateMipmaps{ true };
            SamplerSpec Sampler{};

            const void* InitialData{};
        };

        GLASS_API ResourceID createTexture(const TextureSpec& spec);

        GLASS_API void destroyTexture(ResourceID texture);

        /**
         * FRAMEBUFFERS
         */
        static constexpr auto MAX_COLOR_ATTACHMENTS = 8u;

        /** Specification for the frame buffer */
        struct FrameBufferSpec {
            /** The width of the framebuffer */
            int32_t Width{};

            /** The height of the framebuffer */
            int32_t Height{};

            /** Color attachment formats. 8 possible. If AttachmentFormat is not EPF_Undefined the texture will be created. */
            EPixelFormat ColorAttachmentFormats[MAX_COLOR_ATTACHMENTS]{};

            /** Pixel format of the depth stencil attachment. If Attachment format is not EPF_Undefined the texture will be created. */
            EPixelFormat DepthAttachmentFormat = EPF_Undefined;
        };

        class FrameBuffer;

        GLASS_API FrameBuffer* createFrameBuffer(const FrameBufferSpec& spec);
        GLASS_API void destroyFrameBuffer(FrameBuffer* frameBuffer);

        /** Bind framebuffer to the pipeline. Use nullptr to bind default framebuffer. */
        GLASS_API void setFrameBuffer(FrameBuffer* frameBuffer, bool updateViewport = true);

        /**
         * @brief Retrieve frame buffer color attachment if it exists.
         * @param fb FrameBuffer to get the texture from
         * @param index The index of the color attachment
         * @return A valid texture id if it exists, ResourceID::Null otherwise
         */
        GLASS_API ResourceID getFrameBufferColorAttachmentTexture(const FrameBuffer* fb, uint8_t index);

        /**
         * @brief Retrieve frame buffer depth stencil attachment texture if it exists.
         * @param fb FrameBuffer to get the texture from
         * @return A valid texture id if it exists, ResourceID::Null otherwise
         */
        GLASS_API ResourceID getFrameBufferDepthStencilAttachment(const FrameBuffer* fb);

        /**
         * @brief Retrieve frame buffer width.
         * @param fb A valid frame buffer handle. Cannot be null.
         * @return Width of the frame buffer
         */
        GLASS_API uint32_t getFrameBufferWidth(const FrameBuffer* fb);

        /**
         * @brief Retrieve frame buffer height.
         * @param fb A valid frame buffer handle. Cannot be null.
         * @return Height of the frame buffer
         */
        GLASS_API uint32_t getFrameBufferHeight(const FrameBuffer* fb);

        /**
         * @brief Resize the frame buffer.
         * @param fb A valid frame buffer handle
         * @param width New width of the frame buffer
         * @param height New height of the frame buffer
         * 
         * Resizing of the frame buffer happens only if the width or height of the frame buffer are different from specified values.
         * New textures are created then.
         */
        GLASS_API void resizeFrameBuffer(FrameBuffer* fb, uint32_t width, uint32_t height);

        /**
         * SHADER API
         */
        enum EShaderType {
            EST_VertexShader,
            EST_FragmentShader,
            EST_ComputeShader,
            EST_GeometryShader,
            EST_TesellationControlShader,
            EST_TesellationEvaluationShader,
        };

        class Shader;
        class ShaderProgram;

        struct ProgramSpec {
            Shader* VertexShader{};
            Shader* FragmentShader{};
            Shader* ComputeShader{};
            Shader* GeometryShader{};
            Shader* TesellationControlShader{};
            Shader* TesellationEvaluationShader{};
        };

        /**
         * Get or create shader. If shader was already created with this path, a valid instance will be returned.
         */
        GLASS_API Shader* getOrCreateShader(const std::string& path, EShaderType type);

        /**
         * Get or create shader program. If program already exists, it will be returned.
         */
        GLASS_API ShaderProgram* getOrCreateShaderProgram(const ProgramSpec& spec);

        /**
         * Binds shader program to the pipeline.
         */
        GLASS_API void bindShaderProgram(const ShaderProgram* program);

        /**
         * Set shader program uniforms
         */
        GLASS_API void setUniform(const ShaderProgram* program, const char* name, float uniform);
        GLASS_API void setUniform(const ShaderProgram* program, const char* name, glm::vec2 uniform);
        GLASS_API void setUniform(const ShaderProgram* program, const char* name, glm::vec3 uniform);
        GLASS_API void setUniform(const ShaderProgram* program, const char* name, glm::vec4 uniform);

        GLASS_API void setUniform(const ShaderProgram* program, const char* name, int32_t uniform);
        GLASS_API void setUniform(const ShaderProgram* program, const char* name, glm::ivec2 uniform);
        GLASS_API void setUniform(const ShaderProgram* program, const char* name, glm::ivec3 uniform);
        GLASS_API void setUniform(const ShaderProgram* program, const char* name, glm::ivec4 uniform);

        GLASS_API void setUniform(const ShaderProgram* program, const char* name, uint32_t uniform);
        GLASS_API void setUniform(const ShaderProgram* program, const char* name, glm::uvec2 uniform);
        GLASS_API void setUniform(const ShaderProgram* program, const char* name, glm::uvec3 uniform);
        GLASS_API void setUniform(const ShaderProgram* program, const char* name, glm::uvec4 uniform);

        GLASS_API void setUniform(const ShaderProgram* program, const char* name, const glm::mat3& uniform, bool transpose = false);
        GLASS_API void setUniform(const ShaderProgram* program, const char* name, const glm::mat4& uniform, bool transpose = false);

        /**
         * @brief Bind texture to the shader program uniform slot.
         * @param program Shader program to bind the texture to
         * @param name The name of the uniform
         * @param id A texture to bind to the uniform slot
         * @param slot (optional) binding slot. For binding multiple textures.
         */
        GLASS_API void setUniformTexture(const ShaderProgram* program, const char* name, ResourceID texture, uint32_t slot = 0);
        /**
         * @brief Bind a uniform buffer to the pipeline
         * @param program An instance of a shader program to use for binding lookup
         * @param name Name of the binding in the GLSL code (can be null if optBinding is specified).
         * @param id Id of the buffer to bind
         * @param optBinding An optional binding slot, can be INVALID_BINDING if a name is specified.
         */
        GLASS_API void setUniformBuffer(const ShaderProgram* program, const char* name, ResourceID buffer, uint32_t optBinding = INVALID_BINDING);

        /**
         * DRAWING
         */
        enum EPrimitiveTopology {
            EPT_Triangles,
            EPT_Lines,
            EPT_Points,
        };

        enum EIndexType {
            EIT_UInt16,
            EIT_UInt32,
        };

        /**
         * Draw arrays.
         * @param topology Primitive topology (triangles, lines or points)
         * @param vertexCount number of vertices to draw
         * @param firstVertex the vertex to begin from (offset from the start of the buffer)
         */
        void draw(EPrimitiveTopology topology, uint32_t vertexCount, uint32_t firstVertex = 0);

        /**
         * Draw elements.
         * @param topology Primitive topology (triangles, lines or points)
         * @param indexCount number of indices to draw
         * @param indexType type of index.
         */
        void drawElements(EPrimitiveTopology topology, uint32_t indexCount, EIndexType indexType = EIT_UInt32);
    } // namespace gfx
} // namespace glass