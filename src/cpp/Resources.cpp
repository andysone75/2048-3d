#include "Resources.h"
#include "Engine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#define TEXT_TEXTURE_RESOLUTION 128

Shader loadTextShader() {
	Shader shader = Shader::Load(
		"shaders/texture_plane_vs.glsl",
		"shaders/texture_plane_fs.glsl"
	);
	return shader;
}

Model loadModel(
	Mesh mesh, 
	Shader shader, 
	glm::mat4 scaleMatrix = glm::mat4(1),
	glm::mat4 translateMatrix = glm::mat4(1)
) {
	Model model = Model::Load(mesh);
	model.transform = translateMatrix * scaleMatrix;
	model.shader = shader;
	return model;
}

// Maybe use FreeType for fonts rendering

//Texture2D createTextTexture(string text, unsigned int fbo) {
//	int fontSize = 70;
//	int fontWidth = int(fontSize * 0.55f);
//
//	unsigned int textureId = rlLoadTexture(nullptr, TEXT_TEXTURE_RESOLUTION, TEXT_TEXTURE_RESOLUTION, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
//	rlFramebufferAttach(fbo, textureId, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
//
//	Texture2D texture = { 0 };
//	texture.width = TEXT_TEXTURE_RESOLUTION;
//	texture.height = TEXT_TEXTURE_RESOLUTION;
//	texture.format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
//	texture.mipmaps = 1;
//	texture.id = textureId;
//
//	RenderTexture2D rt = {};
//	rt.id = fbo;
//	rt.texture = texture;
//
//	BeginTextureMode(rt);
//	ClearBackground(BLANK);
//	DrawText(
//		text.c_str(),
//		(TEXT_TEXTURE_RESOLUTION - fontWidth * text.size()) / 2,
//		(TEXT_TEXTURE_RESOLUTION - fontSize) / 2,
//		fontSize,
//		Color{ 0,0,0,(unsigned int)(.6f * 255) });
//	EndTextureMode();
//
//	return texture;
//}

glm::vec3 colorToVector3(Color color) {
	return glm::vec3{
		(float)color.r / 255.0f,
		(float)color.g / 255.0f,
		(float)color.b / 255.0f,
	};
}

inline int getShaderIndex(LitShaderType shaderType) { return static_cast<int>(shaderType); }

#define MATRIX_SCALE(x, y, z) glm::scale(glm::mat4(1), glm::vec3(x, y, z))
#define MATRIX_TRANSLATE(x, y, z) glm::translate(glm::mat4(1), glm::vec3(x, y, z))

