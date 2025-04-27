#pragma once

#include "Game2048.h"
#include "Scene.h"
#include <stack>

const float ANIMATION_TIME = .25f;

struct View2048_Cube {
    int cubeId;
    int labelId;
};

struct View2048_Object {
    int level;
    int row, col;
    View2048_Cube cube;
};

class View2048 {
public:
    View2048(const Game2048& game, Scene& scene);
    void update(float dt);
    void updateBoardFast();
    void updateBoard();

private:
    const Game2048& game;
    Scene& scene;

    float animationTimer;
    std::vector<std::stack<View2048_Object>> objectsPools;
    std::vector<View2048_Object> placedObjects;
    std::vector<View2048_Object> animationTargets;
    std::vector<Vector3> animationStartPositions;
    std::vector<Vector3> animationTargetPositions;

    void poolObjects();
    View2048_Object placeObject(int level, int row, int col);
};