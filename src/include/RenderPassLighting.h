#pragma once

#include "Engine.h"
#include "Scene.h"

class RenderPassLighting : public RenderPass {
public:
	RenderPassLighting(
		const Scene& scene,
		const Camera& camera,
		const Light& light) :
	scene(scene),
	camera(camera),
	light(light)
	{}

	void initialize(GLsizei width, GLsizei height, const void* arg = nullptr) override;
	void render(const void* arg = nullptr) const override;

private:
	const Scene& scene;
	const Camera& camera;
	const Light& light;
	GLuint shadowMapTex;
	GLuint shadowMapRes;
	GLuint gPosition;
	Shader depthShader;
	static const int ssaoKernelSize = 128;
	float ssaoKernelData[ssaoKernelSize * 3];
};