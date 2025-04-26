#include "View2048.h"
#include "Utils.h"
#include <cmath>
#include "raymath.h"

View2048::View2048(const Game2048& game, Scene& scene)
    : game(game), scene(scene) {}

#include <iostream>

void View2048::initialize() {
    Mesh cubeMesh0 = GenMeshCube(1.0f, 1.0f, 1.0f);
    Mesh cubeMesh1 = GenMeshCube(1.0f, 2.0f, 1.0f);
    Mesh cubeMesh2 = GenMeshCube(1.0f, 3.0f, 1.0f);
    Mesh cubeMesh3 = GenMeshCube(1.0f, 4.0f, 1.0f);
    Mesh cubeMesh4 = GenMeshCube(1.0f, 5.0f, 1.0f);

    Model cubeModel0 = LoadModelFromMesh(cubeMesh0);
    Model cubeModel1 = LoadModelFromMesh(cubeMesh1);
    Model cubeModel2 = LoadModelFromMesh(cubeMesh2);
    Model cubeModel3 = LoadModelFromMesh(cubeMesh3);
    Model cubeModel4 = LoadModelFromMesh(cubeMesh4);

    cubeModel0.transform = MatrixTranslate(0.0f, 0.5f, 0.0f);
    cubeModel1.transform = MatrixTranslate(0.0f, 1.0f, 0.0f);
    cubeModel2.transform = MatrixTranslate(0.0f, 1.5f, 0.0f);
    cubeModel3.transform = MatrixTranslate(0.0f, 2.0f, 0.0f);
    cubeModel4.transform = MatrixTranslate(0.0f, 2.5f, 0.0f);

    cubeModel0.materials[0].shader = scene.getShader("grid_cell");
    cubeModel1.materials[0].shader = scene.getShader("grid_cell");
    cubeModel2.materials[0].shader = scene.getShader("grid_cell");
    cubeModel3.materials[0].shader = scene.getShader("grid_cell");
    cubeModel4.materials[0].shader = scene.getShader("grid_cell");

    scene.addModel("cube_0", cubeModel0);
    scene.addModel("cube_1", cubeModel1);
    scene.addModel("cube_2", cubeModel2);
    scene.addModel("cube_3", cubeModel3);
    scene.addModel("cube_4", cubeModel4);
}

void View2048::update(float dt) {
    if (animationTimer <= 0.0f) {
        return;
    }

    animationTimer -= dt;

    if (animationTimer <= 0.0f) {
        updateBoardFast();
        return;
    }

    float t = 1.0f - animationTimer / ANIMATION_TIME;
    t = 1.0f - std::pow(1.0f - t, 3); // out cubic easing

    for (size_t i = 0; i < animationTargets.size(); i++)
    {
        Vector3 pos = utils::lerp(
            animationStartPositions[i],
            animationTargetPositions[i],
            t
        );

        scene.getObject(animationTargets[i]).position = pos;
    }
}

void View2048::updateBoardFast() {
    const auto& board = game.getBoard();
    poolObjects();
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (board[i][j] == 0) continue;
            placeObject(board[i][j] - 1, i, j);
        }
    }
}

void View2048::updateBoard() {
    animationTargets.clear();
    animationStartPositions.clear();
    animationTargetPositions.clear();

    const auto& previousBoard = game.getPreviousBoard();
    const auto& lastMoves = game.getLastMoves();
    poolObjects();
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (previousBoard[i][j] == 0) continue;
            View2048_Object object = placeObject(previousBoard[i][j] - 1, i, j);

            for (auto& move : lastMoves) {
                if (move.fromX == i && move.fromY == j) {
                    Vector3 srcPosition = { move.fromX * 2.2f, 0.0f, move.fromY * 2.2f };
                    Vector3 dstPosition = { move.toX * 2.2f, 0.0f, move.toY * 2.2f };
                    animationTargets.push_back(object.sceneObject);
                    animationStartPositions.push_back(srcPosition);
                    animationTargetPositions.push_back(dstPosition);
                    break;
                }
            }
        }
    }

    animationTimer = ANIMATION_TIME;
}

#include <iostream>

void View2048::poolObjects() {
    for (auto& object : placedObjects)
    {
        while (object.level + 1 > objectsPools.size())
        {
            std::stack<int> pool;
            objectsPools.push_back(pool);
        }
        
        std::stack<int>& pool = objectsPools[object.level];
        pool.push(object.sceneObject);
        scene.getObject(object.sceneObject).isActive = false;
    }

    placedObjects.clear();
}

View2048_Object View2048::placeObject(int level, int row, int col) {
    View2048_Object object;
    int sceneObjectIndex = -1;

    if (level < objectsPools.size()) {
        std::stack<int>& pool = objectsPools[level];

        if (!pool.empty()) {
            sceneObjectIndex = pool.top();
            pool.pop();
        }
    }

    if (sceneObjectIndex == -1) {
        string modelId = "cube_" + to_string(level);
        sceneObjectIndex = scene.createObject(modelId);
    }

    SceneObject& sceneObject = scene.getObject(sceneObjectIndex);
    sceneObject.position = {row * 2.2f, 0.0f, col * 2.2f};
    sceneObject.isActive = true;
    
    object.sceneObject = sceneObjectIndex;
    object.row = row;
    object.col = col;
    object.level = level;
    placedObjects.push_back(object);
    return object;
}