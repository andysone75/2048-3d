#include "Scene.h"
#include "raymath.h"
#include <stdexcept>

using namespace std;

Scene::Scene(const Resources& resources)
    : resources(resources) {}

void Scene::initialize() {
    float shift = 1.05f;

    createObjectOpaque(ModelType::GridCell, {0 * shift, -0.15f, 0 * shift});
    createObjectOpaque(ModelType::GridCell, {1 * shift, -0.15f, 0 * shift});
    createObjectOpaque(ModelType::GridCell, {2 * shift, -0.15f, 0 * shift});
    createObjectOpaque(ModelType::GridCell, {3 * shift, -0.15f, 0 * shift});
    createObjectOpaque(ModelType::GridCell, {0 * shift, -0.15f, 1 * shift});
    createObjectOpaque(ModelType::GridCell, {1 * shift, -0.15f, 1 * shift});
    createObjectOpaque(ModelType::GridCell, {2 * shift, -0.15f, 1 * shift});
    createObjectOpaque(ModelType::GridCell, {3 * shift, -0.15f, 1 * shift});
    createObjectOpaque(ModelType::GridCell, {0 * shift, -0.15f, 2 * shift});
    createObjectOpaque(ModelType::GridCell, {1 * shift, -0.15f, 2 * shift});
    createObjectOpaque(ModelType::GridCell, {2 * shift, -0.15f, 2 * shift});
    createObjectOpaque(ModelType::GridCell, {3 * shift, -0.15f, 2 * shift});
    createObjectOpaque(ModelType::GridCell, {0 * shift, -0.15f, 3 * shift});
    createObjectOpaque(ModelType::GridCell, {1 * shift, -0.15f, 3 * shift});
    createObjectOpaque(ModelType::GridCell, {2 * shift, -0.15f, 3 * shift});
    createObjectOpaque(ModelType::GridCell, {3 * shift, -0.15f, 3 * shift});
}

int Scene::createObjectOpaque(ModelType model, Vector3 position) {
    SceneObject object = SceneObject(resources.getModel(model), position);
    objects.push_back(object);
    int index = objects.size() - 1;
    opaqueObjects.push_back(index);
    return index;
}

int Scene::createObjectTransparent(ModelType model, Vector3 position) {
    SceneObject object = SceneObject(resources.getModel(model), position);
    objects.push_back(object);
    int index = objects.size() - 1;
    transparentObjects.push_back(index);
    return index;
}
