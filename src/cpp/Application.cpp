#include "Application.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#else
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define RES_LIGHTING  1024
#define RES_SHADOWMAP 1024
#define RES_SSAO      1024
#define SCALE_RES(res, scale) static_cast<int>((float)res * scale);

inline glm::vec3 getCameraPos(float angle, float radius, float height) {
    return { cos(glm::radians(angle)) * radius, height, sin(glm::radians(angle)) * radius };
}

void Application::keyCallback(int key, int action) {
    if (action == GLFW_PRESS) {
        bool u = GLFW_KEY_UP == key;
        bool r = GLFW_KEY_RIGHT == key;
        bool d = GLFW_KEY_DOWN == key;
        bool l = GLFW_KEY_LEFT == key;

        if (u || r || d || l) {
            if (u) game.goUp();
            else if (r) game.goLeft();
            else if (d) game.goDown();
            else game.goRight();

            if (game.boardChanged()) {
                game.addRandom();
                view.updateBoard();
            }
        }
    }
}

void keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->keyCallback(key, action);
}

void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam) {
    std::cerr << "GL DEBUG: " << message << "\n";
}

float getDPI() {
    float dpi = 0;

#ifndef __EMSCRIPTEN__
    HDC screen = GetDC(0);
    dpi = GetDeviceCaps(screen, LOGPIXELSX);
    ReleaseDC(0, screen);
#else
    dpi = emscripten_run_script_int("window.devicePixelRatio * 96.0");
#endif
    return dpi;
}

float getDevicePixelRatio() {
    float ratio = -1;
#ifdef __EMSCRIPTEN__
    ratio = emscripten_run_script_int("window.devicePixelRatio");
#endif
    return ratio;
}

Application::Application(
    const Resources& _resources,
    Scene& _scene,
    Game2048& _game,
    View2048& _view) :
    resources(_resources),
    scene(_scene),
    game(_game),
    view(_view),
    lightingPass(_scene, camera, light),
    shadowPass( _scene, light),
    positionPass(_scene, camera),
    normalPass(_scene, camera),
    ssaoPass(camera)
{}

bool Application::initialize() {
#ifdef __EMSCRIPTEN__
    emscripten_get_canvas_element_size("#canvas", &canvasW, &canvasH);
#endif

    glfwInit();
    //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(canvasW, canvasH, "City 2048", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    dpr = getDevicePixelRatio();

    if (dpr < 1.01f) {
        ssaoScale = 2.0f;
        shadowScale = 2.0f;
        lightScale = 2.0f;
    }
    else if (dpr > 2.99f) {
        ssaoScale = 1.5f;
        shadowScale = 1.0f;
        lightScale = 1.0f;
    }

#ifndef __EMSCRIPTEN__
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return false;
    }

    //glEnable(GL_DEBUG_OUTPUT);
    //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //glDebugMessageCallback(debugCallback, nullptr);
#else
    glfwSwapInterval(0);
#endif

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallbackWrapper);

    return true;
}

void Application::initGame() {
    cameraOffset = glm::vec3(1.6, 0, 1.6);
    camera.target = cameraOffset;
    camera.up = glm::vec3(0, 1, 0);
    camera.fovY = 11;
    camera.aspect = (float)canvasW / (float)canvasH;
    camera.nearZ = .01f;
    camera.farZ = 100.0f;

    glm::vec3 lightCamOffset = cameraOffset + glm::vec3(0, 1, 0) * 0.9f;
    glm::vec3 lightDir = glm::vec3(-0.32f, -0.77f, 0.56);
    light.target = lightCamOffset;
    light.up = glm::vec3(0, 1, 0);
    light.position = lightCamOffset - lightDir * 3.0f;
    light.fovY = 6;
    light.aspect = 1.0f;
    light.nearZ = .01f;
    light.farZ = 10.0f;

    std::array<std::array<int, 4>, 4> start = { {
        {0, 0, 0, 0},
        {0, 0, 2, 0},
        {0, 11, 0, 0},
        {0, 0, 0, 0}
    } };
    game.setBoard(start);
    game.addRandom();
    view.updateBoardFast();

    // Initialize Rendering
    int ssaoResScaled = SCALE_RES(RES_SSAO, ssaoScale);
    int shadowResScaled = SCALE_RES(RES_SHADOWMAP, shadowScale);
    int lightResScaled = SCALE_RES(RES_LIGHTING, lightScale);

    positionPass.initialize(ssaoResScaled, ssaoResScaled);
    normalPass.initialize(ssaoResScaled, ssaoResScaled);
    GLuint gBuffer[2] = { positionPass.getTexture(), normalPass.getTexture() };
    ssaoPass.initialize(ssaoResScaled, ssaoResScaled, static_cast<const void*>(gBuffer));

    shadowPass.initialize(shadowResScaled, shadowResScaled);
    GLuint shadowMap[2] = { shadowPass.getTexture(), (GLuint)shadowResScaled };

    lightingPass.initialize(lightResScaled, lightResScaled, static_cast<const void*>(shadowMap));

    fullscreenQuadMesh = Mesh::GenFullscreenQuad();
    fullscreenQuadShader = Shader::Load(
        "shaders/fullscreen-quad-vs.glsl",
        "shaders/fullscreen-quad-fs.glsl");

    textRenderer.initialize(canvasW, canvasH);
}

