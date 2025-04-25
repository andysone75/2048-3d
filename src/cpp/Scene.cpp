#include "Scene.h"
#include "raymath.h"

using namespace std;

const string GRID_ID = "grid_cell";

void Scene::initialize() {
    Mesh cubeMesh = GenMeshCube(2.0f, 2.0f, 2.0f);
    Model cubeModel = LoadModelFromMesh(cubeMesh);

    addShader(GRID_ID, "shaders/lighting.vs", "shaders/lighting.fs");
    addModel(GRID_ID, cubeModel, GRID_ID, {1.0f, .25f, 1.0f});

    float shift = 2.2f;

    createObject(GRID_ID, {0 * shift, 0, 0 * shift});
    createObject(GRID_ID, {1 * shift, 0, 0 * shift});
    createObject(GRID_ID, {2 * shift, 0, 0 * shift});
    createObject(GRID_ID, {3 * shift, 0, 0 * shift});
    createObject(GRID_ID, {0 * shift, 0, 1 * shift});
    createObject(GRID_ID, {1 * shift, 0, 1 * shift});
    createObject(GRID_ID, {2 * shift, 0, 1 * shift});
    createObject(GRID_ID, {3 * shift, 0, 1 * shift});
    createObject(GRID_ID, {0 * shift, 0, 2 * shift});
    createObject(GRID_ID, {1 * shift, 0, 2 * shift});
    createObject(GRID_ID, {2 * shift, 0, 2 * shift});
    createObject(GRID_ID, {3 * shift, 0, 2 * shift});
    createObject(GRID_ID, {0 * shift, 0, 3 * shift});
    createObject(GRID_ID, {1 * shift, 0, 3 * shift});
    createObject(GRID_ID, {2 * shift, 0, 3 * shift});
    createObject(GRID_ID, {3 * shift, 0, 3 * shift});
}

void Scene::unload() {
    for (const Model& model : models) {
        UnloadModel(model);
    }

    for (const Shader& shader : shaders) {
        UnloadShader(shader);
    }
}

const vector<SceneObject>& Scene::getObjects() const {
    return objects;
}

const vector<Shader>& Scene::getShaders() const {
    return shaders;
}

SceneObject* Scene::createObject(string modelId) {
    int modelIndex = -1;

    for (size_t i = 0; i < modelIds.size(); i++)
    {
        if (modelIds[i] == modelId) {
            modelIndex = i;
            break;
        }
    }

    SceneObject object;
    object.model = models[modelIndex];
    objects.push_back(object);
    return &objects.back();
}

SceneObject* Scene::createObject(string modelId, Vector3 position) {
    SceneObject* object = createObject(modelId);
    object->position = position;
    return object;
}

void Scene::addModel(string id, Model model, string shaderId, Vector3 scale) {
    model.transform = MatrixScale(scale.x, scale.y, scale.z);
    addModel(id, model, shaderId);
}

void Scene::addModel(string id, Model model, string shaderId) {
    int shaderIndex = -1;

    for (size_t i = 0; i < shaderIds.size(); i++)
    {
        if (shaderIds[i] == shaderId) {
            shaderIndex = i;
            break;
        }
    }

    model.materials[0].shader = shaders[shaderIndex];

    for (size_t i = 0; i < modelIds.size(); i++)
    {
        if (modelIds[i] == id) {
            models[i] = model;
            return;
        }
    }
    
    models.push_back(model);
    modelIds.push_back(id);
}

void Scene::addShader(string id, const string& vsPath, const string& fsPath) {
    Shader shader = LoadShader(vsPath.c_str(), fsPath.c_str());

    for (size_t i = 0; i < shaderIds.size(); i++)
    {
        if (shaderIds[i] == id) {
            shaders[i] = shader;
        }
    }
    
    shaders.push_back(shader);
    shaderIds.push_back(id);
}
