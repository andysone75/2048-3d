#pragma once

#include "raylib.h"
#include <vector>
#include <string>

using namespace std;

struct SceneObject {
    Model model;
    Vector3 position;
    bool isActive = true;
};

class Scene {
public:
    void initialize();
    void unload();

    int createObject(string modelId);
    int createObject(string modelId, Vector3 position);
    void addModel(string id, Model model);
    void addShader(string id, const string& vsPath, const string& fsPath);

    SceneObject& getObject(int index);
    const Shader& getShader(string id);
    const vector<Shader>& getShaders() const;
    const vector<SceneObject>& getObjects() const;

private:
    vector<SceneObject> objects;
    vector<Model> models;
    vector<Shader> shaders;
    vector<string> modelIds;
    vector<string> shaderIds;
};
