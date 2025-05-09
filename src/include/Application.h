#pragma once

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "Engine.h"
#include "Scene.h"
#include "Game2048.h"
#include "View2048.h"
#include "RenderPasses.h"
#include "SwipeDetector.h"
#include "UI.h"

//#define ENABLE_ONSCREEN_LOG
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
    void restartGame();
    void undoMove();

private:
    GLFWwindow* window;
    int canvasW = CANVAS_W;
    int canvasH = CANVAS_H;
    float dpr = -1;
    float ssaoScale = 1.0f;
    float shadowScale = 1.0f;
    float lightScale = 1.0f;

    float lastTime;
    float cameraAngleOffset = 0.0f;
    float cameraStartAngle = 25.0f;
    float cameraAngle;
    float cameraSpeed = 160.0f;
    float cameraRadius = 10;
    float cameraHeight = 8;
    glm::vec3 cameraOffset = glm::vec3(1.6f, 0.0f, 1.6f);
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
    SwipeDetector swipeDetector;

    RenderPassLighting lightingPass;
    RenderPassShadow shadowPass;
    RenderPassPosition positionPass;

    Mesh fullscreenQuadMesh;
    Shader fullscreenQuadShader;

    UI ui;
    TextId scoreText;
    TextId bestScoreText;

#ifdef ENABLE_ONSCREEN_LOG
    TextId fpsText;
    TextId dprText;
    TextId resText;
    TextId shadowText;
    TextId gPositionText;
    TextId lightingText;
#endif
};