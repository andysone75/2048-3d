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
#include "TextRenderer.h"

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

private:
    GLFWwindow* window;
    int canvasW = 720;
    int canvasH = 1280;
    float dpr = -1;
    float ssaoScale = 1.0f;
    float shadowScale = 1.0f;
    float lightScale = 1.0f;

    float lastTime;
    float cameraAngle = 25.0f;
    float cameraSpeed = 160.0f;
    float cameraRadius = 10;
    float cameraHeight = 8;
    glm::vec3 cameraOffset = glm::vec3(1.6f, 0.0f, 1.6f);

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

    RenderPassLighting lightingPass;
    RenderPassShadow shadowPass;
    RenderPassPosition positionPass;

    Mesh fullscreenQuadMesh;
    Shader fullscreenQuadShader;

    TextRenderer textRenderer;
};