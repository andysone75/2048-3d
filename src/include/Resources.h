#pragma once

#include "raylib.h"
#include <array>
#include <string>

using namespace std;

enum class LitShaderType {
    White,
	Gray,
	LightGreen,
	DarkGreen,
	Blue,
	DarkBlue,
	Purple,
	Pink,
	Red,
	Orange,
	Yellow,
	Gradient,
};

enum class ModelType {
	GridCell,
	Level0,
	Level1,
	Level2,
	Level3,
	Level4,
	Level5,
	Level6,
	Level7,
	Level8,
	Level9,
	Level10,
	Text_2,
	Text_4,
	Text_8,
	Text_16,
	Text_32,
	Text_64,
	Text_128,
	Text_256,
	Text_512,
	Text_1024,
	Text_2048,
};

enum class TextureType {
	Text_2,
	Text_4,
	Text_8,
	Text_16,
	Text_32,
	Text_64,
	Text_128,
	Text_256,
	Text_512,
	Text_1024,
	Text_2048,
};

class Resources {
public:
	void initialize();
	void unload();

	const Model& getModel(ModelType model) const { return models[static_cast<int>(model)]; }
	const array<Shader, 12>& getLitShaders() const { return litShaders; }

private:
	array<Shader, 12> litShaders;
	array<Shader, 1> textShaders;
	array<Model, 23> models;
	array<RenderTexture, 11> renderTextures;
};