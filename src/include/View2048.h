#pragma once

#include "Game2048.h"
#include "Scene.h"
#include <stack>

struct View2048_Object {
    int level;
    SceneObject* sceneObject = nullptr;
};

class View2048 {
public:
    View2048(const Game2048& game, Scene& scene);
    void initialize();
    void update();

private:
    const Game2048& game;
    Scene& scene;
    std::vector<std::stack<View2048_Object>*> objectsPools;
    std::stack<View2048_Object> placedObjects;

    void poolObjects();
    void placeObject(int level, int row, int col);
};