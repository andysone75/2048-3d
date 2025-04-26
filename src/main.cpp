#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

#include <iostream>
#include <unordered_map>
#include <string>

#include "raylib.h"
#include "raymath.h"
#include "Game2048.h"
#include "View2048.h"
#include "Scene.h"

struct ShaderData {
    int lightPosLocation;
    int viewPosLocation;
};

Vector3 getCameraPos(float angle, float radius, float height) {
    return { cos(DEG2RAD * angle) * radius, height, sin(DEG2RAD * angle) * radius };
}

int main() {
    int canvasW = 800, canvasH = 600;
#ifdef __EMSCRIPTEN__
    emscripten_get_canvas_element_size("#canvas", &canvasW, &canvasH);
#endif
    InitWindow(canvasW, canvasH, "Hello, raylib!");

    float cameraAngle = 25;
    float cameraRadius = 10;
    float cameraHeight = 8;
    float cameraSpeed = 160;
    Vector3 cameraOffset = {4, 0, 4};

    Camera3D camera = { 0 };
    camera.target = cameraOffset;
    camera.up = { 0, 1, 0 };
    camera.fovy = 20;
    camera.projection = CAMERA_ORTHOGRAPHIC;

    Vector3 lightPos = { 2, 4, 2 };

    Game2048 game(5);
    std::array<std::array<int, 4>, 4> start = {{
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 0}
    }};
    game.setBoard(start);

    Scene scene;
    scene.initialize();

    View2048 view(game, scene);
    view.initialize();
    view.updateBoardFast();

    std::unordered_map<const Shader*, ShaderData> shaderLocations;
    
    while (!WindowShouldClose()) {
        float cameraDirection = 0;

        if (IsKeyDown(KEY_A))
            cameraDirection = -1;
        else if (IsKeyDown(KEY_D))
            cameraDirection = 1;

        float dt = GetFrameTime();
        cameraAngle += cameraDirection * cameraSpeed * dt;
        camera.position = cameraOffset + getCameraPos(cameraAngle, cameraRadius, cameraHeight);
        Vector3 camPos = camera.position;

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

        for (const Shader& shader : scene.getShaders()) {
            if (shaderLocations.find(&shader) == shaderLocations.end()) {
                ShaderData shaderData;
                shaderData.lightPosLocation = GetShaderLocation(shader, "lightPos");
                shaderData.viewPosLocation = GetShaderLocation(shader, "viewPos");
                shaderLocations[&shader] = shaderData;
            }
        }

        for (const Shader& shader : scene.getShaders()) {
            SetShaderValue(shader, shaderLocations[&shader].lightPosLocation, &lightPos, SHADER_UNIFORM_VEC3);
            SetShaderValue(shader, shaderLocations[&shader].viewPosLocation, &camPos, SHADER_UNIFORM_VEC3);
        }

        BeginDrawing();
        ClearBackground(GRAY);

        BeginMode3D(camera);
        for (const auto& obj : scene.getObjects()) {
            if (obj.isActive)
                DrawModel(obj.model, obj.position, 1.0f, WHITE);
        }
        EndMode3D();

        int hiddenCount = 0;
        for (const auto& obj : scene.getObjects())
            if (!obj.isActive)
                hiddenCount++;


        float logShift = 30;
        DrawText("Build: 2", 10, 10, 20, WHITE);
        DrawText(("FPS: " + std::to_string(int(1 / dt))).c_str(), 10, 10 + 1 * logShift, 20, WHITE);
        DrawText(("Angle: " + std::to_string(int(cameraAngle))).c_str(), 10, 10 + 2 * logShift, 20, WHITE);
        DrawText(("Objects: " + std::to_string(scene.getObjects().size())).c_str(), 10, 10 + 3 * logShift, 20, WHITE);
        DrawText(("Hidden: " + std::to_string(hiddenCount)).c_str(), 10, 10 + 4 * logShift, 20, WHITE);
        EndDrawing();
    }

    scene.unload();
    CloseWindow();
    return 0;
}
