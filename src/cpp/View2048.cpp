#include "View2048.h"
#include <cmath>
#include <stdexcept>
#include <glm/glm.hpp>

int getCubesCount(int level) { return level < 10 ? (level + 1) : 1; }

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

    for (size_t i = 0; i < animationTargets.size(); i++) {
        glm::vec3 pos = glm::mix(
            animationStartPositions[i],
            animationTargetPositions[i],
            t
        );

        const auto& target = animationTargets[i];
        int cubesCount = getCubesCount(target.level);

        for (int i = 0; i < cubesCount; i++) {
            scene.getObject(target.cubeIds[i]).position = glm::vec3(pos.x, .2f * i, pos.z);
        }
        
        //scene.getObject(target.labelId).position = pos + glm::vec3(0, .2f * (target.level + 1), 0);
    }
}

void View2048::updateBoardFast() {
    const auto& board = game.getBoard();
    poolObjects();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
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
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (previousBoard[i][j] == 0) continue;
            View2048_Object object = placeObject(previousBoard[i][j] - 1, i, j);

            for (auto& move : lastMoves) {
                if (move.fromX == i && move.fromY == j) {
                    glm::vec3 srcPosition = { move.fromX * 1.05f, 0.0f, move.fromY * 1.05f };
                    glm::vec3 dstPosition = { move.toX * 1.05f, 0.0f, move.toY * 1.05f };
                    animationTargets.push_back(object.cube);
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
    for (auto& object : placedObjects) {
        while (object.cube.level + 1 > objectsPools.size()) {
            std::stack<View2048_Cube> pool;
            objectsPools.push_back(pool);
        }
        
        std::stack<View2048_Cube>& pool = objectsPools[object.cube.level];
        pool.push(object.cube);

        int cubesCount = getCubesCount(object.cube.level);

        for (int i = 0; i < cubesCount; i++) {
            scene.getObject(object.cube.cubeIds[i]).isActive = false;
        }

        //scene.getObject(object.cube.labelId).isActive = false;
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
    case 5:
        return ModelType::Level5;
    case 6:
        return ModelType::Level6;
    case 7:
        return ModelType::Level7;
    case 8:
        return ModelType::Level8;
    case 9:
        return ModelType::Level9;
    case 10:
        return ModelType::Level10;
    }

    throw std::runtime_error("Model not found");
}

//ModelType getLabelModelByLevel(int level) {
//    switch (level) {
//    case 0:
//        return ModelType::Text_2;
//    case 1:
//        return ModelType::Text_4;
//    case 2:
//        return ModelType::Text_8;
//    case 3:
//        return ModelType::Text_16;
//    case 4:
//        return ModelType::Text_32;
//    case 5:
//        return ModelType::Text_64;
//    case 6:
//        return ModelType::Text_128;
//    case 7:
//        return ModelType::Text_256;
//    case 8:
//        return ModelType::Text_512;
//    case 9:
//        return ModelType::Text_1024;
//    case 10:
//        return ModelType::Text_2048;
//    }
//
//    throw std::runtime_error("Model not found");
//}

View2048_Object View2048::placeObject(int level, int row, int col) {
    View2048_Object object;
    bool poolHit = false;

    if (level < objectsPools.size()) {
        std::stack<View2048_Cube>& pool = objectsPools[level];

        if (!pool.empty()) {
            object.cube = pool.top();
            pool.pop();
            poolHit = true;
        }
    }

    if (!poolHit) {
        if (level < 10) {
            object.cube.cubeIds = new int[level + 1];

            for (int i = 0; i < level + 1; i++) {
                object.cube.cubeIds[i] = scene.createObjectOpaque(getCubeModelByLevel(i));
            }
        } else {
            object.cube.cubeIds = new int[1];
            object.cube.cubeIds[0] = scene.createObjectOpaque(getCubeModelByLevel(10));
        }

        //object.cube.labelId = scene.createObjectTransparent(getLabelModelByLevel(level));
        object.cube.level = level;
    }

    int cubesCount = getCubesCount(level);

    for (int i = 0; i < cubesCount; i++) {
        SceneObject& sceneObjectCube = scene.getObject(object.cube.cubeIds[i]);
        sceneObjectCube.position = { row * 1.05f, .2f * i, col * 1.05f };
        sceneObjectCube.isActive = true;
    }

    //SceneObject& sceneObjectLabel = scene.getObject(object.cube.labelId);
    //sceneObjectLabel.position = { row * 1.05f, .2f * (level + 1), col * 1.05f};
    //sceneObjectLabel.isActive = true;
    //sceneObjectLabel.isActive = false; // Temporary disable labels

    object.row = row;
    object.col = col;
    placedObjects.push_back(object);
    return object;
}