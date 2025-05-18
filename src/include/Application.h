#pragma once

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <memory>

#include "Engine.h"
#include "Scene.h"
#include "Game2048.h"
#include "View2048.h"
#include "RenderPasses.h"
#include "SwipeDetector.h"
#include "UI.h"
#include "FileSaveStorage.h"
#include "YandexSaveStorage.h"
#include "Audio.h"
#include "Config.h"

#define ENABLE_ONSCREEN_LOG
//#define ENABLE_IMGUI

#define CANVAS_W 576
#define CANVAS_H 1024

class Application {
public:
    Application(
        const Resources& resources,
        Scene& scene,
        Game2048& game,
        View2048& view);
    bool initialize();
    void initGame();
    bool isRunning();
    void mainLoop();
    void terminate();
    void keyCallback(int key, int action);
    void mouseCallback(int button, int action);
    void framebufferSizeCallback(int width, int height);
    void restartGame();
    GLFWwindow* window;
    float dpr = -1;

private:
    int canvasW = CANVAS_W;
    int canvasH = CANVAS_H;
    float ssaoScale = 1.0f;
    float shadowScale = 1.0f;
    float lightScale = 1.0f;

    float time = 0.0f;
    float lastTime = 0.0f;
    float cameraAngleOffset = 0.0f;
    float cameraStartAngle = 25.0f;
    float cameraAngle;
    float cameraSpeed = 160.0f;
    float cameraRadius = 10;
    float cameraHeight = 8;
    glm::vec3 cameraOffset = glm::vec3(1.6f, 0.0f, 1.6f);
    
    int maxLevel = 0;
    int moveInputs[4] = { 
        GLFW_KEY_UP, 
        GLFW_KEY_RIGHT, 
        GLFW_KEY_DOWN, 
        GLFW_KEY_LEFT };

    float ssaoRadius;
    float ssaoBias;
    float ssaoPower;
    float shadingPower;
    float shadowPower;

    Camera camera;
    Light light;

    const Resources& resources;
    Scene& scene;
    Game2048& game;
    View2048& view;
    std::unique_ptr<SaveStorage> saveStorage = 
#ifndef __EMSCRIPTEN__
        std::make_unique<FileSaveStorage>();
#else
        std::make_unique<YandexSaveStorage>();
#endif
	std::unique_ptr<SaveData> saveData;

    SwipeDetector swipeDetector;
    Audio audio;
    Config config;

    RenderPassLighting lightingPass;
    RenderPassShadow shadowPass;
    RenderPassPosition positionPass;

    Mesh fullscreenQuadMesh;
    Shader fullscreenQuadShader;

    UI ui;
    TextId scoreText;
    TextId priceText;
    TextId bestScoreText;
    TextId tutorialText;
    ImageId restartButtonImage;
    ImageId undoButtonImage;
    ImageId noAdsButtonImage;
    ImageId rightArrowImage;
    ImageId leftArrowImage;
    ImageId audioUnlockerBgImage;
    ImageId audioUnlockerPointerImage;

#ifdef ENABLE_ONSCREEN_LOG
    TextId fpsText;
    TextId dprText;
    TextId resText;
    TextId shadowText;
    TextId gPositionText;
    TextId lightingText;
#endif

    bool firstAdFlag = false;
    bool purchasesUpdateStartFlag = false;
    bool purchasesUpdated = false;
    float lastInterTime = 0.0f;
    float saveTimer = -1.0f;
    float leaderboardTimer = -1.0f;
    int frameCounter = 0;
    float frameTimer = 0.f;
    int fps = 0.f;
    float resizeTimer = -1.f;
    int resizeWidth = CANVAS_W;
    int resizeHeight = CANVAS_H;

    void go(MoveDirection direction);
    void undoMove();
    void onUndoButtonClicked();
    bool tryShowInter();
    void updateUiPositions(float dt);
    void moveCameraRight();
    void moveCameraLeft();
    void audioUnlockerButtonClicked();
    void resizeCanvas();
};