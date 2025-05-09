#include "Application.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#else
// for dpi
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// imgui
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Utils.h"

#define SCALE_RES(res, scale) static_cast<int>((float)res * scale);

#define RES_SHADOWMAP 1024
#define RES_SSAO      1024

#ifdef __EMSCRIPTEN__
#define POWER_SHADING   0.3f;
#define POWER_SHADOW    0.2f;
#define POWER_OCCLUSION 1.9f;
#else
#define POWER_SHADING   0.15f;
#define POWER_SHADOW    0.2f;
#define POWER_OCCLUSION 1.5f;
#endif

#ifdef __EMSCRIPTEN__
#define SSAO_RADIUS 0.04f;
#else
#define SSAO_RADIUS 0.06f;
#endif
#define SSAO_BIAS   0.04f;

inline glm::vec3 getCameraPos(float angle, float radius, float height) {
    return { cos(glm::radians(angle)) * radius, height, sin(glm::radians(angle)) * radius };
}

void Application::keyCallback(int key, int action) {
    if (action == GLFW_PRESS) {
        bool u = moveInputs[0] == key;
        bool r = moveInputs[1] == key;
        bool d = moveInputs[2] == key;
        bool l = moveInputs[3] == key;

        if (u || r || d || l) {
            if (u) game.goUp();
            else if (r) game.goLeft();
            else if (d) game.goDown();
            else game.goRight();
                
            if (game.getScore() > saveData->bestScore)
                saveData->bestScore = game.getScore();

            view.updateBoard();
            saveStorage->save(*saveData);
        }

        if (key == GLFW_KEY_D) {
            cameraAngleOffset -= 90.0f;

            int first = moveInputs[0];
            for (int i = 0; i < 3; ++i)
                moveInputs[i] = moveInputs[i + 1];
            moveInputs[3] = first;
        }
        else if (key == GLFW_KEY_A) {
            cameraAngleOffset += 90.0f;

            int last = moveInputs[3];
            for (int i = 3; i > 0; --i)
                moveInputs[i] = moveInputs[i - 1];
            moveInputs[0] = last;
        }
    }
}

void Application::mouseCallback(int button, int action) {
    double posX, posY;
    glfwGetCursorPos(window, &posX, &posY);

    glm::vec2 position = glm::vec2(posX, posY);

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            swipeDetector.onMouseDown(position);
        }
        else if (action == GLFW_RELEASE) {
            swipeDetector.onMouseUp(position);
        }
    }

    ui.mouseCallback(button, action, position);
}

void Application::restartGame() {
    game.reset();
    view.updateBoardFast();
    saveStorage->save(*saveData);
}

void Application::undoMove() {
    game.undoMove();
    view.updateBoardFast();
    saveStorage->save(*saveData);
}

void keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->keyCallback(key, action);
}

void mouseCallbackWrapper(GLFWwindow* window, int button, int action, int modds) {
    auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->mouseCallback(button, action);
}

void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam) {
    std::cerr << "GL DEBUG: " << message << "\n";
}

