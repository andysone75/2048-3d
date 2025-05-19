#pragma once

#include "Engine.h"
#include "Scene.h"

class RenderPassSSAO : public RenderPass {
public:
	RenderPassSSAO(const Camera& _camera) : camera(_camera) {}

	void initialize(GLsizei width, GLsizei height, const void* arg = nullptr) override;
	void render(const void* arg = nullptr) const override;

private:
	const Camera& camera;
	GLuint gPosition;
	GLuint gNormal;
	GLuint noiseTex;
	Shader ssaoShader;
	Mesh fullscreenQuadMesh;
	static const int ssaoKernelSize = 128;
	float ssaoKernelData[ssaoKernelSize * 3];
	GLuint ssaoBlurFBO;
	GLuint ssaoTexture;
	Shader blurShader;
};