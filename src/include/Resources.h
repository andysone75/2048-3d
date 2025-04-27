#pragma once

#include "raylib.h"
#include <array>
#include <string>

using namespace std;

enum class LitShaderType {
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
	Text_2,
	Text_4,
	Text_8,
	Text_16,
	Text_32,
};

enum class TextureType {
	Text_2,
	Text_4,
	Text_8,
	Text_16,
	Text_32,
};

class Resources {
public:
	void initialize();
	void unload();

	const Model& getModel(ModelType model) const { return models[static_cast<int>(model)]; }
	const array<Shader, 6>& getLitShaders() const { return litShaders; }

private:
	array<Shader, 6> litShaders;
	array<Shader, 1> textShaders;
	array<Model, 11> models;
	array<RenderTexture, 5> renderTextures;
};