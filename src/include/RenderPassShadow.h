#pragma once

#include "Engine.h"
#include "Scene.h"

class RenderPassShadow : public RenderPass {
public:
	RenderPassShadow(
		const Scene& scene,
		const Light& light) :
		scene(scene),
		light(light)
	{}

	void initialize(GLsizei width, GLsizei height, const void* arg = nullptr) override;
	void render(const void* arg = nullptr) const override;

private:
	const Scene& scene;
	const Light& light;
	Shader depthShader;
};