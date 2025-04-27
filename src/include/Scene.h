#pragma once

#include "raylib.h"
#include "Resources.h"
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
    Scene(const Resources& resources);
    void initialize();

    int createObject(Model model);
    int createObject(Model model, Vector3 position);

    SceneObject& getObject(int index);
    const vector<SceneObject>& getObjects() const;

private:
    const Resources& resources;
    vector<SceneObject> objects;
};
