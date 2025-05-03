#include "RenderPassSSAO.h"

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

void RenderPassSSAO::initialize(GLsizei width, GLsizei height, const void* arg) {
    RenderPass::initialize(width, height);

    positionMap = static_cast<const GLuint*>(arg)[0];
    normalMap = static_cast<const GLuint*>(arg)[1];

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

    uniform_real_distribution<float> randomFloats(0.0, 1.0);
    default_random_engine generator;
    vector<glm::vec3> ssaoKernel;
    const int ssaoKernelSize = 64;
    float ssaoKernelData[ssaoKernelSize * 3];

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

    GLuint noiseTex;
    glGenTextures(1, &noiseTex);
    glBindTexture(GL_TEXTURE_2D, noiseTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    ssaoShader = Shader::Load("shaders/fullscreen-quad-vs.glsl", "shaders/ssao.glsl");
    ssaoShader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noiseTex);
    ssaoShader.setUniform1i("texNoise", 0);

    glUniform3fv(glGetUniformLocation(ssaoShader.id, "samples"), (sizeof(ssaoKernelData) / sizeof(float) / 3), ssaoKernelData);
    ssaoShader.setUniformVec2("resolution", glm::vec2(width, height));

	fullscreenQuadMesh = Mesh::GenFullscreenQuad();

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPassSSAO::render(const void* arg) const {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ssaoShader.use();
    ssaoShader.setUniformMatrix("projection", glm::value_ptr(camera.getProjectionMatrixOrtho()));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, positionMap);
    ssaoShader.setUniform1i("gPosition", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    ssaoShader.setUniform1i("gNormal", 1);

	fullscreenQuadMesh.use();
	glDrawArrays(GL_TRIANGLES, 0, fullscreenQuadMesh.vertexCount);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}