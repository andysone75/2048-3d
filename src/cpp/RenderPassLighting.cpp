#include "RenderPassLighting.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <iostream>
#include "glm/gtc/type_ptr.hpp"

void RenderPassLighting::initialize(GLsizei width, GLsizei height, const void* arg) {
	RenderPass::initialize(width, height);

	shadowMapTex = static_cast<const GLuint*>(arg)[0];
	shadowMapRes = static_cast<const GLuint*>(arg)[1];

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

void RenderPassLighting::render(const void* arg) const {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	glViewport(0, 0, width, height);
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
		shader.setUniformMatrix("model", glm::value_ptr(model));
		shader.setUniformMatrix("MVP", glm::value_ptr(proj * view * model));
		shader.setUniformVec3("lightDir", light.getDirection());
		shader.setUniformVec3("viewPos", camera.position);
		shader.setUniform1f("time", static_cast<float>(glfwGetTime()));

		// for shadow mapping
		shader.setUniformMatrix("lightViewProj", glm::value_ptr(lightViewProj));
		shader.setUniform1i("shadowMapResolution", shadowMapRes);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMapTex);
		shader.setUniform1i("shadowMap", 0);

		obj.model.mesh.use();
		glDrawArrays(GL_TRIANGLES, 0, obj.model.mesh.vertexCount);
	}

	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}