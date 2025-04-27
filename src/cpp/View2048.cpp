#include "View2048.h"
#include "Utils.h"
#include <cmath>
#include "raymath.h"
#include <stdexcept>

View2048::View2048(const Game2048& game, Scene& scene)
    : game(game), scene(scene) {}

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

        const auto& target = animationTargets[i];

        scene.getObject(target.cube.cubeId).position = pos;
        scene.getObject(target.cube.labelId).position = pos + Vector3{0, 0.6f * (target.level + 1) + .1f, 0};
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
                    animationTargets.push_back(object);
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
            std::stack<View2048_Object> pool;
            objectsPools.push_back(pool);
        }
        
        std::stack<View2048_Object>& pool = objectsPools[object.level];
        pool.push(object);
        scene.getObject(object.cube.cubeId).isActive = false;
        scene.getObject(object.cube.labelId).isActive = false;
    }

    placedObjects.clear();
}

ModelType getCubeModelByLevel(int level) {
    switch (level) {
    case 0:
        return ModelType::Level0;
    case 1:
        return ModelType::Level1;
    case 2:
        return ModelType::Level2;
    case 3:
        return ModelType::Level3;
    case 4:
        return ModelType::Level4;
    }

    throw std::runtime_error("Model not found");
}

ModelType getLabelModelByLevel(int level) {
    switch (level) {
    case 0:
        return ModelType::Text_2;
    case 1:
        return ModelType::Text_4;
    case 2:
        return ModelType::Text_8;
    case 3:
        return ModelType::Text_16;
    case 4:
        return ModelType::Text_32;
    }

    throw std::runtime_error("Model not found");
}

View2048_Object View2048::placeObject(int level, int row, int col) {
    View2048_Object object;
    bool poolHit = false;

    if (level < objectsPools.size()) {
        std::stack<View2048_Object>& pool = objectsPools[level];

        if (!pool.empty()) {
            object = pool.top();
            pool.pop();
            poolHit = true;
        }
    }

    if (!poolHit) {
        object.cube.cubeId = scene.createObjectOpaque(getCubeModelByLevel(level));
        object.cube.labelId = scene.createObjectTransparent(getLabelModelByLevel(level));
    }

    SceneObject& sceneObjectCube = scene.getObject(object.cube.cubeId);
    sceneObjectCube.position = {row * 1.05f, 0.0f, col * 1.05f};
    sceneObjectCube.isActive = true;

    SceneObject& sceneObjectLabel = scene.getObject(object.cube.labelId);
    sceneObjectLabel.position = { row * 1.05f, 0.6f * (level + 1) + .1f, col * 1.05f };
    sceneObjectLabel.isActive = true;

    object.row = row;
    object.col = col;
    object.level = level;
    placedObjects.push_back(object);
    return object;
}