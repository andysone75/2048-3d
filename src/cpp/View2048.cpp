#include "View2048.h"
#include "Utils.h"
#include <cmath>
#include "raymath.h"
#include <stdexcept>

View2048::View2048(const Resources& resources, const Game2048& game, Scene& scene)
    : resources(resources), game(game), scene(scene) {}

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
                    Vector3 srcPosition = { move.fromX * 1.05f, 0.0f, move.fromY * 1.05f };
                    Vector3 dstPosition = { move.toX * 1.05f, 0.0f, move.toY * 1.05f };
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

Model getModelByLevel(int level, const Resources& resources) {
    switch (level) {
    case 0:
        return resources.getModel(ModelType::Level0);
    case 1:
        return resources.getModel(ModelType::Level1);
    case 2:
        return resources.getModel(ModelType::Level2);
    case 3:
        return resources.getModel(ModelType::Level3);
    case 4:
        return resources.getModel(ModelType::Level4);
    }

    throw std::runtime_error("Shader not found");
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
        sceneObjectIndex = scene.createObject(getModelByLevel(level, resources));
    }

    SceneObject& sceneObject = scene.getObject(sceneObjectIndex);
    sceneObject.position = {row * 1.05f, 0.0f, col * 1.05f};
    sceneObject.isActive = true;
    
    object.sceneObject = sceneObjectIndex;
    object.row = row;
    object.col = col;
    object.level = level;
    placedObjects.push_back(object);
    return object;
}