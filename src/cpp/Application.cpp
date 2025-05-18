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
#include "JS.h"

#define SCALE_RES(res, scale) static_cast<int>((float)res * scale);

#define RES_SHADOWMAP 1024
#define RES_SSAO      1024
#define INTER_COOLDOWN 1.5f * 60.0f
#define NO_ADS_ID "no_ads"

#ifdef __EMSCRIPTEN__
#define POWER_SHADING   0.3f;
#define POWER_SHADOW    0.2f;
#define POWER_OCCLUSION 1.5f;
#else
#define POWER_SHADING   0.15f;
#define POWER_SHADOW    0.2f;
#define POWER_OCCLUSION 1.5f;
#endif

#ifdef __EMSCRIPTEN__
#define SSAO_RADIUS 0.045f;
#else
#define SSAO_RADIUS 0.06f;
#endif
#define SSAO_BIAS   0.04f;

inline glm::vec3 getCameraPos(float angle, float radius, float height) {
    return { cos(glm::radians(angle)) * radius, height, sin(glm::radians(angle)) * radius };
}

void buyNoAds() {
    js::purchase(NO_ADS_ID);
}

void Application::onUndoButtonClicked() {
    if (purchasesUpdated && !js::hasPurchase(NO_ADS_ID)) {
        js::showRewardedVideo();
    }
    else {
        undoMove();
    }
}

void Application::undoMove() {
    game.undoMove();
    view.updateBoardFast();
    saveStorage->save(*saveData);
}

void Application::go(MoveDirection direction) {
    bool boardChanged = false;
    switch (direction) {
        case MoveDirection::Left: boardChanged = game.goLeft(); break;
        case MoveDirection::Up: boardChanged = game.goUp(); break;
        case MoveDirection::Right: boardChanged = game.goRight(); break;
        case MoveDirection::Down: boardChanged = game.goDown(); break;
    }

    if (!boardChanged) {
        audio.playStuck();
        return;
    }

    if (game.getScore() > saveData->bestScore) {
        saveData->bestScore = game.getScore();
        if (leaderboardTimer < .0f)
            leaderboardTimer = 1.f;
    }

    if (game.isGameOver()) {
        view.updateBoard([this]() { audio.playFail(); });
    }
    else if (maxLevel < game.getMaxLevel() && game.getMaxLevel() > 1) {
        maxLevel = game.getMaxLevel();
        view.updateBoard([this]() { audio.playReveal(); });
    }
    else {
        view.updateBoard();
    }

    if (saveTimer < .0f)
        saveTimer = 2.f;

    if (firstAdFlag && time >= lastInterTime + INTER_COOLDOWN) {
        if (tryShowInter())
            lastInterTime = time;
    }

    audio.playGo();
    ui.getText(tutorialText).active = false;
}

bool Application::tryShowInter() {
    bool showed = false;
    if (js::getPurchasesUpdateFlag() && !js::hasPurchase(NO_ADS_ID)) {
        js::showFullscreenAdv();
        showed = true;
    }
    return showed;
}

void Application::updateUiPositions(float time)
{
    float uiScale = (float)CANVAS_H / canvasH;

    ui.getText(scoreText).position = glm::vec2(canvasW * uiScale / 2, canvasH * uiScale - 200 * uiScale);
    ui.getText(bestScoreText).position = glm::vec2(canvasW * uiScale / 2, canvasH * uiScale - 200 * uiScale - 50);
    ui.getText(tutorialText).position = glm::vec2(canvasW * uiScale / 2, 250 * uiScale);
    ui.getText(tutorialText).scale = glm::vec2(Utils::lerp(0.9f, 1.0f, (glm::cos(time * 3.0f) + 1.0f) * 0.5f));
    ui.getImage(restartButtonImage).position = glm::vec2(5, canvasH * uiScale - 5);
    ui.getImage(undoButtonImage).position = glm::vec2(canvasW * uiScale - 5, canvasH * uiScale - 5);
    ui.getImage(noAdsButtonImage).position = glm::vec2(canvasW * uiScale - 15, canvasH * uiScale - 100);
    ui.getImage(rightArrowImage).position = glm::vec2(canvasW / 2 * uiScale + 50, 150 * uiScale);
    ui.getImage(leftArrowImage).position = glm::vec2(canvasW / 2 * uiScale - 50, 150 * uiScale);
    ui.getImage(audioUnlockerBgImage).scale = glm::vec2(canvasW * uiScale / 100, canvasH * uiScale / 100);
    ui.getImage(audioUnlockerBgImage).position = glm::vec2(canvasW * uiScale / 2, canvasH * uiScale / 2);
    ui.getImage(audioUnlockerPointerImage).scale = glm::vec2(Utils::lerp(0.5f, .75f, (glm::cos(time * 3.0f) + 1.0f) * 0.5f));
    ui.getImage(audioUnlockerPointerImage).position = glm::vec2(canvasW * uiScale / 2, canvasH * uiScale / 2);

    Image noAdsImage = ui.getImage(noAdsButtonImage);
    ui.getText(priceText).position = glm::vec2(
        canvasW * uiScale - 15 - noAdsImage.width * noAdsImage.scale.x / 2,
        canvasH * uiScale - 100 - noAdsImage.height * noAdsImage.scale.y - 20);

#ifdef ENABLE_ONSCREEN_LOG
    int logCounter = 0;
    int logShift = 35;
    int logPad = 35;

    ui.getText(fpsText).position = glm::vec2(25.0f, (logCounter++) * logShift + logPad);
    ui.getText(resText).position = glm::vec2(25.0f, (logCounter++) * logShift + logPad);
    if (dpr != -1) ui.getText(dprText).position = glm::vec2(25.0f, (logCounter++) * logShift + logPad);
    ui.getText(shadowText).position = glm::vec2(25.0f, (logCounter++) * logShift + logPad);
    ui.getText(gPositionText).position = glm::vec2(25.0f, (logCounter++) * logShift + logPad);
    ui.getText(lightingText).position = glm::vec2(25.0f, (logCounter++) * logShift + logPad);
#endif
}

