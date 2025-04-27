#pragma once

#include "Game2048.h"
#include "Scene.h"
#include <stack>

const float ANIMATION_TIME = .25f;

struct View2048_Object {
    int level;
    int row, col;
    int sceneObject = -1;
};

class View2048 {
public:
    View2048(const Resources& resources, const Game2048& game, Scene& scene);
    void update(float dt);
    void updateBoardFast();
    void updateBoard();

private:
    const Game2048& game;
    const Resources& resources;
    Scene& scene;

    float animationTimer;
    std::vector<std::stack<int>> objectsPools;
    std::vector<View2048_Object> placedObjects;
    std::vector<int> animationTargets;
    std::vector<Vector3> animationStartPositions;
    std::vector<Vector3> animationTargetPositions;

    void poolObjects();
    View2048_Object placeObject(int level, int row, int col);
};