bool Application::isRunning() {
    return !glfwWindowShouldClose(window);
}

void Application::mainLoop() {
    float time = static_cast<float>(glfwGetTime());
    static float lastTime = time;
    float dt = time - lastTime;
    lastTime = time;
    int fps = static_cast<int>(1.0f / dt);

    // Game logic
    float cameraDirection = 0;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraDirection = -1;
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraDirection = 1;

    float angleDelta = cameraDirection * cameraSpeed * dt;
    cameraAngle += cameraDirection * cameraSpeed * dt;
    camera.position = cameraOffset + getCameraPos(cameraAngle, cameraRadius, cameraHeight);

    view.update(dt);

    // 1) Maps
    // [ COMPLETE ] shadow pass -> shadowmap (light camera depth)
    // [ COMPLETE ] position pass -> position map
    // [ COMPLETE ] normal pass -> normal map
    // 
    // 2) SSAO
    // [ COMPLETE ] ssao pass (position map, normal map) -> occlusion map
    // [ TODO ]     blur ssao pass (occlusion map) -> blurred occlusion map
    // 
    // 3) Main
    // [ COMPLETE ] lighting pass (shadowMap) -> result
    // [ COMPLETE ] post processing (result, blurred occlusion map) -> draw

    // Update Render Passes
    shadowPass.render();
    lightingPass.render();
    positionPass.render();
    normalPass.render();
    ssaoPass.render();

    // Drawing result
    glViewport(0, 0, canvasW, canvasH);
    fullscreenQuadShader.use();
    fullscreenQuadMesh.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, lightingPass.getTexture());
    fullscreenQuadShader.setUniform1i("source", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ssaoPass.getTexture());
    fullscreenQuadShader.setUniform1i("ssaoMap", 1);

    fullscreenQuadShader.setUniform1f("ssaoPower", 0.8f);

    glDrawArrays(GL_TRIANGLES, 0, fullscreenQuadMesh.vertexCount);
    // =============== //

    // Drawing text
    int logCounter = 0;
    textRenderer.draw("fps: " + std::to_string(fps), 25.0f, canvasH - (logCounter++) * 50.0f - 50.0f, 1.0f, glm::vec3(0, 1, 0));
    textRenderer.draw("res: " + std::to_string(canvasW) + "x" + std::to_string(canvasH), 25.0f, canvasH - (logCounter++) * 50.0f - 50.0f, 1.0f, glm::vec3(0, 1, 0));
    if (dpr != -1)
        textRenderer.draw("dpr: " + std::to_string(dpr), 25.0f, canvasH - (logCounter++) * 50.0f - 50.0f, 1.0f, glm::vec3(0, 1, 0));
    textRenderer.draw("ssaoScale: " + std::to_string(ssaoScale), 25.0f, canvasH - (logCounter++) * 50.0f - 50.0f, 1.0f, glm::vec3(0, 1, 0));
    textRenderer.draw("shadowScale: " + std::to_string(shadowScale), 25.0f, canvasH - (logCounter++) * 50.0f - 50.0f, 1.0f, glm::vec3(0, 1, 0));
    textRenderer.draw("lightScale: " + std::to_string(lightScale), 25.0f, canvasH - (logCounter++) * 50.0f - 50.0f, 1.0f, glm::vec3(0, 1, 0));
    // =============== //

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Application::terminate() {
    glfwTerminate();
}