void Resources::initialize() {
	// Lit Shaders

	Color white = WHITE;
	Color gray		 = Color{ 186, 202, 211, 255 };
	Color lightGreen = Color{ 146, 255, 124, 255 };
	Color darkGreen  = Color{ 5,   181, 119, 255 };
	Color blue		 = Color{ 10,  226, 255, 255 };
	Color darkBlue	 = Color{ 42,  120, 255, 255 };
	Color purple	 = Color{ 202, 103, 255, 255 };
	Color pink		 = Color{ 158, 51,  216, 255 };
	Color red		 = Color{ 255, 110, 112, 255 };
	Color orange	 = Color{ 255, 159, 124, 255 };
	Color yellow	 = Color{ 255, 238, 124, 255 };

	Shader shaderLit = Shader::Load(
		"shaders/lit-vs.glsl",
		"shaders/lit-fs.glsl"
	);

	Shader shaderGradient = Shader::Load(
		"shaders/gradient-vert.glsl",
		"shaders/gradient-frag.glsl"
	);

	litShaders[getShaderIndex(LitShaderType::Color)] = shaderLit;
	litShaders[getShaderIndex(LitShaderType::Gradient)] = shaderGradient;

	shaderGradient.use();
	shaderGradient.setUniformVec3("color0", colorToVector3(darkBlue));
	shaderGradient.setUniformVec3("color1", colorToVector3(purple));

	// Text Shaders
	textShaders[0] = loadTextShader();

	// Models
	Model gridCell = loadModel(Mesh::GenCube(white),	 shaderLit, MATRIX_SCALE(1, .3, 1), MATRIX_TRANSLATE(0, 0, 0));
	Model level0  = loadModel(Mesh::GenCube(gray),		 shaderLit, MATRIX_SCALE(.7, .7 * .2, .7), MATRIX_TRANSLATE(0, .7 * .2 * .5, 0));
	Model level1  = loadModel(Mesh::GenCube(lightGreen), shaderLit, MATRIX_SCALE(.7, .7 * .2, .7), MATRIX_TRANSLATE(0, .7 * .2 * .5, 0));
	Model level2  = loadModel(Mesh::GenCube(darkGreen),  shaderLit, MATRIX_SCALE(.7, .7 * .2, .7), MATRIX_TRANSLATE(0, .7 * .2 * .5, 0));
	Model level3  = loadModel(Mesh::GenCube(blue),		 shaderLit, MATRIX_SCALE(.7, .7 * .2, .7), MATRIX_TRANSLATE(0, .7 * .2 * .5, 0));
	Model level4  = loadModel(Mesh::GenCube(darkBlue),	 shaderLit, MATRIX_SCALE(.7, .7 * .2, .7), MATRIX_TRANSLATE(0, .7 * .2 * .5, 0));
	Model level5  = loadModel(Mesh::GenCube(purple),	 shaderLit, MATRIX_SCALE(.7, .7 * .2, .7), MATRIX_TRANSLATE(0, .7 * .2 * .5, 0));
	Model level6  = loadModel(Mesh::GenCube(pink),		 shaderLit, MATRIX_SCALE(.7, .7 * .2, .7), MATRIX_TRANSLATE(0, .7 * .2 * .5, 0));
	Model level7  = loadModel(Mesh::GenCube(red),		 shaderLit, MATRIX_SCALE(.7, .7 * .2, .7), MATRIX_TRANSLATE(0, .7 * .2 * .5, 0));
	Model level8  = loadModel(Mesh::GenCube(orange),	 shaderLit, MATRIX_SCALE(.7, .7 * .2, .7), MATRIX_TRANSLATE(0, .7 * .2 * .5, 0));
	Model level9  = loadModel(Mesh::GenCube(yellow),	 shaderLit, MATRIX_SCALE(.7, .7 * .2, .7), MATRIX_TRANSLATE(0, .7 * .2 * .5, 0));
	Model level10 = loadModel(Mesh::GenCube(), shaderGradient,		MATRIX_SCALE(.7, .19 * 11, .7), MATRIX_TRANSLATE(0, .19 * 11 * .5, 0));

	//Model planeModel_2 = loadModel(Mesh::GenPlane(1.0f, 1.0f, 1), textShaders[0]);
	//Model planeModel_4 = loadModel(Mesh::GenPlane(1.0f, 1.0f, 1), textShaders[0]);
	//Model planeModel_8 = loadModel(Mesh::GenPlane(1.0f, 1.0f, 1), textShaders[0]);
	//Model planeModel_16 = loadModel(Mesh::GenPlane(1.0f, 1.0f, 1), textShaders[0]);
	//Model planeModel_32 = loadModel(Mesh::GenPlane(1.0f, 1.0f, 1), textShaders[0]);
	//Model planeModel_64 = loadModel(Mesh::GenPlane(1.0f, 1.0f, 1), textShaders[0]);
	//Model planeModel_128 = loadModel(Mesh::GenPlane(1.0f, 1.0f, 1), textShaders[0]);
	//Model planeModel_256 = loadModel(Mesh::GenPlane(1.0f, 1.0f, 1), textShaders[0]);
	//Model planeModel_512 = loadModel(Mesh::GenPlane(1.0f, 1.0f, 1), textShaders[0]);
	//Model planeModel_1024 = loadModel(Mesh::GenPlane(1.0f, 1.0f, 1), textShaders[0]);
	//Model planeModel_2048 = loadModel(Mesh::GenPlane(1.0f, 1.0f, 1), textShaders[0]);

	//planeModel_2.transform = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0, 1, 0));
	//planeModel_4.transform = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0, 1, 0));
	//planeModel_8.transform = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0, 1, 0));
	//planeModel_16.transform = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0, 1, 0));
	//planeModel_32.transform = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0, 1, 0));
	//planeModel_64.transform = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0, 1, 0));
	//planeModel_128.transform = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0, 1, 0));
	//planeModel_256.transform = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0, 1, 0));
	//planeModel_512.transform = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0, 1, 0));
	//planeModel_1024.transform = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0, 1, 0));
	//planeModel_2048.transform = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0,1,0));

	//unsigned int textFbo = rlLoadFramebuffer();

	//planeModel_2.texture = createTextTexture("2", textFbo);
	//planeModel_4.texture = createTextTexture("4", textFbo);
	//planeModel_8.texture = createTextTexture("8", textFbo);
	//planeModel_16.texture = createTextTexture("16", textFbo);
	//planeModel_32.texture = createTextTexture("32", textFbo);
	//planeModel_64.texture = createTextTexture("64", textFbo);
	//planeModel_128.texture = createTextTexture("128", textFbo);
	//planeModel_256.texture = createTextTexture("256", textFbo);
	//planeModel_512.texture = createTextTexture("512", textFbo);
	//planeModel_1024.texture = createTextTexture("1024", textFbo);
	//planeModel_2048.texture = createTextTexture("2048", textFbo);

	//rlUnloadFramebuffer(textFbo);

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
	//models[12] = planeModel_2;
	//models[13] = planeModel_4;
	//models[14] = planeModel_8;
	//models[15] = planeModel_16;
	//models[16] = planeModel_32;
	//models[17] = planeModel_64;
	//models[18] = planeModel_128;
	//models[19] = planeModel_256;
	//models[20] = planeModel_512;
	//models[21] = planeModel_1024;
	//models[22] = planeModel_2048;
}

void Resources::unload() {
	// unload resources here...
}