float getDPR() {
    float dpr = -1;
#ifdef __EMSCRIPTEN__
    dpr = emscripten_run_script_int("window.devicePixelRatio");
#else
    HDC screen = GetDC(0);
    dpr = GetDeviceCaps(screen, LOGPIXELSX) / 96;
    ReleaseDC(0, screen);
#endif
    return dpr;
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
    positionPass(_scene, camera)
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

    dpr = getDPR();

    if (dpr < 1.01f) {
        ssaoScale = 2.0f;
        shadowScale = 2.0f;
        lightScale = 2.0f;
    }
    else if (dpr > 2.99f) {
        ssaoScale = 1.0f;
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

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 100");
#else
    glfwSwapInterval(0);
#endif

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallbackWrapper);
    glfwSetMouseButtonCallback(window, mouseCallbackWrapper);

    ssaoRadius = SSAO_RADIUS;
    ssaoBias = SSAO_BIAS;
    shadingPower = POWER_SHADING;
    shadowPower = POWER_SHADOW;
    ssaoPower = POWER_OCCLUSION;

    saveStorage->load();

    float uiScale = dpr > 1.01f ? dpr * .75f : 1.0f; // Scale 1.0 looks good on dpr == 1, but too large on dpr == 3
    ui.initialize(canvasW, canvasH, uiScale);

    TextDescription scoreDesc;
    scoreDesc.position = glm::vec2(canvasW / 2, canvasH / 2 + 350);
    scoreDesc.alignmentX = 0.5f;
    scoreText = ui.createText(scoreDesc);
    
    scoreDesc.position = glm::vec2(canvasW / 2, canvasH / 2 + 350 + 50 * uiScale);
    scoreDesc.scale = 0.8f;
    scoreDesc.color = glm::vec3(1) * .8f;
    bestScoreText = ui.createText(scoreDesc);

    ImageDescription restartButtonDesc;
    restartButtonDesc.scale = .75f;
    restartButtonDesc.alignmentY = 1.0f;
    restartButtonDesc.position = glm::vec2(5, canvasH - 5);
    ImageId restartButtonImage = ui.createImage(restartButtonDesc, "textures/restart-icon.png");

    ImageDescription undoButtonDesc;
    undoButtonDesc.scale = .75f;
    undoButtonDesc.alignmentY = 1.0f;
    undoButtonDesc.alignmentX = 1.0f;
    undoButtonDesc.position = glm::vec2(canvasW - 5, canvasH - 5);
    ImageId undoButtonImage = ui.createImage(undoButtonDesc, "textures/restart-icon.png");

    ui.createButton(restartButtonImage, [this]() { restartGame(); });
    ui.createButton(undoButtonImage, [this]() { undoMove(); });

#ifdef ENABLE_ONSCREEN_LOG
    int logCounter = 0;
    int logShift = 35;
    int logPad = 35;
    
    TextDescription logDesc;
    logDesc.scale = .5f;
    
    logDesc.position = glm::vec2(25.0f, canvasH - (logCounter++) * logShift - logPad);
    fpsText = ui.createText(logDesc);
    
    if (dpr != -1) {
        logDesc.position = glm::vec2(25.0f, canvasH - (logCounter++) * logShift - logPad);
        dprText = ui.createText(logDesc);
    }

    logDesc.position = glm::vec2(25.0f, canvasH - (logCounter++) * logShift - logPad);
    resText = ui.createText(logDesc);

    logDesc.position = glm::vec2(25.0f, canvasH - (logCounter++) * logShift - logPad);
    shadowText = ui.createText(logDesc);

    logDesc.position = glm::vec2(25.0f, canvasH - (logCounter++) * logShift - logPad);
    gPositionText = ui.createText(logDesc);

    logDesc.position = glm::vec2(25.0f, canvasH - (logCounter++) * logShift - logPad);
    lightingText = ui.createText(logDesc);
#endif

    return true;
}

