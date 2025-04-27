#include "Scene.h"
#include "raymath.h"
#include <stdexcept>

using namespace std;

Scene::Scene(const Resources& resources)
    : resources(resources) {}

void Scene::initialize() {
    float shift = 1.05f;

    Model gridCell = resources.getModel(ModelType::GridCell);

    createObject(gridCell, {0 * shift, -0.25f, 0 * shift});
    createObject(gridCell, {1 * shift, -0.25f, 0 * shift});
    createObject(gridCell, {2 * shift, -0.25f, 0 * shift});
    createObject(gridCell, {3 * shift, -0.25f, 0 * shift});
    createObject(gridCell, {0 * shift, -0.25f, 1 * shift});
    createObject(gridCell, {1 * shift, -0.25f, 1 * shift});
    createObject(gridCell, {2 * shift, -0.25f, 1 * shift});
    createObject(gridCell, {3 * shift, -0.25f, 1 * shift});
    createObject(gridCell, {0 * shift, -0.25f, 2 * shift});
    createObject(gridCell, {1 * shift, -0.25f, 2 * shift});
    createObject(gridCell, {2 * shift, -0.25f, 2 * shift});
    createObject(gridCell, {3 * shift, -0.25f, 2 * shift});
    createObject(gridCell, {0 * shift, -0.25f, 3 * shift});
    createObject(gridCell, {1 * shift, -0.25f, 3 * shift});
    createObject(gridCell, {2 * shift, -0.25f, 3 * shift});
    createObject(gridCell, {3 * shift, -0.25f, 3 * shift});
}

int Scene::createObject(Model model) {
    SceneObject object;
    object.model = model;
    objects.push_back(object);
    return objects.size() - 1;
}

int Scene::createObject(Model model, Vector3 position) {
    int index = createObject(model);
    SceneObject& object = getObject(index);
    object.position = position;
    return index;
}

SceneObject& Scene::getObject(int index) {
    return objects[index];
}

const vector<SceneObject>& Scene::getObjects() const {
    return objects;
}
