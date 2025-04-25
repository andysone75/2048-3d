#include <emscripten/html5.h>
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
    int canvasW = 0, canvasH = 0;
    emscripten_get_canvas_element_size("#canvas", &canvasW, &canvasH);
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

    Game2048 game;
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
    view.update();

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

        if (IsKeyPressed(KEY_UP)) {
            game.goUp();
            game.addRandom();
            view.update();
            game.printBoard();
        } else if (IsKeyPressed(KEY_RIGHT)) {
            game.goLeft();
            game.addRandom();
            view.update();
            game.printBoard();
        } else if (IsKeyPressed(KEY_DOWN)) {
            game.goDown();
            game.addRandom();
            view.update();
            game.printBoard();
        } else if (IsKeyPressed(KEY_LEFT)) {
            game.goRight();
            game.addRandom();
            view.update();
            game.printBoard();
        }

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

        float logShift = 30;
        DrawText("Build: 1", 10, 10, 20, WHITE);
        DrawText(("FPS: " + std::to_string(int(1 / dt))).c_str(), 10, 10 + 1 * logShift, 20, WHITE);
        DrawText(("Angle: " + std::to_string(int(cameraAngle))).c_str(), 10, 10 + 2 * logShift, 20, WHITE);
        DrawText(("Objects: " + std::to_string(scene.getObjects().size())).c_str(), 10, 10 + 3 * logShift, 20, WHITE);
        EndDrawing();
    }

    scene.unload();
    CloseWindow();
    return 0;
}