void Application::initGame() {
    cameraOffset = glm::vec3(1.6, 0, 1.6);
    camera.target = cameraOffset;
    camera.up = glm::vec3(0, 1, 0);
    camera.fovY = 11;
    camera.aspect = (float)canvasW / (float)canvasH;
    camera.nearZ = .01f;
    camera.farZ = 50.0f;
    cameraAngle = cameraStartAngle;

    glm::vec3 lightCamOffset = cameraOffset + glm::vec3(0, 1, 0) * 0.9f;
    glm::vec3 lightDir = glm::vec3(-0.32f, -0.77f, 0.56);
    light.target = lightCamOffset;
    light.up = glm::vec3(0, 1, 0);
    light.position = lightCamOffset - lightDir * 3.0f;
    light.fovY = 6;
    light.aspect = 1.0f;
    light.nearZ = .01f;
    light.farZ = 10.0f;

    game.reset();
    view.updateBoardFast();

    // Initialize Rendering
    int ssaoResScaled = SCALE_RES(RES_SSAO, ssaoScale);
    int shadowResScaled = SCALE_RES(RES_SHADOWMAP, shadowScale);

    shadowPass.initialize(shadowResScaled, shadowResScaled);
    positionPass.initialize(ssaoResScaled, ssaoResScaled);

    GLuint lightingPassInput[3] = {
        shadowPass.getTexture(), 
        (GLuint)shadowResScaled, 
        positionPass.getTexture()
    };
    
    int ssaaWidth = canvasW * lightScale;
    int ssaaHeight = canvasH * lightScale;

    lightingPass.initialize(ssaaWidth, ssaaHeight, static_cast<const void*>(lightingPassInput));

    fullscreenQuadMesh = Mesh::GenFullscreenQuad();
    fullscreenQuadShader = Shader::Load(
        "shaders/fullscreen-quad-vs.glsl",
        "shaders/fullscreen-quad-fs.glsl",
        {"aPos", "aTexCoord"});
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
    if (saveStorage->checkLoaded()) {
        game.setHistory(saveStorage->getHistoryTree(), saveStorage->getHistoryPointer());
        saveData = new SaveData(saveStorage->getBestScore(), game.getHistoryPointer(), game.getHistoryTree());
        saveStorage->unload();
        view.updateBoardFast();
    }

    cameraAngle = Utils::lerp(cameraAngle, cameraStartAngle + cameraAngleOffset, dt * 15.0f);
    camera.position = cameraOffset + getCameraPos(cameraAngle, cameraRadius, cameraHeight);
    view.update(dt);
    ui.getText(scoreText).value = std::to_string(game.getScore());
    ui.getText(bestScoreText).value = std::to_string(saveData->bestScore);
    
#ifdef ENABLE_ONSCREEN_LOG
    ui.getText(fpsText).value = "fps: " + std::to_string(fps);
    ui.getText(resText).value = "res: " + std::to_string(canvasW) + "x" + std::to_string(canvasH);
    ui.getText(shadowText).value = "shadow: " + std::to_string(shadowPass.getWidth()) + "x" + std::to_string(shadowPass.getHeight()) + " (" + std::to_string(shadowScale) + ")";
    ui.getText(gPositionText).value = "gPosition: " + std::to_string(positionPass.getWidth()) + "x" + std::to_string(positionPass.getHeight()) + " (" + std::to_string(ssaoScale) + ")";
    ui.getText(lightingText).value = "lighting: " + std::to_string(lightingPass.getWidth()) + "x" + std::to_string(lightingPass.getHeight()) + " (" + std::to_string(lightScale) + ")";
    if (dpr != -1)
        ui.getText(dprText).value = "dpr: " + std::to_string(dpr);
#endif

    if (swipeDetector.checkSwipe()) {
        const Swipe& swipe = swipeDetector.getSwipe();
        if (swipe.getLength() >= 10.0f && swipe.time >= 0.01f && swipe.time <= 1.0f) {
            glm::vec2 direction = swipe.getDirection();

            glm::vec4 dir = glm::vec4(direction.x, direction.y, 0.0f, 0.0f);
            glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(cameraAngleOffset), glm::vec3(0, 0, 1));
            dir = rotation * dir;
            direction.x = dir.x;
            direction.y = dir.y;

            float inputs[4] = {
                glm::dot(direction, glm::vec2(0.0f, -1.0f)),
                glm::dot(direction, glm::vec2(1.0f, 0.0f)),
                glm::dot(direction, glm::vec2(0.0f, 1.0f)),
                glm::dot(direction, glm::vec2(-1.0f, 0.0f))
            };

            int indexMax = 0;
            if (inputs[1] > inputs[indexMax]) indexMax = 1;
            if (inputs[2] > inputs[indexMax]) indexMax = 2;
            if (inputs[3] > inputs[indexMax]) indexMax = 3;

            bool u = indexMax == 0;
            bool r = indexMax == 1;
            bool d = indexMax == 2;
            bool l = indexMax == 3;

            if (u) game.goUp();
            else if (r) game.goLeft();
            else if (d) game.goDown();
            else game.goRight();

            if (game.getScore() > saveData->bestScore)
                saveData->bestScore = game.getScore();

            view.updateBoard();
            saveStorage->save(*saveData);
        }
    }

    // Render
    float lightingParams[5] = { ssaoRadius, ssaoBias, shadingPower, shadowPower, ssaoPower };
    shadowPass.render();
    positionPass.render();
    lightingPass.render(lightingParams);

    // Drawing result
    glViewport(0, 0, canvasW, canvasH);
    fullscreenQuadShader.use();
    fullscreenQuadMesh.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, lightingPass.getTexture());
    fullscreenQuadShader.setUniform1i("source", 0);
    glDrawArrays(GL_TRIANGLES, 0, fullscreenQuadMesh.vertexCount);

    ui.render();

    // ImGui
#ifdef ENABLE_IMGUI
#ifndef __EMSCRIPTEN__
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("SSAO");
    ImGui::DragFloat("Power", &ssaoPower, 0.01f, 0.0f, 2.0f);
    ImGui::DragFloat("Radius", &ssaoRadius, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Bias", &ssaoBias, 0.01f, 0.0f, 1.0f);
    if (ImGui::Button("Reset")) {
        ssaoPower = POWER_OCCLUSION;
        ssaoRadius = SSAO_RADIUS;
        ssaoBias = SSAO_BIAS;
    }
    ImGui::End();

    ImGui::Begin("Shading");
    ImGui::DragFloat("Power", &shadingPower, 0.01f, 0.0f, 2.0f);
    if (ImGui::Button("Reset")) {
        shadingPower = POWER_SHADING;
    }
    ImGui::End();

    ImGui::Begin("Shadow");
    ImGui::DragFloat("Power", &shadowPower, 0.01f, 0.0f, 2.0f);
    if (ImGui::Button("Reset")) {
        shadowPower = POWER_SHADOW;
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
#endif

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Application::terminate() {
    saveStorage.reset();
    glfwTerminate();
}