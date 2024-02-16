#include "glass/glass.h"
#include "assert.h"
#include "print"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace gp = glass::platform;
namespace gfx = glass::gfx;

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
};

static float yaw{-90.0f};
static float pitch{};

static glm::vec3 camPos{ 0.0f, 0.0f, 5.0f };
static float camSensitivity{ 10.0f };
static bool isControllingCamera = false;
static glm::vec3 cameraDirection{};
static float cameraMoveSpeed = 10.0f;

static constexpr float lerp(float a, float b, float alpha) {
    return a * (1.0f - alpha) + b * alpha;
}

void controlCameraPos() {
    const glm::vec3 up{ 0.0f, 1.0f, 0.0f };
    const glm::vec3 forward = glm::normalize(cameraDirection);
    const glm::vec3 right = glm::cross(forward, up);

    if (gp::isKeyDown(gp::EKeyCode::W)) {
        camPos -= forward * cameraMoveSpeed * (float)gp::getDeltaTime();
    } 

    if (gp::isKeyDown(gp::EKeyCode::S)) {
        camPos += forward * cameraMoveSpeed * (float)gp::getDeltaTime();
    }

    if (gp::isKeyDown(gp::EKeyCode::A)) {
        camPos += right * cameraMoveSpeed * (float)gp::getDeltaTime();
    }

    if (gp::isKeyDown(gp::EKeyCode::D)) {
        camPos -= right * cameraMoveSpeed * (float)gp::getDeltaTime();
    }

    if (gp::isKeyDown(gp::EKeyCode::E)) {
        camPos -= up * cameraMoveSpeed * (float)gp::getDeltaTime();
    }

    if (gp::isKeyDown(gp::EKeyCode::Q)) {
        camPos += up * cameraMoveSpeed * (float)gp::getDeltaTime();
    }
}

