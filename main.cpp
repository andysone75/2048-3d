#include "raylib.h"
#include "raymath.h"

int main() {
    InitWindow(800, 600, "Hello, raylib!");
    SetTargetFPS(60);

    Camera3D camera = { 0 };
    camera.position = { 4.0f, 4.0f, 4.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Mesh cubeMesh = GenMeshCube(2.0f, 2.0f, 2.0f);
    Model cubeModel = LoadModelFromMesh(cubeMesh);

    Shader shader = LoadShader("shaders/lighting.vs", "shaders/lighting.fs");
    cubeModel.materials[0].shader = shader;

    Vector3 lightPos = { 2.0f, 4.0f, 2.0f };

    int lightPosLoc = GetShaderLocation(shader, "lightPos");
    int viewPosLoc = GetShaderLocation(shader, "viewPos");

    float rotation = 0.0f;
    
    while (!WindowShouldClose()) {
        rotation += 1.0f;

        Matrix transform = MatrixRotateY(DEG2RAD * rotation);
        cubeModel.transform = transform;

        Vector3 camPos = camera.position;
        SetShaderValue(shader, lightPosLoc, &lightPos, SHADER_UNIFORM_VEC3);
        SetShaderValue(shader, viewPosLoc, &camPos, SHADER_UNIFORM_VEC3);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawModel(cubeModel, { 0, 1, 0 }, 1.0f, WHITE);
        EndMode3D();

        DrawText("Lit Cube (Phong-style lighting)", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    UnloadModel(cubeModel);
    UnloadShader(shader);
    CloseWindow();
    return 0;
}
