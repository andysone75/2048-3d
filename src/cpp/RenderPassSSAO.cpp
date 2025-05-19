#include "RenderPassSSAO.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <iostream>
#include <random>
#include "glm/gtc/type_ptr.hpp"
#include "Utils.h"

void RenderPassSSAO::initialize(GLsizei width, GLsizei height, const void* arg) {
	RenderPass::initialize(width, height);

	gPosition = static_cast<const GLuint*>(arg)[0];
	gNormal = static_cast<const GLuint*>(arg)[1];

	glGenTextures(1, &ssaoTexture);
	glBindTexture(GL_TEXTURE_2D, ssaoTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer not complete!" << std::endl;
	}

	ssaoShader = Shader::Load("shaders/fullscreen-quad-vs.glsl", "shaders/ssao-fs.glsl", { "aPos", "aTexCoord" });

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	fullscreenQuadMesh = Mesh::GenFullscreenQuad();

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
		scale = Utils::lerp(0.1f, 1.0f, scale * scale);
		ssaoKernel.push_back(sample);
	}

	for (int i = 0; i < ssaoKernelSize; i++)
	{
		ssaoKernelData[i * 3] = ssaoKernel[i].x;
		ssaoKernelData[i * 3 + 1] = ssaoKernel[i].y;
		ssaoKernelData[i * 3 + 2] = ssaoKernel[i].z;
	}

	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &noiseTex);
	glBindTexture(GL_TEXTURE_2D, noiseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	blurShader = Shader::Load("shaders/fullscreen-quad-vs.glsl", "shaders/blur-fs.glsl", { "aPos", "aTexCoord" });
}

void RenderPassSSAO::render(const void* arg) const {
	float ssaoRadius = static_cast<const float*>(arg)[0];
	float ssaoBias = static_cast<const float*>(arg)[1];
	float ssaoPower = static_cast<const float*>(arg)[2];

	// SSAO
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);

	ssaoShader.use();
	fullscreenQuadMesh.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	ssaoShader.setUniform1i("gPosition", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	ssaoShader.setUniform1i("gNormal", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTex);
	ssaoShader.setUniform1i("noiseTex", 2);

	ssaoShader.setUniformMatrix("view", glm::value_ptr(camera.getViewMatrix()));
	ssaoShader.setUniformMatrix("projection", glm::value_ptr(camera.getProjectionMatrixOrtho()));
	ssaoShader.setUniform1f("ssaoRadius", ssaoRadius);
	ssaoShader.setUniform1f("ssaoBias", ssaoBias);
	ssaoShader.setUniform1f("ssaoPower", ssaoPower);
	ssaoShader.setUniformVec2("resolution", glm::vec2(width, height));
	glUniform3fv(glGetUniformLocation(ssaoShader.id, "samples"), (sizeof(ssaoKernelData) / sizeof(float) / 3), ssaoKernelData);

	glDrawArrays(GL_TRIANGLES, 0, fullscreenQuadMesh.vertexCount);

	// Blur
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);

	blurShader.use();
	fullscreenQuadMesh.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssaoTexture);
	blurShader.setUniform1i("ssaoInput", 0);
	blurShader.setUniformVec2("texSize", glm::vec2(width, height));
		
	glDrawArrays(GL_TRIANGLES, 0, fullscreenQuadMesh.vertexCount);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}