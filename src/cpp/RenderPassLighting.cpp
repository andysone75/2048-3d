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

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

	//glGenTextures(1, &depthTexture);
	//glBindTexture(GL_TEXTURE_2D, depthTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer not complete!" << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	depthShader = Shader::Load("shaders/depth-vs.glsl", "shaders/depth-fs.glsl");

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
}

void RenderPassLighting::render(const void* arg) const {
	float ssaoRadius = static_cast<const float*>(arg)[0];
	float ssaoBias = static_cast<const float*>(arg)[1];
	float shadingPower = static_cast<const float*>(arg)[2];
	float shadowPower = static_cast<const float*>(arg)[3];
	float ssaoPower = static_cast<const float*>(arg)[4];

	const vector<int>& objects = scene.getOpaqueObjects();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);

	// Reset
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	glm::vec4 bgCol = glm::vec4(70.0f, 129.0f, 221.0f, 255.0f) / 255.0f;
	glClearColor(bgCol.r, bgCol.g, bgCol.b, bgCol.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	// Depth pass
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	glm::mat4 viewProj = camera.getProjectionMatrixOrtho() * camera.getViewMatrix();

	depthShader.use();
	depthShader.setUniformMatrix("viewProj", glm::value_ptr(viewProj));

	for (int i = objects.size() - 1; i >= 0; i--) {
		const SceneObject& obj = scene.getObject(objects[i]);
		if (!obj.isActive) continue;
		glm::mat4 model = glm::translate(glm::mat4(1), obj.position) * obj.model.transform;
		depthShader.setUniformMatrix("model", glm::value_ptr(model));
		obj.model.mesh.use();
		glDrawArrays(GL_TRIANGLES, 0, obj.model.mesh.vertexCount);
	}

	glDisable(GL_POLYGON_OFFSET_FILL);

	// main pass
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	for (int i = objects.size() - 1; i >= 0; i--) {
		const SceneObject& obj = scene.getObject(objects[i]);
		if (!obj.isActive) continue;

		glm::mat4 model = glm::translate(glm::mat4(1), obj.position) * obj.model.transform;
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 proj = camera.getProjectionMatrixOrtho();
		glm::mat4 lightViewProj = light.getProjectionMatrixOrtho() * light.getViewMatrix();

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
		shader.setUniform1f("shadingPower", shadingPower);

		// shadow mapping
		shader.setUniformMatrix("lightViewProj", glm::value_ptr(lightViewProj));
		shader.setUniform1i("shadowMapResolution", shadowMapRes);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMapTex);
		shader.setUniform1i("shadowMap", 0);
		shader.setUniform1f("shadowPower", shadowPower);

		// ssao
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		shader.setUniform1i("gPosition", 1);

		glUniform3fv(glGetUniformLocation(shader.id, "samples"), (sizeof(ssaoKernelData) / sizeof(float) / 3), ssaoKernelData);
		shader.setUniform1f("ssaoRadius", ssaoRadius);
		shader.setUniform1f("ssaoBias", ssaoBias);
		shader.setUniform1f("ssaoPower", ssaoPower);

		// draw
		obj.model.mesh.use();
		glDrawArrays(GL_TRIANGLES, 0, obj.model.mesh.vertexCount);
	}

	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}