void Application::moveCameraRight()
{
    cameraAngleOffset -= 90.0f;

    int first = moveInputs[0];
    for (int i = 0; i < 3; ++i)
        moveInputs[i] = moveInputs[i + 1];
    moveInputs[3] = first;
}

void Application::moveCameraLeft()
{
    cameraAngleOffset += 90.0f;

    int last = moveInputs[3];
    for (int i = 3; i > 0; --i)
        moveInputs[i] = moveInputs[i - 1];
    moveInputs[0] = last;
}

void Application::audioUnlockerButtonClicked()
{
    ui.getImage(audioUnlockerBgImage).active = false;
    ui.getImage(audioUnlockerPointerImage).active = false;
    ui.getText(tutorialText).active = true;
}

void Application::keyCallback(int key, int action) {
    if (action == GLFW_PRESS) {
        bool u = moveInputs[0] == key;
        bool l = moveInputs[1] == key;
        bool d = moveInputs[2] == key;
        bool r = moveInputs[3] == key;

        if (u || r || d || l) {
            MoveDirection direction =
                u ? MoveDirection::Up :
                r ? MoveDirection::Right :
                d ? MoveDirection::Down :
                MoveDirection::Left;
            go(direction);
        }

        if (key == GLFW_KEY_D) {
            moveCameraRight();
        }
        else if (key == GLFW_KEY_A) {
            moveCameraLeft();
        }

        if (ui.getImage(audioUnlockerBgImage).active) {
            audioUnlockerButtonClicked();
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

    float uiScale = (float)CANVAS_H / canvasH;
    position.y = canvasH - position.y;
    position.x = position.x * uiScale;
    position.y = position.y * uiScale;

    ui.mouseCallback(button, action, position);
}

void Application::restartGame() {
    game.reset();
    view.updateBoardFast();
    saveStorage->save(*saveData);
    maxLevel = 0;
}

void keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->keyCallback(key, action);
}

void mouseCallbackWrapper(GLFWwindow* window, int button, int action, int modds) {
    auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->mouseCallback(button, action);
}

void framebufferSizeCallbackWrapper(GLFWwindow* window, int width, int height) {
    auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->framebufferSizeCallback(width, height);
}

#ifdef __EMSCRIPTEN__
EM_BOOL emscripten_resize_callback(int event_type, const EmscriptenUiEvent* e, void* user_data) {
    double width, height;
    emscripten_get_element_css_size("#canvas", &width, &height);
    Application* app = static_cast<Application*>(user_data);
    glfwSetWindowSize(app->window, (int)width * app->dpr, (int)height * app->dpr);
    return EM_TRUE;
}
#endif

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

void Application::framebufferSizeCallback(int width, int height) {
    resizeTimer = .1f;
    resizeWidth = width;
    resizeHeight = height;
#ifdef __EMSCRIPTEN__
    emscripten_set_canvas_element_size("#canvas", resizeWidth, resizeHeight);
#endif
}

void Application::resizeCanvas() {
    canvasW = resizeWidth;
    canvasH = resizeHeight;

    camera.aspect = (float)canvasW / (float)canvasH;

    // Reinitialize lighting pass resolution
    lightingPass.unload();

    int shadowResScaled = SCALE_RES(RES_SHADOWMAP, shadowScale);
    GLuint lightingPassInput[3] = {
        shadowPass.getTexture(),
        (GLuint)shadowResScaled,
        positionPass.getTexture()
    };

    int ssaaWidth = canvasW * lightScale;
    int ssaaHeight = canvasH * lightScale;

    lightingPass.initialize(ssaaWidth, ssaaHeight, static_cast<const void*>(lightingPassInput));

    // Reinitialize UI
    float uiScale = (float)CANVAS_H / canvasH;
    ui.reinitialize(canvasW * uiScale, canvasH * uiScale, uiScale);
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
    double w, h;
    emscripten_get_element_css_size("#canvas", &w, &h);
    canvasW = w;
    canvasH = h;
#endif

    dpr = getDPR();

    canvasW *= dpr;
    canvasH *= dpr;

    glfwInit();
    //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(canvasW, canvasH, "City 2048", nullptr, nullptr);
    glfwMakeContextCurrent(window);

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

#ifdef __EMSCRIPTEN__
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, EM_FALSE, emscripten_resize_callback);
#else
#endif
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallbackWrapper);

    ssaoRadius = SSAO_RADIUS;
    ssaoBias = SSAO_BIAS;
    shadingPower = POWER_SHADING;
    shadowPower = POWER_SHADOW;
    ssaoPower = POWER_OCCLUSION;

    saveStorage->load();
    audio.initialize();
    config.initialize();

    float uiScale = (float)CANVAS_H / canvasH;
    ui.initialize(canvasW * uiScale, canvasH * uiScale, uiScale);

    TextDescription scoreDesc;
    scoreDesc.alignmentX = 0.5f;
    scoreText = ui.createText(scoreDesc);
    
    scoreDesc.scale = glm::vec2(0.8f);
    scoreDesc.color = glm::vec4(.8f, .8f, .8f, 1.0f);
    bestScoreText = ui.createText(scoreDesc);

    ImageDescription restartButtonDesc;
    restartButtonDesc.scale = glm::vec2(.75f);
    restartButtonDesc.alignmentX = 0.0f;
    restartButtonDesc.alignmentY = 1.0f;
    restartButtonImage = ui.createImage(restartButtonDesc, "textures/restart-icon.png");

    ImageDescription undoButtonDesc;
    undoButtonDesc.scale = glm::vec2(.75f);
    undoButtonDesc.alignmentX = 1.0f;
    undoButtonDesc.alignmentY = 1.0f;
    undoButtonImage = ui.createImage(undoButtonDesc, "textures/undo-icon.png");

    ImageDescription noAdsButtonDesc;
    noAdsButtonDesc.scale = glm::vec2(.3f);
    noAdsButtonDesc.alignmentX = 1.0f;
    noAdsButtonDesc.alignmentY = 1.0f;
    noAdsButtonImage = ui.createImage(noAdsButtonDesc, "textures/no-ads-icon.png");

    TextDescription priceLabelDesc;
    Image noAdsImage = ui.getImage(noAdsButtonImage);
    priceLabelDesc.alignmentX = 0.5f;
    priceLabelDesc.alignmentY = 1.0f;
    priceLabelDesc.scale = glm::vec2(0.5f);
    priceText = ui.createText(priceLabelDesc);

    ImageDescription arrowButtonDesc;
    arrowButtonDesc.scale = glm::vec2(.7f);
    arrowButtonDesc.color = glm::vec4(1.0f, 1.0f, 1.0f, .6f);
    rightArrowImage = ui.createImage(arrowButtonDesc, "textures/arrow-right-icon.png");
    leftArrowImage = ui.createImage(arrowButtonDesc, "textures/arrow-left-icon.png");

    ImageDescription audioUnlockerBgDesc;
    audioUnlockerBgDesc.color = glm::vec4(0.0f, 0.0f, 0.0f, .4f);
    audioUnlockerBgImage = ui.createImage(audioUnlockerBgDesc);
    ImageDescription audioUnlockerPointerDesc;
    audioUnlockerPointerDesc.alignmentX = 0.0f;
    audioUnlockerPointerDesc.alignmentY = 1.0f;
    audioUnlockerPointerImage = ui.createImage(audioUnlockerPointerDesc, "textures/tutorial-pointer.png");

    tutorialText = ui.createText();
    ui.getText(tutorialText).active = false;
    ui.getText(tutorialText).value = config.getOption("tutorial-en");

    ui.createButton(restartButtonImage, [this]() { restartGame(); });
    ui.createButton(undoButtonImage, [this]() { onUndoButtonClicked(); });
    ui.createButton(noAdsButtonImage, [this]() { buyNoAds(); });
    ui.createButton(rightArrowImage, [this]() { moveCameraRight(); });
    ui.createButton(leftArrowImage, [this]() { moveCameraLeft(); });
    ui.createButton(audioUnlockerBgImage, [](){});

