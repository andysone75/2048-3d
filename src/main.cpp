#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#include <emscripten.h>
#endif

#include <iostream>
#include <unordered_map>
#include <string>

#include "raylib.h"
#include "raymath.h"
#include "Game2048.h"
#include "View2048.h"
#include "Scene.h"
#include "Resources.h"

struct ShaderData {
    int lightDirLocation;
    int viewPosLocation;
};

Vector3 getCameraPos(float angle, float radius, float height) {
    return { cos(DEG2RAD * angle) * radius, height, sin(DEG2RAD * angle) * radius };
}

class Application {
public:
    Application(
        Resources& resources,
        Scene& scene,
        Game2048& game,
        View2048& view
    );
    void initialize();
    void terminate();
    void update();
    bool isRunning();

private:
    Resources& resources;
    Scene& scene;
    Game2048& game;
    View2048& view;
    Camera3D camera = {};

    std::unordered_map<const Shader*, ShaderData> shaderLocations;
    
    float cameraAngle = 25;
    float cameraRadius = 10;
    float cameraHeight = 8;
    float cameraSpeed = 160;
    Vector3 cameraOffset = { 1.6, 0, 1.6 };
    
    Vector3 lightDir = { -0.32f, -0.77f, 0.56 };
    Color bgColor = { 70, 129, 221 };
};

int main() {
    int canvasW = 800, canvasH = 600;
#ifdef __EMSCRIPTEN__
    emscripten_get_canvas_element_size("#canvas", &canvasW, &canvasH);
#endif
    InitWindow(canvasW, canvasH, "Hello, raylib!");

    Resources resources;
    resources.initialize();

    Game2048 game(5);
    std::array<std::array<int, 4>, 4> start = { {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 0}
    } };
    game.setBoard(start);

    Scene scene = Scene(resources);
    scene.initialize();

    View2048 view = View2048(game, scene);
    view.updateBoardFast();

    Application app(resources, scene, game, view);
    app.initialize();

#ifdef __EMSCRIPTEN__
    auto callback = [](void* arg) {
        Application* pApp = reinterpret_cast<Application*>(arg);
        pApp->update();
    };
    emscripten_set_main_loop_arg(callback, &app, 0, true);
#else
    while (app.isRunning()) {
        app.update();
    }
#endif

    app.terminate();
    return 0;
}

Application::Application(
    Resources& _resources,
    Scene& _scene,
    Game2048& _game,
    View2048& _view
) : resources(_resources), scene(_scene), game(_game), view(_view) {}

void Application::initialize() {
    camera.target = cameraOffset;
    camera.up = { 0, 1, 0 };
    camera.fovy = 11;
    camera.projection = CAMERA_ORTHOGRAPHIC;
}

void Application::terminate() {
    resources.unload();
    CloseWindow();
}

void Application::update() {
    float dt = GetFrameTime();

    float cameraDirection = 0;
    if (IsKeyDown(KEY_D))
        cameraDirection = -1;
    else if (IsKeyDown(KEY_A))
        cameraDirection = 1;

    cameraAngle += cameraDirection * cameraSpeed * dt;
    camera.position = cameraOffset + getCameraPos(cameraAngle, cameraRadius, cameraHeight);

    bool u = IsKeyPressed(KEY_UP);
    bool r = IsKeyPressed(KEY_RIGHT);
    bool d = IsKeyPressed(KEY_DOWN);
    bool l = IsKeyPressed(KEY_LEFT);

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

    view.update(dt);

    for (const Shader& shader : resources.getLitShaders()) {
        if (shaderLocations.find(&shader) == shaderLocations.end()) {
            ShaderData shaderData;
            shaderData.lightDirLocation = GetShaderLocation(shader, "lightDir");
            shaderData.viewPosLocation = GetShaderLocation(shader, "viewPos");
            shaderLocations[&shader] = shaderData;
        }

        SetShaderValue(shader, shaderLocations[&shader].lightDirLocation, &lightDir, SHADER_UNIFORM_VEC3);
        SetShaderValue(shader, shaderLocations[&shader].viewPosLocation, &camera.position, SHADER_UNIFORM_VEC3);
    }

    BeginDrawing();
    ClearBackground(bgColor);

    BeginMode3D(camera);

    for (const int& objIndex : scene.getOpaqueObjects()) {
        const SceneObject& obj = scene.getObject(objIndex);
        if (obj.isActive)
            DrawModel(obj.model, obj.position, 1.0f, WHITE);
    }

    for (const int& objIndex : scene.getTransparentObjects()) {
        const SceneObject& obj = scene.getObject(objIndex);
        if (obj.isActive)
            DrawModel(obj.model, obj.position, 1.0f, WHITE);
    }

    EndMode3D();

    float logShift = 30;
    DrawText("Build: 8", 10, 10, 20, WHITE);
    DrawFPS(10, 10 + 1 * logShift);
    EndDrawing();
}

bool Application::isRunning() {
    return !WindowShouldClose();
}