#pragma once

#include "Engine.h"
#include "Scene.h"

class RenderPassNormal : public RenderPass {
public:
	RenderPassNormal(
		const Scene& scene,
		const Camera& camera) :
		scene(scene),
		camera(camera)
	{
	}

	void initialize(GLsizei width, GLsizei height, const void* arg = nullptr) override;
	void render(const void* arg = nullptr) const override;

private:
	const Scene& scene;
	const Camera& camera;
	Shader normalShader;
};