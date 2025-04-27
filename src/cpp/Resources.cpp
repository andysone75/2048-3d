#include "Resources.h"
#include "raymath.h"

Shader loadShader(Vector3 color) {
	Shader shader = LoadShader("shaders/lit-vert.glsl", "shaders/lit-frag.glsl");
	int colorLoc = GetShaderLocation(shader, "objectColor");
	SetShaderValue(shader, colorLoc, &color, SHADER_UNIFORM_VEC3);
	return shader;
}

Model loadModel(Mesh mesh, Matrix scaleMatrix, Matrix translateMatrix, Shader shader) {
	Model model = LoadModelFromMesh(mesh);
	model.transform = MatrixMultiply(translateMatrix, scaleMatrix);
	model.materials[0].shader = shader;
	return model;
}

void Resources::initialize() {
	// Shaders
	Vector3 white = { 1.0, 1.0, 1.0 };
	Vector3 blue = { 0.509804f, 0.8509804f, 0.9176471f };
	Vector3 green = { 0.0f, 1.0f, 0.3960784f };
	Vector3 purple = { 0.8117647f, 0.4078431f, 1.0f };
	Vector3 red = { 1.0f, 0.2392157f, 0.2392157f };
	Vector3 yellow = { 1.0f, 0.8862745f, 0.3372549f };

	shaders[0] = loadShader(white);
	shaders[1] = loadShader(blue);
	shaders[2] = loadShader(green);
	shaders[3] = loadShader(purple);
	shaders[4] = loadShader(red);
	shaders[5] = loadShader(yellow);

	// Models
	Mesh cube = GenMeshCube(1, 1, 1);
	
	Model gridCell = loadModel(cube, MatrixScale(1, .3, 1), MatrixTranslate(0, -.3 / 2, 0), shaders[static_cast<int>(ShaderType::White)]);
	Model level0 = loadModel(cube, MatrixScale(.7, .7 * 1, .7), MatrixTranslate(0, .7 * 1 / 2, 0), shaders[static_cast<int>(ShaderType::Blue)]);
	Model level1 = loadModel(cube, MatrixScale(.7, .7 * 2, .7), MatrixTranslate(0, .7 * 1 / 2, 0), shaders[static_cast<int>(ShaderType::Green)]);
	Model level2 = loadModel(cube, MatrixScale(.7, .7 * 3, .7), MatrixTranslate(0, .7 * 1 / 2, 0), shaders[static_cast<int>(ShaderType::Purple)]);
	Model level3 = loadModel(cube, MatrixScale(.7, .7 * 4, .7), MatrixTranslate(0, .7 * 1 / 2, 0), shaders[static_cast<int>(ShaderType::Red)]);
	Model level4 = loadModel(cube, MatrixScale(.7, .7 * 5, .7), MatrixTranslate(0, .7 * 1 / 2, 0), shaders[static_cast<int>(ShaderType::Yellow)]);

	models[0] = gridCell;
	models[1] = level0;
	models[2] = level1;
	models[3] = level2;
	models[4] = level3;
	models[5] = level4;
}

void Resources::unload() {
	// Still have a bug with one mesh for multiple models
	for (const auto& model : models)
		UnloadModel(model);

	for (const auto& shader : shaders)
		UnloadShader(shader);
}

Model Resources::getModel(ModelType type) const {
	return models[static_cast<int>(type)];
}

const array<Shader, 6>& Resources::getShaders () const {
	return shaders;
}