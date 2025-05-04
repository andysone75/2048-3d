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
	GLuint noiseTex;
	static const int ssaoKernelSize = 64;
	float ssaoKernelData[ssaoKernelSize * 3];

	inline void preRender() const {
		if (width != -1 && height != -1) {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glViewport(0, 0, width, height);
		}
	}

	inline void postRender() const {
		if (width != -1 && height != -1)
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};