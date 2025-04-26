#include "Scene.h"
#include "raymath.h"
#include <stdexcept>

using namespace std;

const string GRID_CELL = "grid_cell";

void Scene::initialize() {
    addShader("white", "shaders/lit-vert.glsl", "shaders/lit-frag.glsl");
    Shader shader = getShader("white");
    int colorLoc = GetShaderLocation(shader, "objectColor");
    Vector3 col = { 1.0, 1.0, 1.0 };
    SetShaderValue(shader, colorLoc, &col, SHADER_UNIFORM_VEC3);

    Mesh cubeMesh = GenMeshCube(1.0f, 0.3f, 1.0f);
    Model cubeModel = LoadModelFromMesh(cubeMesh);
    cubeModel.materials[0].shader = getShader("white");

    addModel(GRID_CELL, cubeModel);

    float shift = 1.05f;

    createObject(GRID_CELL, {0 * shift, -0.25f, 0 * shift});
    createObject(GRID_CELL, {1 * shift, -0.25f, 0 * shift});
    createObject(GRID_CELL, {2 * shift, -0.25f, 0 * shift});
    createObject(GRID_CELL, {3 * shift, -0.25f, 0 * shift});
    createObject(GRID_CELL, {0 * shift, -0.25f, 1 * shift});
    createObject(GRID_CELL, {1 * shift, -0.25f, 1 * shift});
    createObject(GRID_CELL, {2 * shift, -0.25f, 1 * shift});
    createObject(GRID_CELL, {3 * shift, -0.25f, 1 * shift});
    createObject(GRID_CELL, {0 * shift, -0.25f, 2 * shift});
    createObject(GRID_CELL, {1 * shift, -0.25f, 2 * shift});
    createObject(GRID_CELL, {2 * shift, -0.25f, 2 * shift});
    createObject(GRID_CELL, {3 * shift, -0.25f, 2 * shift});
    createObject(GRID_CELL, {0 * shift, -0.25f, 3 * shift});
    createObject(GRID_CELL, {1 * shift, -0.25f, 3 * shift});
    createObject(GRID_CELL, {2 * shift, -0.25f, 3 * shift});
    createObject(GRID_CELL, {3 * shift, -0.25f, 3 * shift});
}

void Scene::unload() {
    for (Model& model : models)
        UnloadModel(model);

    for (Shader& shader : shaders)
        UnloadShader(shader);
}

const vector<SceneObject>& Scene::getObjects() const {
    return objects;
}

const vector<Shader>& Scene::getShaders() const {
    return shaders;
}

int Scene::createObject(string modelId) {
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
    return objects.size() - 1;
}

int Scene::createObject(string modelId, Vector3 position) {
    int index = createObject(modelId);
    SceneObject& object = getObject(index);
    object.position = position;
    return index;
}

SceneObject& Scene::getObject(int index) {
    return objects[index];
}

void Scene::addModel(string id, Model model) {
    for (size_t i = 0; i < modelIds.size(); i++)
        if (modelIds[i] == id)
            throw std::runtime_error("Model ID already exists");

    models.push_back(model);
    modelIds.push_back(id);
}

void Scene::addShader(string id, const string& vsPath, const string& fsPath) {
    Shader shader = LoadShader(vsPath.c_str(), fsPath.c_str());

    for (size_t i = 0; i < shaderIds.size(); i++)
        if (shaderIds[i] == id)
            throw std::runtime_error("Shader ID already exists");
    
    shaders.push_back(shader);
    shaderIds.push_back(id);
}

const Shader& Scene::getShader(string id) {
    for (size_t i = 0; i < shaderIds.size(); i++)
        if (id == shaderIds[i])
            return shaders[i];
    throw std::runtime_error("Shader not found");
}
