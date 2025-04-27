#include "Resources.h"
#include "raymath.h"

Shader loadLitShader(Vector3 color) {
	Shader shader = LoadShader(
		"shaders/lit-vert.glsl",
		"shaders/lit-frag.glsl"
	);
	int colorLoc = GetShaderLocation(shader, "objectColor");
	SetShaderValue(shader, colorLoc, &color, SHADER_UNIFORM_VEC3);
	return shader;
}

Shader loadTextShader() {
	Shader shader = LoadShader(
		"shaders/texture_plane_vs.glsl",
		"shaders/texture_plane_fs.glsl"
	);
	return shader;
}

Model loadModel(
	Mesh mesh, 
	Shader shader, 
	Matrix scaleMatrix = MatrixIdentity(), 
	Matrix translateMatrix = MatrixIdentity()
) {
	Model model = LoadModelFromMesh(mesh);
	model.transform = MatrixMultiply(translateMatrix, scaleMatrix);
	model.materials[0].shader = shader;
	return model;
}

RenderTexture loadTextRenderTexture(string text) {
	int texSize = 256;
	int fontSize = 120;
	int fontWidth = int(fontSize * 0.55f);
	RenderTexture textRender = LoadRenderTexture(256, 256);
	BeginTextureMode(textRender);
	ClearBackground(BLANK);
	DrawText(text.c_str(), (texSize - fontWidth * text.size()) / 2, (texSize - fontSize) / 2, fontSize, BLACK);
	EndTextureMode();
	return textRender;
}

void Resources::initialize() {
	// Lit Shaders
	Vector3 white = { 1.0, 1.0, 1.0 };
	Vector3 blue = { 0.509804f, 0.8509804f, 0.9176471f };
	Vector3 green = { 0.0f, 1.0f, 0.3960784f };
	Vector3 purple = { 0.8117647f, 0.4078431f, 1.0f };
	Vector3 red = { 1.0f, 0.2392157f, 0.2392157f };
	Vector3 yellow = { 1.0f, 0.8862745f, 0.3372549f };

	litShaders[0] = loadLitShader(white);
	litShaders[1] = loadLitShader(blue);
	litShaders[2] = loadLitShader(green);
	litShaders[3] = loadLitShader(purple);
	litShaders[4] = loadLitShader(red);
	litShaders[5] = loadLitShader(yellow);

	// Text Shaders
	textShaders[0] = loadTextShader();

	// Render Textures
	renderTextures[0] = loadTextRenderTexture("2");
	renderTextures[1] = loadTextRenderTexture("4");
	renderTextures[2] = loadTextRenderTexture("8");
	renderTextures[3] = loadTextRenderTexture("16");
	renderTextures[4] = loadTextRenderTexture("32");

	// Models
	Model gridCell = loadModel(
		GenMeshCube(1, 1, 1),
		litShaders[static_cast<int>(LitShaderType::White)],
		MatrixScale(1, .3, 1),
		MatrixTranslate(0, -.3 / 2, 0)
	);

	Model level0 = loadModel(
		GenMeshCube(1, 1, 1),
		litShaders[static_cast<int>(LitShaderType::Blue)],
		MatrixScale(.7, .7 * 1, .7),
		MatrixTranslate(0, .7 * 1 / 2, 0)
	);

	Model level1 = loadModel(
		GenMeshCube(1, 1, 1),
		litShaders[static_cast<int>(LitShaderType::Green)],
		MatrixScale(.7, .7 * 2, .7),
		MatrixTranslate(0, .7 * 1 / 2, 0)
	);

	Model level2 = loadModel(
		GenMeshCube(1, 1, 1),
		litShaders[static_cast<int>(LitShaderType::Purple)],
		MatrixScale(.7, .7 * 3, .7),
		MatrixTranslate(0, .7 * 1 / 2, 0)
	);

	Model level3 = loadModel(
		GenMeshCube(1, 1, 1),
		litShaders[static_cast<int>(LitShaderType::Red)],
		MatrixScale(.7, .7 * 4, .7),
		MatrixTranslate(0, .7 * 1 / 2, 0)
	);

	Model level4 = loadModel(
		GenMeshCube(1, 1, 1),
		litShaders[static_cast<int>(LitShaderType::Yellow)],
		MatrixScale(.7, .7 * 5, .7),
		MatrixTranslate(0, .7 * 1 / 2, 0)
	);

	Model planeModel2 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	Model planeModel4 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	Model planeModel8 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	Model planeModel16 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	Model planeModel32 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));

	planeModel2.transform = MatrixRotateY(PI / 2);
	planeModel2.materials[0].shader = textShaders[0];
	planeModel2.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_2)].texture;

	planeModel4.transform = MatrixRotateY(PI / 2);
	planeModel4.materials[0].shader = textShaders[0];
	planeModel4.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_4)].texture;

	planeModel8.transform = MatrixRotateY(PI / 2);
	planeModel8.materials[0].shader = textShaders[0];
	planeModel8.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_8)].texture;

	planeModel16.transform = MatrixRotateY(PI / 2);
	planeModel16.materials[0].shader = textShaders[0];
	planeModel16.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_16)].texture;

	planeModel32.transform = MatrixRotateY(PI / 2);
	planeModel32.materials[0].shader = textShaders[0];
	planeModel32.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_32)].texture;

	models[0] = gridCell;
	models[1] = level0;
	models[2] = level1;
	models[3] = level2;
	models[4] = level3;
	models[5] = level4;
	models[6] = planeModel2;
	models[7] = planeModel4;
	models[8] = planeModel8;
	models[9] = planeModel16;
	models[10] = planeModel32;
}

void Resources::unload() {
	for (const auto& model : models)
		UnloadModel(model);

	for (const auto& shader : litShaders)
		UnloadShader(shader);

	for (const auto& tex : renderTextures)
		UnloadRenderTexture(tex);
}