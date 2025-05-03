#pragma once

#include "Engine.h"
#include "Scene.h"

class RenderPassSSAO : public RenderPass {
public:
	RenderPassSSAO(
		const Camera& camera) :
		camera(camera)
	{
	}

	void initialize(GLsizei width, GLsizei height, const void* arg = nullptr) override;
	void render(const void* arg = nullptr) const override;

private:
	const Camera& camera;
	Shader ssaoShader;
	Mesh fullscreenQuadMesh;
	GLuint positionMap;
	GLuint normalMap;
};