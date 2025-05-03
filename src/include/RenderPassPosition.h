#pragma once

#include "Engine.h"
#include "Scene.h"

class RenderPassPosition : public RenderPass {
public:
	RenderPassPosition(
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
	Shader positionShader;
};