#include "RenderPassLighting.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <iostream>
#include <random>
#include "glm/gtc/type_ptr.hpp"

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void RenderPassLighting::initialize(GLsizei width, GLsizei height, const void* arg) {
	RenderPass::initialize(width, height);

	shadowMapTex = static_cast<const GLuint*>(arg)[0];
	shadowMapRes = static_cast<const GLuint*>(arg)[1];
	gPosition = static_cast<const GLuint*>(arg)[2];

	if (width != -1 && height != -1) {
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLuint depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Framebuffer not complete!" << std::endl;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

	// SSAO
	uniform_real_distribution<float> randomFloats(0.0, 1.0);
	default_random_engine generator;
	vector<glm::vec3> ssaoKernel;

	for (int i = 0; i < ssaoKernelSize; i++)
	{
		glm::vec3 sample = {
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator)
		};
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = (float)i / 64.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		ssaoKernel.push_back(sample);
	}

	for (int i = 0; i < ssaoKernelSize; i++)
	{
		ssaoKernelData[i * 3] = ssaoKernel[i].x;
		ssaoKernelData[i * 3 + 1] = ssaoKernel[i].y;
		ssaoKernelData[i * 3 + 2] = ssaoKernel[i].z;
	}

	vector<glm::vec3> ssaoNoise;
	for (int i = 0; i < 16; i++)
	{
		glm::vec3 noise = {
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator) * 2.0f - 1.0f,
			0.0f
		};
		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &noiseTex);
	glBindTexture(GL_TEXTURE_2D, noiseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void RenderPassLighting::render(const void* arg) const {
	preRender();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec4 bgCol = glm::vec4(70.0f, 129.0f, 221.0f, 255.0f) / 255.0f;
	glClearColor(bgCol.r, bgCol.g, bgCol.b, bgCol.a);
	
	glEnable(GL_DEPTH_TEST);

	for (const int& objIndex : scene.getOpaqueObjects()) {
		const SceneObject& obj = scene.getObject(objIndex);
		if (!obj.isActive) continue;

		glm::mat4 model = glm::translate(glm::mat4(1), obj.position) * obj.model.transform;
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 proj = camera.getProjectionMatrixOrtho();
		glm::mat4 lightViewProj = light.getProjectionMatrixOrtho() * light.getViewMatrix(); // for shadow mapping

		Shader shader = obj.model.shader;
		shader.use();

		// common
		shader.setUniformMatrix("model", glm::value_ptr(model));
		shader.setUniformMatrix("view", glm::value_ptr(view));
		shader.setUniformMatrix("projection", glm::value_ptr(proj));

		// gradient
		shader.setUniform1f("time", static_cast<float>(glfwGetTime()));
		
		// lighting
		shader.setUniformVec3("lightDir", light.getDirection());
		shader.setUniformVec3("viewPos", camera.position);

		// shadow mapping
		shader.setUniformMatrix("lightViewProj", glm::value_ptr(lightViewProj));
		shader.setUniform1i("shadowMapResolution", shadowMapRes);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMapTex);
		shader.setUniform1i("shadowMap", 0);

		// ssao
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		shader.setUniform1i("gPosition", 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTex);
		shader.setUniform1i("texNoise", 2);

		glUniform3fv(glGetUniformLocation(shader.id, "samples"), (sizeof(ssaoKernelData) / sizeof(float) / 3), ssaoKernelData);
		shader.setUniformVec2("resolution", glm::vec2(width, height));

		// draw
		obj.model.mesh.use();
		glDrawArrays(GL_TRIANGLES, 0, obj.model.mesh.vertexCount);
	}

	glDisable(GL_DEPTH_TEST);
	postRender();
}