int main() {
    assert(gp::init());
    gp::WindowSpec spec{};

    spec.Size = { 1280, 720 };
    spec.Title = "Hello, Glass";
    spec.Resizable = true;
    spec.Fullscreen = false;
    spec.EventCallback = [](const gp::WindowEvent& event) {
        gp::EventDispatcher dispatcher{ event };
        dispatcher.dispatch(+[](const gp::MouseButtonPressEvent& event) {
            if (event.Button == gp::EMouseButton::Right) {
                gp::disableCursor(event.EventWindow);
                isControllingCamera = true;
            }
        });
        dispatcher.dispatch(+[](const gp::MouseButtonReleaseEvent& event) {
            if (event.Button == gp::EMouseButton::Right) {
                gp::enableCursor(event.EventWindow);
                isControllingCamera = false;
            }
        });
        dispatcher.dispatch(+[](const gp::MouseMoveEvent& event) {
            if (isControllingCamera) {
                yaw += static_cast<float>(gp::getMouseOffset().X * camSensitivity * gp::getDeltaTime());
                pitch -= static_cast<float>(gp::getMouseOffset().Y * camSensitivity * gp::getDeltaTime());

                if (pitch > 89.0f)
                    pitch = 89.0f;
                if (pitch < -89.0f)
                    pitch = -89.0f;
            }
        });

        dispatcher.dispatch(+[](const gp::KeyPressEvent& event) {
            if (event.KeyCode == gp::EKeyCode::Escape) {
                gp::requestExit();
            }
        });
    };

    gp::Window* window = gp::createWindow(spec);
    gfx::Context* context = gfx::createContext({ window, true });

    gfx::ResourceID vbo{};
    {
        Vertex vertices[] = {
            // Front
            { { -0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f } },
            { { -0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f } },
            { { +0.5f, -0.5f, +0.5f }, { 1.0f, 0.0f } },
            { { +0.5f, +0.5f, +0.5f }, { 1.0f, 1.0f } },

            // Left
            { { -0.5f, +0.5f, -0.5f }, { 0.0f, 1.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
            { { -0.5f, -0.5f, +0.5f }, { 1.0f, 0.0f } },
            { { -0.5f, +0.5f, +0.5f }, { 1.0f, 1.0f } },

            // Right
            { { +0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f } },
            { { +0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f } },
            { { +0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
            { { +0.5f, +0.5f, -0.5f }, { 1.0f, 1.0f } },

            // Back
            { { +0.5f, +0.5f, -0.5f }, { 0.0f, 1.0f } },
            { { +0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
            { { -0.5f, +0.5f, -0.5f }, { 1.0f, 1.0f } },

            // Top
            { { -0.5f, +0.5f, -0.5f }, { 0.0f, 1.0f } },
            { { -0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f } },
            { { +0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f } },
            { { +0.5f, +0.5f, -0.5f }, { 1.0f, 1.0f } },

            // Bottom
            { { -0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
            { { +0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
            { { +0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f } },
        };

        gfx::BufferInputLayout layout{};
        layout
            .add(gfx::EVT_Float, 3)
            .add(gfx::EVT_Float, 2);

        vbo = gfx::createStaticVertexBuffer(vertices, &layout);
    }

    // clang-format off
    uint32_t indices[] {
        // Front
        0, 1, 3, 1, 2, 3,

        // Left
        4, 5, 7, 5, 6, 7,

        // Right
        8, 9, 11, 9, 10, 11,

        // Back
        12 ,13, 15, 13, 14, 15,

        // Top
        16, 17, 19, 17, 18, 19,

        // Bottom
        20, 21, 23, 21, 22, 23
    };
    // clang-format on

    gfx::ResourceID ibo = gfx::createStaticElementBuffer(indices);

    gfx::ProgramSpec programSpec{
        .VertexShader = gfx::getOrCreateShader("shaders/test.vert", gfx::EST_VertexShader),
        .FragmentShader = gfx::getOrCreateShader("shaders/test.frag", gfx::EST_FragmentShader)
    };
    gfx::ShaderProgram* program = gfx::getOrCreateShaderProgram(programSpec);

    int w, h, bpp;
    const stbi_uc* texData = stbi_load("res/container.jpg", &w, &h, &bpp, 4);

    gfx::TextureSpec textureSpec{};
    textureSpec.Width = w;
    textureSpec.Height = h;
    textureSpec.Type = gfx::ETT_Texture2D;
    textureSpec.Format = gfx::EPF_RGBA8;
    textureSpec.InitialData = texData;

    gfx::ResourceID texture = gfx::createTexture(textureSpec);

    struct Matrices {
        glm::mat4 ViewProjection{ 1 };
        glm::mat4 Model{ 1 };
    };

    gfx::ResourceID uniformBuffer = gfx::createUniformBuffer(Matrices{});

    while (gp::pollEvents()) {
        glm::vec4 clearColor{ 0.2f, 0.31f, 0.12f, 1.0f };
        gfx::clearViewport(gfx::ECF_All, &clearColor);
        gfx::setViewport();

        gfx::bindVertexBuffer(vbo);
        gfx::bindElementBuffer(ibo);

        gfx::bindShaderProgram(program);
        gfx::setUniformTexture(program, "uTexture", texture);

        // Set projection matrix
        const gp::WindowSize size = gp::getWindowSize(window);
        const glm::mat4 projection = glm::perspectiveFov(glm::radians(45.0f), glm::max(1.0f, (float)size.Width), glm::max(1.0f, (float)size.Height), 0.1f, 100.0f);

        /**
         * CAMERA
         */
        cameraDirection.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
        cameraDirection.y = glm::sin(glm::radians(pitch));
        cameraDirection.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
        glm::vec3 cameraFront = glm::normalize(cameraDirection);
        controlCameraPos();

        const glm::mat4 view = glm::lookAt(camPos, camPos + cameraFront, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)gp::getTime(), glm::vec3(1.0f, 1.0f, 1.0f));

        Matrices matrices{
            .ViewProjection = projection * view,
            .Model = model
        };
        writeBufferData(uniformBuffer, &matrices, sizeof(matrices));
        gfx::setUniformBuffer(program, "Matrices", uniformBuffer);
        // gfx::setUniform(program, "uViewProjection", projection * view);
        // gfx::setUniform(program, "uModel", model);

        gfx::enableDepthTest();
        gfx::setFillMode(gfx::EFM_Solid);
        gfx::setCullMode(gfx::ECM_Back);

        gfx::drawElements(gfx::EPT_Triangles, (uint32_t)std::size(indices));
        gfx::present(context);
    }

    gp::shutdown();
}