#ifdef ENABLE_ONSCREEN_LOG
    int logCounter = 0;
    int logShift = 35;
    int logPad = 35;
    
    TextDescription logDesc;
    logDesc.scale = glm::vec2(.5f);
    logDesc.alignmentX = 0.0f;

    fpsText = ui.createText(logDesc);
    if (dpr != -1) dprText = ui.createText(logDesc);
    resText = ui.createText(logDesc);
    shadowText = ui.createText(logDesc);
    gPositionText = ui.createText(logDesc);
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
	saveData = std::make_unique<SaveData>(0, game.getHistoryPointer(), game.getHistoryTree());

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
    time = static_cast<float>(glfwGetTime());
    float dt = time - lastTime;
    lastTime = time;

    frameCounter++;
    frameTimer += dt;

    if (frameTimer >= 1.f) {
        fps = frameCounter / frameTimer;
        frameCounter = 0;
        frameTimer = 0.f;
    }

    // Game logic
    if (saveStorage->checkLoaded()) {
        game.setHistory(saveStorage->getHistoryTree(), saveStorage->getHistoryPointer());

        saveData.reset();
        saveData = std::make_unique<SaveData>(saveStorage->getBestScore(), game.getHistoryPointer(), game.getHistoryTree());
        saveStorage->unload();

        view.updateBoardFast();
        maxLevel = game.getMaxLevel();
    }

    if (!purchasesUpdateStartFlag && js::yandexInitialized()) {
        js::updatePurchases();
        js::gameReadyApi_ready();
        purchasesUpdateStartFlag = true;
        purchasesUpdated = false;

        std::string lang = std::string(js::getLanguage());
        const char* langConfigOption =
            lang == "ru" ||
            lang == "be" ||
            lang == "kk" ||
            lang == "uk" ||
            lang == "uz"
            ? "tutorial-ru"
            : "tutorial-en";

        ui.getText(tutorialText).value = config.getOption(langConfigOption);
    }

    if (!purchasesUpdated && js::getPurchasesUpdateFlag()) {
        bool noAdsPurchased = js::hasPurchase(NO_ADS_ID);
        ui.getImage(noAdsButtonImage).active = !noAdsPurchased;
        ui.getText(priceText).active = !noAdsPurchased;
        ui.getText(priceText).value = js::getProductPrice(NO_ADS_ID);
        if (noAdsPurchased) js::hideBanner();
        else js::showBanner();
        purchasesUpdated = true;
    }

    if (js::getPurchaseCompleteFlag()) {
        js::resetPurchaseCompleteFlag();
        js::resetPurchasesUpdateFlag();
        js::updatePurchases();
        purchasesUpdated = false;
    }

    if (!firstAdFlag)
        firstAdFlag = tryShowInter();

    if (js::getRewardedAdCloseFlag()) {
        if (js::getRewardedAdCompleteFlag()) {
            undoMove();
        }
        js::resetFlagsRewardedAd();
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

    double posX, posY;
    glfwGetCursorPos(window, &posX, &posY);
    glm::vec2 position = glm::vec2(posX, posY);
    swipeDetector.update(position);

    if (ui.getImage(audioUnlockerBgImage).active) {
        if (swipeDetector.checkSwipeRelease()) {
            if (swipeDetector.getSwipe().getLength() <= 1.0f)
                audioUnlockerButtonClicked();
        }
    }
    else if (swipeDetector.checkSwipeMove()) {
        const Swipe& swipe = swipeDetector.getSwipe();

        if (swipe.getLength() >= 10.0f && swipe.time >= 0.01f && swipe.time <= 1.0f) {
            glm::vec2 direction = swipe.getDirection();
            swipeDetector.cancelSwipe();

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

            if (u || r || d || l) {
                MoveDirection direction =
                    u ? MoveDirection::Up :
                    l ? MoveDirection::Right :
                    d ? MoveDirection::Down :
                    MoveDirection::Left;
                go(direction);
            }
        }
    }

    updateUiPositions(time);

    if (saveTimer > .0f) {
        saveTimer -= dt;

        if (saveTimer <= .0f) {
            saveStorage->save(*saveData);
            saveTimer = -1.f;
        }
    }

    if (leaderboardTimer > .0f) {
        leaderboardTimer -= dt;

        if (leaderboardTimer <= .0f) {
            js::setLeaderboardScore(saveData->bestScore);
            leaderboardTimer = -1.f;
        }
    }

    if (resizeTimer > 0.f) {
        resizeTimer -= dt;

        if (resizeTimer <= 0.f) {
            resizeTimer = -1.f;
            resizeCanvas();
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
    saveData.reset();
    lightingPass.unload();
    shadowPass.unload();
    positionPass.unload();
    glfwTerminate();
}