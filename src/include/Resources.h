#pragma once

#include "raylib.h"
#include <array>
#include <string>

using namespace std;

enum class ShaderType {
    White,
	Blue,
	Green,
	Purple,
	Red,
	Yellow,
};

enum class ModelType {
	GridCell,
	Level0,
	Level1,
	Level2,
	Level3,
	Level4,
};

class Resources {
public:
	void initialize();
	void unload();
	Model getModel(ModelType model) const;
	const array<Shader, 6>& getShaders() const;

private:
	array<Shader, 6> shaders;
	array<Model, 6> models;
};