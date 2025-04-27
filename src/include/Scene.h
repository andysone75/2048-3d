#pragma once

#include "raylib.h"
#include "Resources.h"
#include <vector>
#include <string>

using namespace std;

struct SceneObject {
    const Model& model;
    Vector3 position;
    bool isActive = true;

    SceneObject(const Model& _model, Vector3 _position)
        : model(_model), position(_position) {};
};

class Scene {
public:
    Scene(const Resources& resources);
    void initialize();

    int createObjectOpaque(ModelType model, Vector3 position = {});
    int createObjectTransparent(ModelType model, Vector3 position = {});

    SceneObject& getObject(int index) { return objects[index]; }
    const vector<int>& getOpaqueObjects() const { return opaqueObjects; }
    const vector<int>& getTransparentObjects() const { return transparentObjects; }

private:
    const Resources& resources;
    vector<int> opaqueObjects;
    vector<int> transparentObjects;
    vector<SceneObject> objects;
};
