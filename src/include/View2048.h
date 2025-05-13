#pragma once

#include "Game2048.h"
#include "Scene.h"
#include <stack>
#include <functional>
#include <memory>

const float ANIMATION_TIME = .25f;

struct View2048_Cube {
    int* cubeIds;
    //int labelId;
    int level;
};

struct View2048_Object {
    int row, col;
    View2048_Cube cube;
};

class View2048 {
public:
    View2048(const Game2048& game, Scene& scene);
    void update(float dt);
    void updateBoardFast();
    void updateBoard();
    void updateBoard(const std::function<void()>& callback);

private:
    const Game2048& game;
    Scene& scene;

    float animationTimer;
    std::vector<std::stack<View2048_Cube>> objectsPools;
    std::vector<View2048_Object> placedObjects;
    std::vector<View2048_Cube> animationTargets;
    std::vector<glm::vec3> animationStartPositions;
    std::vector<glm::vec3> animationTargetPositions;

    void poolObjects();
    View2048_Object placeObject(int level, int row, int col);
    std::unique_ptr<std::function<void()>> animationCallback;
};