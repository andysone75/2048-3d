#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#include <emscripten.h>
#endif

#include <iostream>
#include <unordered_map>
#include <string>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "Game2048.h"
#include "View2048.h"
#include "Scene.h"
#include "Resources.h"

#define SHADOWMAP_RESOLUTION 1024
#define SHADOWMAP_PROJ_MAT MatrixOrtho(-3, 3, -3, 3, .1, 10.)

//#define SHADOWMAP_DEBUG

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
    Camera3D lightCam = {};
    RenderTexture2D renderTarget;
    RenderTexture2D shadowTarget;
    Shader depthShader;

    float cameraAngle = 25;
    float cameraRadius = 10;
    float cameraHeight = 8;
    float cameraSpeed = 160;
    Vector3 cameraOffset = { 1.6, 0, 1.6 };
    
    Vector3 lightDir = { -0.32f, -0.77f, 0.56 };
    Color bgColor = { 70, 129, 221 };
    float time = 0;
};

int main() {
    int canvasW = 720, canvasH = 1280;
#ifdef __EMSCRIPTEN__
    emscripten_get_canvas_element_size("#canvas", &canvasW, &canvasH);
#endif
    InitWindow(canvasW, canvasH, "Hello, raylib!");

    Resources resources;
    resources.initialize();

    Game2048 game(11);
    std::array<std::array<int, 4>, 4> start = { {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    } };
    game.setBoard(start);
    game.addRandom();

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

    Vector3 lightCamOffset = cameraOffset + Vector3UnitY * 0.9f;
    lightCam.target = lightCamOffset;
    lightCam.up = { 0, 1, 0 };
    lightCam.fovy = 6;
    lightCam.projection = CAMERA_ORTHOGRAPHIC;
    lightCam.position = lightCamOffset - lightDir * 3;

    renderTarget = LoadRenderTexture(GetScreenWidth() * 2.f, GetScreenHeight() * 2.f);
    SetTextureFilter(renderTarget.texture, TEXTURE_FILTER_BILINEAR);

    shadowTarget = { 0 };
    shadowTarget.id = rlLoadFramebuffer();
    shadowTarget.texture.width = SHADOWMAP_RESOLUTION;
    shadowTarget.texture.height = SHADOWMAP_RESOLUTION;
    if (shadowTarget.id > 0) {
        rlEnableFramebuffer(shadowTarget.id);
        shadowTarget.depth.id = rlLoadTextureDepth(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION, false);
        shadowTarget.depth.width = SHADOWMAP_RESOLUTION;
        shadowTarget.depth.height = SHADOWMAP_RESOLUTION;
        shadowTarget.depth.format = 19;
        shadowTarget.depth.mipmaps = 1;

        rlFramebufferAttach(shadowTarget.id, shadowTarget.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);
        if (rlFramebufferComplete(shadowTarget.id)) TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", shadowTarget.id);

        rlDisableFramebuffer();
    }
    else TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");

    depthShader = LoadShader("shaders/depth-vert.glsl", "shaders/depth-frag.glsl");
    Matrix lightViewProj = MatrixMultiply(GetCameraMatrix(lightCam), SHADOWMAP_PROJ_MAT);
    SetShaderValueMatrix(depthShader, GetShaderLocation(depthShader, "viewProj"), lightViewProj);
}

void Application::terminate() {
    resources.unload();
    UnloadRenderTexture(renderTarget);
    UnloadRenderTexture(shadowTarget);
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
        int shadowMapResolution = SHADOWMAP_RESOLUTION;
        SetShaderValue(shader, GetShaderLocation(shader, "lightDir"),            &lightDir,             SHADER_UNIFORM_VEC3);
        SetShaderValue(shader, GetShaderLocation(shader, "viewPos"),             &camera.position,      SHADER_UNIFORM_VEC3);
        SetShaderValue(shader, GetShaderLocation(shader, "time"),                &time,                 SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader, GetShaderLocation(shader, "shadowMapResolution"), &shadowMapResolution,  SHADER_UNIFORM_INT);
    }

    // Shadow Pass
    BeginTextureMode(shadowTarget);
        ClearBackground(WHITE);

        BeginShaderMode(depthShader);
            BeginMode3D(lightCam);
                for (const int& objIndex : scene.getOpaqueObjects()) {
                    const SceneObject& obj = scene.getObject(objIndex);
                    if (!obj.isActive) continue;

                    Vector3 pos = obj.position;
                    Matrix modelMat = MatrixMultiply(obj.model.transform, MatrixTranslate(pos.x, pos.y, pos.z));
                    SetShaderValueMatrix(depthShader, GetShaderLocation(depthShader, "model"), modelMat);

                    for (int i = 0; i < obj.model.meshCount; i++) {
                        Mesh& mesh = obj.model.meshes[i];
                        rlEnableVertexArray(mesh.vaoId);
                        if (mesh.indices != nullptr)
                            rlDrawVertexArrayElements(0, mesh.triangleCount * 3, 0);
                        else 
                            rlDrawVertexArray(0, mesh.vertexCount);
                        rlDisableVertexArray();
                    }
                }
            EndMode3D();
        EndShaderMode();
    EndTextureMode();

    // Scene pass
    BeginTextureMode(renderTarget);
        ClearBackground(bgColor);
        BeginMode3D(camera);
            for (const int& objIndex : scene.getOpaqueObjects()) {
                const SceneObject& obj = scene.getObject(objIndex);
                if (!obj.isActive) continue;

                Shader shader = obj.model.materials[0].shader;
                Matrix lightViewProj = MatrixMultiply(GetCameraMatrix(lightCam), SHADOWMAP_PROJ_MAT);

                rlEnableShader(shader.id);
                int slot = 10;
                rlActiveTextureSlot(10);
                rlEnableTexture(shadowTarget.depth.id);
                rlSetUniform(GetShaderLocation(shader, "shadowMap"), &slot, SHADER_UNIFORM_INT, 1);
                rlSetUniformMatrix(GetShaderLocation(shader, "lightVp"), lightViewProj);
                rlDisableShader();

                DrawModel(obj.model, obj.position, 1.0f, WHITE);
                rlDisableTexture();
            }

            for (const int& objIndex : scene.getTransparentObjects()) {
                const SceneObject& obj = scene.getObject(objIndex);
                if (obj.isActive)
                    DrawModel(obj.model, obj.position, 1.0f, WHITE);
            }
        EndMode3D();
    EndTextureMode();

    BeginDrawing();
        ClearBackground(bgColor);
            DrawTexturePro(
                renderTarget.texture,
                Rectangle{ 0,0, (float)renderTarget.texture.width, -(float)renderTarget.texture.height },
                Rectangle{0,0, (float)GetScreenWidth(), -(float)GetScreenHeight() },
                Vector2{0,0},
                0.f,
                WHITE
            );
        float logShift = 30;
        DrawText("Build: 8", 10, 10, 20, WHITE);
        DrawFPS(10, 10 + 1 * logShift);

#ifdef SHADOWMAP_DEBUG
        DrawTexturePro(
            shadowTarget.depth,
            Rectangle{ 0, 0, (float)shadowTarget.texture.width, -(float)shadowTarget.texture.height },
            Rectangle{ 0, (float)GetScreenHeight() - 256, 256, 256},
            Vector2{ 0, 0 },
            0.f,
            Fade(WHITE, .8f)
        );
#endif

    EndDrawing();

    time += dt;
}

bool Application::isRunning() {
    return !WindowShouldClose();
}