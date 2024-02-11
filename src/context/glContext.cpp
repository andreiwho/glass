#include "glad/glad.h"

#include "glContext.h"
#include "print"

#include "cassert"
#include "ranges"

namespace glass::gfx {

    static void glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
        std::string_view messageSource{};
        switch (source) {
            case GL_DEBUG_SOURCE_API:
                messageSource = "API";
                break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                messageSource = "WindowSystem";
                break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                messageSource = "ShaderCompiler";
                break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:
                messageSource = "ThirdParty";
                break;
            case GL_DEBUG_SOURCE_APPLICATION:
                messageSource = "Application";
                break;
            case GL_DEBUG_SOURCE_OTHER:
                messageSource = "Other";
                break;
        }

        std::string_view messageType{};
        switch (type) {
            case GL_DEBUG_TYPE_ERROR:
                messageType = "Error";
                break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                messageType = "DeprecatedBehavior";
                break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                messageType = "UndefinedBehavior";
                break;
            case GL_DEBUG_TYPE_PORTABILITY:
                messageType = "Portability";
                break;
            case GL_DEBUG_TYPE_PERFORMANCE:
                messageType = "Performance";
                break;
            case GL_DEBUG_TYPE_OTHER:
                messageType = "Other";
                break;
        }

        switch (severity) {
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                std::println("GLASS OpenGL debug (Notification, {}, {}): {}", messageSource, messageType, message);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                std::println("GLASS OpenGL debug (Low, {}, {}): {}", messageSource, messageType, message);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                std::println("GLASS OpenGL debug (Medium, {}, {}): {}", messageSource, messageType, message);
                break;
            case GL_DEBUG_SEVERITY_HIGH:
                std::println("GLASS OpenGL debug (High, {}, {}): {}", messageSource, messageType, message);
                break;
        }
    }

    struct ContextData {
        std::unordered_map<const platform::Window*, std::shared_ptr<Context>> Contexts;
        bool FunctionsLoaded{};

        Context* findContext(const platform::Window* window) const {
            return Contexts.at(window).get();
        }

        void destroyContext(const Context* context) {
            auto window = context->getWindow();
            if (Contexts.contains(window)) {
                Contexts.erase(window);
            }
        }
    };

    static std::unique_ptr<ContextData> GContextData{};

    void init() {
        GContextData = std::make_unique<ContextData>();
    }

    void shutdown() {
        GContextData.reset();
        GContextData = nullptr;
    }

    Context* createContext(const ContextSpec& spec) {
        if (!GContextData) {
            init();
        }

        assert(GContextData && "Graphics layer was not initialized. Did you forget to call glass::gfx::init()?");
        std::shared_ptr<Context> context = std::make_shared<Context>(spec);
        GContextData->Contexts[spec.ContextWindow] = context;
        return context.get();
    }

    void destroyContext(const Context* context) {
        assert(GContextData && "Graphics layer was not initialized. Did you forget to call glass::gfx::init()?");
        GContextData->destroyContext(context);

        if (GContextData->Contexts.empty()) {
            shutdown();
        }
    }

    Context* getContextForWindow(const platform::Window* window) {
        assert(GContextData && "Graphics layer was not initialized. Did you forget to call glass::gfx::init()?");
        return GContextData->findContext(window);
    }

    void present(const Context* context) {
        const_cast<Context*>(context)->present();
    }

    Context::Context(const ContextSpec& spec)
        : m_Window(spec.ContextWindow)
        , m_VSyncEnabled(spec.EnableVSync) {
        makeContextCurrent(this);

        if (!GContextData->FunctionsLoaded) {
            if (!gladLoadGL()) {
                std::println("GLASS: Failed to load OpenGL functions.");
                return;
            }

            GContextData->FunctionsLoaded = true;
        }

        glDebugMessageCallback(glDebugCallback, this);
    }

    Context::~Context() {
    }

    void Context::present() {
        glfwSwapInterval(m_VSyncEnabled ? 1 : 0);
        glfwSwapBuffers(m_Window->getHandle());
    }

    void Context::setVSyncEnabled(bool enabled) {
        m_VSyncEnabled = enabled;
    }

    void makeContextCurrent(const Context* context) {
        glfwMakeContextCurrent(context->getWindow()->getHandle());
    }

} // namespace glass::gfx