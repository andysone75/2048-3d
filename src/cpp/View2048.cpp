#include "View2048.h"

View2048::View2048(const Game2048& game, Scene& scene)
    : game(game), scene(scene) {}

void View2048::initialize() {
    Mesh cubeMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    Model cubeModel = LoadModelFromMesh(cubeMesh);
    scene.addModel("cube_0", cubeModel, "grid_cell");
    scene.addModel("cube_1", cubeModel, "grid_cell", {1,2,1});
    scene.addModel("cube_2", cubeModel, "grid_cell", {1,3,1});
    scene.addModel("cube_3", cubeModel, "grid_cell", {1,4,1});
    scene.addModel("cube_4", cubeModel, "grid_cell", {1,5,1});
}

void View2048::update() {
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

void View2048::poolObjects() {
    while (!placedObjects.empty()) {
        View2048_Object object = placedObjects.top();
        placedObjects.pop();

        while (object.level + 1 > objectsPools.size())
            objectsPools.push_back(nullptr);
        
        if (objectsPools[object.level] == nullptr)
            objectsPools[object.level] = new std::stack<View2048_Object>();
        
        std::stack<View2048_Object>* pool = objectsPools[object.level];
        pool->push(object);
        object.sceneObject->isActive = false;
    }
}

void View2048::placeObject(int level, int row, int col) {
    View2048_Object object;

    if (level < objectsPools.size()) {
        std::stack<View2048_Object>* pool = objectsPools[level];

        if (pool != nullptr && !pool->empty()) {
            object = pool->top();
            pool->pop();
        }
    }

    if (object.sceneObject == nullptr) {
        object.level = level;
        object.sceneObject = scene.createObject("cube_" + to_string(object.level));
    }

    object.sceneObject->position = {row * 2.2f, .75f, col * 2.2f};
    object.sceneObject->isActive = true;
    placedObjects.push(object);
}