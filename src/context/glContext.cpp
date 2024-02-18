#include "glad/glad.h"

#include "glContext.h"
#include "print"

#include "cassert"
#include "ranges"
#include "glShader.h"
#include "glFrameBuffer.h"

namespace glass::gfx {
    const Context* GCurrentContext = nullptr;
    bool GUsesGFX = false;

    static void glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
        #ifndef GLASS_ENABLE_VERBOSE_OPENGL_LOGGING
        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
            return;
        }
        #endif
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
            if (!context) {
                return;
            }

            auto window = context->getWindow();
            if (Contexts.contains(window)) {
                Contexts.erase(window);
            }
        }
    };

    static std::unique_ptr<ContextData> GContextData{};

    void init() {
        GContextData = std::make_unique<ContextData>();
        GUsesGFX = true;
    }

    void shutdown() {
        GContextData.reset();
        freeFramebufferRegistry();
        terminateShaderLibrary();
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

    Context* getCurrentContext() {
        return const_cast<Context*>(GCurrentContext);
    }


    platform::Window* getContextWindow(const Context* context) {
        return context->getWindow();
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

    void clearViewport(EClearFlagsMask clearFlags, const glm::vec4* clearColor) {
        if (clearColor) {
            glClearColor(clearColor->r, clearColor->g, clearColor->b, clearColor->a);
        }

        GLuint flags{};
        if (clearFlags & ECF_Color) {
            flags |= GL_COLOR_BUFFER_BIT;
        }
        if (clearFlags & ECF_Depth) {
            flags |= GL_DEPTH_BUFFER_BIT;
        }
        if (clearFlags & ECF_Stencil) {
            flags |= GL_STENCIL_BUFFER_BIT;
        }

        glClear(flags);
    }

    void setViewport(const Viewport2D& viewport) {
        if (viewport.Width > 0 && viewport.Height > 0) {
            glViewport(viewport.X, viewport.Y, viewport.Width, viewport.Height);
            return;
        } else {
            if (!GCurrentContext) {
                return;
            }

            const platform::WindowSize windowSize = platform::getWindowSize(GCurrentContext->getWindow());
            Viewport2D vp = { viewport.X, viewport.Y, windowSize.Width, windowSize.Height };
            glViewport(vp.X, vp.Y, windowSize.Width, windowSize.Height);
        }
    }

    void Context::present() {
        glfwSwapInterval(m_VSyncEnabled ? 1 : 0);
        glfwSwapBuffers(m_Window->getHandle());
    }

    void Context::setVSyncEnabled(bool enabled) {
        m_VSyncEnabled = enabled;
    }

    void Context::bindFrameBuffer(FrameBuffer* frameBuffer, bool updateViewport) const {
        if (m_BoundFrameBuffer != frameBuffer) {
            m_BoundFrameBuffer = frameBuffer;

            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer ? frameBuffer->getId() : 0);

            if (updateViewport) {
                if (frameBuffer) {
                    Viewport2D vp{};
                    vp.Width = frameBuffer->getWidth();
                    vp.Height = frameBuffer->getHeight();
                    setViewport(vp);
                } else {
                    setViewport();
                }
            }
        }
    }

    void makeContextCurrent(const Context* context) {
        glfwMakeContextCurrent(context->getWindow()->getHandle());
        GCurrentContext = context;
    }

    // Depth test
    static bool GDepthTestEnabled = false;
    void enableDepthTest() {
        if (!GDepthTestEnabled) {
            glEnable(GL_DEPTH_TEST);
            GDepthTestEnabled = true;
        }
    }

    void disableDepthTest() {
        if (GDepthTestEnabled) {
            glDisable(GL_DEPTH_TEST);
            GDepthTestEnabled = false;
        }
    }

    // Poligon winding
    static EPoligonWinding GPoligonWinding = EPW_CounterClockwise;
    void setPoligonWinding(EPoligonWinding winding) {
        if (GPoligonWinding != winding) {
            glFrontFace(winding == EPW_Clockwise ? GL_CW : GL_CCW);
            GPoligonWinding = winding;
        }
    }

    // Culling
    static ECullMode GCullMode{ ECM_None };
    GLASS_API void setCullMode(ECullMode mode) {
        if (GCullMode == mode) {
            return;
        }

        if (mode == ECM_None) {
            glDisable(GL_CULL_FACE);
        } else {
            glEnable(GL_CULL_FACE);

            GLenum face{};
            switch (mode) {
                case ECM_Back:
                    face = GL_BACK;
                    break;
                case ECM_Front:
                    face = GL_FRONT;
                    break;
                case ECM_BackAndFront:
                    face = GL_FRONT_AND_BACK;
                    break;
            }

            glCullFace(face);
        }

        GCullMode = mode;
    }

    // Fill mode
    static EFillMode GFillMode = EFM_Solid;
    void setFillMode(EFillMode mode) {
        if (GFillMode == mode) {
            return;
        }

        glPolygonMode(GL_FRONT_AND_BACK, mode == EFM_Solid ? GL_FILL : GL_LINE);
        GFillMode = mode;
    }

    void draw(EPrimitiveTopology topology, uint32_t vertexCount, uint32_t firstVertex) {
        GLenum glTop{};
        switch (topology) {
            case EPT_Triangles:
                glTop = GL_TRIANGLES;
                break;
            case EPT_Lines:
                glTop = GL_LINES;
                break;
            case EPT_Points:
                glTop = GL_POINTS;
                break;
        }

        glDrawArrays(glTop, static_cast<GLint>(firstVertex), static_cast<GLsizei>(vertexCount));
    }

    void drawElements(EPrimitiveTopology topology, uint32_t indexCount, EIndexType indexType) {
        GLenum glTop{};
        switch (topology) {
            case EPT_Triangles:
                glTop = GL_TRIANGLES;
                break;
            case EPT_Lines:
                glTop = GL_LINES;
                break;
            case EPT_Points:
                glTop = GL_POINTS;
                break;
        }

        GLenum glIndexType{};
        switch (indexType) {
            case EIT_UInt16:
                glIndexType = GL_UNSIGNED_SHORT;
                break;
            case EIT_UInt32:
                glIndexType = GL_UNSIGNED_INT;
                break;
            default:
                break;
        }

        glDrawElements(glTop, static_cast<GLsizei>(indexCount), glIndexType, nullptr);
    }

    void setFrameBuffer(FrameBuffer* frameBuffer, bool updateViewport) {
        if (GCurrentContext) {
            GCurrentContext->bindFrameBuffer(frameBuffer, updateViewport);
        }
    }

} // namespace glass::gfx