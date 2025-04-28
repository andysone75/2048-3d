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
	model.transform = MatrixMultiply(scaleMatrix, translateMatrix);
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
	DrawText(text.c_str(), (texSize - fontWidth * text.size()) / 2, (texSize - fontSize) / 2, fontSize, Color{0,0,0,int(.6f * 255)});
	EndTextureMode();
	return textRender;
}

inline int getShaderIndex(LitShaderType shaderType) { return static_cast<int>(shaderType); }

void Resources::initialize() {
	// Lit Shaders
	Vector3 white = Vector3Ones;
	Vector3 gray = Vector3{ 186, 202, 211 } / (255 + 35);
	Vector3 lightGreen = Vector3{ 146, 255, 124 } / 255;
	Vector3 darkGreen = Vector3{ 5, 181, 119 } / 255;
	Vector3 blue = Vector3{ 10, 226, 255 } / 255;
	Vector3 darkBlue = Vector3{ 42, 120, 255 } / 255;
	Vector3 purple = Vector3{ 202, 103, 255 } / 255;
	Vector3 pink = Vector3{ 158, 51, 216 } / 255;
	Vector3 red = Vector3{ 255, 110, 112 } / 255;
	Vector3 orange = Vector3{ 255, 159, 124 } / 255;
	Vector3 yellow = Vector3{ 255, 238, 124 } / 255;

	litShaders[getShaderIndex(LitShaderType::White)] = loadLitShader(white);
	litShaders[getShaderIndex(LitShaderType::Gray)] = loadLitShader(gray);
	litShaders[getShaderIndex(LitShaderType::LightGreen)] = loadLitShader(lightGreen);
	litShaders[getShaderIndex(LitShaderType::DarkGreen)] = loadLitShader(darkGreen);
	litShaders[getShaderIndex(LitShaderType::Blue)] = loadLitShader(blue);
	litShaders[getShaderIndex(LitShaderType::DarkBlue)] = loadLitShader(darkBlue);
	litShaders[getShaderIndex(LitShaderType::Purple)] = loadLitShader(purple);
	litShaders[getShaderIndex(LitShaderType::Pink)] = loadLitShader(pink);
	litShaders[getShaderIndex(LitShaderType::Red)] = loadLitShader(red);
	litShaders[getShaderIndex(LitShaderType::Orange)] = loadLitShader(orange);
	litShaders[getShaderIndex(LitShaderType::Yellow)] = loadLitShader(yellow);
	litShaders[getShaderIndex(LitShaderType::Gradient)] = LoadShader(
		"shaders/gradient-vert.glsl",
		"shaders/gradient-frag.glsl"
	);

	int color0Loc = GetShaderLocation(litShaders[getShaderIndex(LitShaderType::Gradient)], "color0");
	int color1Loc = GetShaderLocation(litShaders[getShaderIndex(LitShaderType::Gradient)], "color1");
	SetShaderValue(litShaders[getShaderIndex(LitShaderType::Gradient)], color0Loc, &darkBlue, SHADER_UNIFORM_VEC3);
	SetShaderValue(litShaders[getShaderIndex(LitShaderType::Gradient)], color1Loc, &purple, SHADER_UNIFORM_VEC3);

	// Text Shaders
	textShaders[0] = loadTextShader();

	// Render Textures
	renderTextures[0] = loadTextRenderTexture("2");
	renderTextures[1] = loadTextRenderTexture("4");
	renderTextures[2] = loadTextRenderTexture("8");
	renderTextures[3] = loadTextRenderTexture("16");
	renderTextures[4] = loadTextRenderTexture("32");
	renderTextures[5] = loadTextRenderTexture("64");
	renderTextures[6] = loadTextRenderTexture("128");
	renderTextures[7] = loadTextRenderTexture("256");
	renderTextures[8] = loadTextRenderTexture("512");
	renderTextures[9] = loadTextRenderTexture("1024");
	renderTextures[10] = loadTextRenderTexture("2048");

	// Models
	Model gridCell = loadModel(GenMeshCube(1, 1, 1), litShaders[static_cast<int>(LitShaderType::White)], MatrixScale(1, .3, 1), MatrixTranslate(0, 0, 0));
	
	Model level0  = loadModel(GenMeshCube(1, 1, 1), litShaders[static_cast<int>(LitShaderType::Gray)],		 MatrixScale(.7, .7 * .2, .7), MatrixTranslate(0, .7 * .2 * .5, 0));
	Model level1  = loadModel(GenMeshCube(1, 1, 1), litShaders[static_cast<int>(LitShaderType::LightGreen)], MatrixScale(.7, .7 * .2, .7), MatrixTranslate(0, .7 * .2 * .5, 0));
	Model level2  = loadModel(GenMeshCube(1, 1, 1), litShaders[static_cast<int>(LitShaderType::DarkGreen)],  MatrixScale(.7, .7 * .2, .7), MatrixTranslate(0, .7 * .2 * .5, 0));
	Model level3  = loadModel(GenMeshCube(1, 1, 1), litShaders[static_cast<int>(LitShaderType::Blue)],		 MatrixScale(.7, .7 * .2, .7), MatrixTranslate(0, .7 * .2 * .5, 0));
	Model level4  = loadModel(GenMeshCube(1, 1, 1), litShaders[static_cast<int>(LitShaderType::DarkBlue)],	 MatrixScale(.7, .7 * .2, .7), MatrixTranslate(0, .7 * .2 * .5, 0));
	Model level5  = loadModel(GenMeshCube(1, 1, 1), litShaders[static_cast<int>(LitShaderType::Purple)],	 MatrixScale(.7, .7 * .2, .7), MatrixTranslate(0, .7 * .2 * .5, 0));
	Model level6  = loadModel(GenMeshCube(1, 1, 1), litShaders[static_cast<int>(LitShaderType::Pink)],		 MatrixScale(.7, .7 * .2, .7), MatrixTranslate(0, .7 * .2 * .5, 0));
	Model level7  = loadModel(GenMeshCube(1, 1, 1), litShaders[static_cast<int>(LitShaderType::Red)],		 MatrixScale(.7, .7 * .2, .7), MatrixTranslate(0, .7 * .2 * .5, 0));
	Model level8  = loadModel(GenMeshCube(1, 1, 1), litShaders[static_cast<int>(LitShaderType::Orange)],	 MatrixScale(.7, .7 * .2, .7), MatrixTranslate(0, .7 * .2 * .5, 0));
	Model level9  = loadModel(GenMeshCube(1, 1, 1), litShaders[static_cast<int>(LitShaderType::Yellow)],	 MatrixScale(.7, .7 * .2, .7), MatrixTranslate(0, .7 * .2 * .5, 0));
	Model level10 = loadModel(GenMeshCube(1, 1, 1), litShaders[static_cast<int>(LitShaderType::Gradient)],	 MatrixScale(.7, .19 * 11, .7), MatrixTranslate(0, .19 * 11 * .5, 0));

	Model planeModel_2 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	Model planeModel_4 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	Model planeModel_8 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	Model planeModel_16 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	Model planeModel_32 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	Model planeModel_64 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	Model planeModel_128 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	Model planeModel_256 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	Model planeModel_512 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	Model planeModel_1024 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	Model planeModel_2048 = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));

	planeModel_2.transform = MatrixRotateY(PI / 2);
	planeModel_2.materials[0].shader = textShaders[0];
	planeModel_2.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_2)].texture;

	planeModel_4.transform = MatrixRotateY(PI / 2);
	planeModel_4.materials[0].shader = textShaders[0];
	planeModel_4.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_4)].texture;

	planeModel_8.transform = MatrixRotateY(PI / 2);
	planeModel_8.materials[0].shader = textShaders[0];
	planeModel_8.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_8)].texture;

	planeModel_16.transform = MatrixRotateY(PI / 2);
	planeModel_16.materials[0].shader = textShaders[0];
	planeModel_16.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_16)].texture;

	planeModel_32.transform = MatrixRotateY(PI / 2);
	planeModel_32.materials[0].shader = textShaders[0];
	planeModel_32.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_32)].texture;

	planeModel_64.transform = MatrixRotateY(PI / 2);
	planeModel_64.materials[0].shader = textShaders[0];
	planeModel_64.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_64)].texture;

	planeModel_128.transform = MatrixRotateY(PI / 2);
	planeModel_128.materials[0].shader = textShaders[0];
	planeModel_128.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_128)].texture;

	planeModel_256.transform = MatrixRotateY(PI / 2);
	planeModel_256.materials[0].shader = textShaders[0];
	planeModel_256.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_256)].texture;

	planeModel_512.transform = MatrixRotateY(PI / 2);
	planeModel_512.materials[0].shader = textShaders[0];
	planeModel_512.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_512)].texture;

	planeModel_1024.transform = MatrixRotateY(PI / 2);
	planeModel_1024.materials[0].shader = textShaders[0];
	planeModel_1024.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_1024)].texture;

	planeModel_2048.transform = MatrixRotateY(PI / 2);
	planeModel_2048.materials[0].shader = textShaders[0];
	planeModel_2048.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = renderTextures[static_cast<int>(TextureType::Text_2048)].texture;

	models[0] = gridCell;
	models[1] = level0;
	models[2] = level1;
	models[3] = level2;
	models[4] = level3;
	models[5] = level4;
	models[6] = level5;
	models[7] = level6;
	models[8] = level7;
	models[9] = level8;
	models[10] = level9;
	models[11] = level10;
	models[12] = planeModel_2;
	models[13] = planeModel_4;
	models[14] = planeModel_8;
	models[15] = planeModel_16;
	models[16] = planeModel_32;
	models[17] = planeModel_64;
	models[18] = planeModel_128;
	models[19] = planeModel_256;
	models[20] = planeModel_512;
	models[21] = planeModel_1024;
	models[22] = planeModel_2048;
}

void Resources::unload() {
	for (const auto& model : models)
		UnloadModel(model);

	for (const auto& shader : litShaders)
		UnloadShader(shader);

	for (const auto& tex : renderTextures)
		UnloadRenderTexture(tex);
}