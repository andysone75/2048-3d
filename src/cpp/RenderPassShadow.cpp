#include "RenderPassShadow.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <iostream>
#include "glm/gtc/type_ptr.hpp"

void RenderPassShadow::initialize(GLsizei width, GLsizei height, const void* arg) {
	RenderPass::initialize(width, height);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// compile error on Emscripten
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

	//glDrawBuffer(GL_NONE); // compile error on Emscripten
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	depthShader = Shader::Load("shaders/depth-vert.glsl", "shaders/depth-frag.glsl");
}

void RenderPassShadow::render(const void* arg) const {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glm::mat4 view = light.getViewMatrix();
	glm::mat4 proj = light.getProjectionMatrixOrtho();

	depthShader.use();
	depthShader.setUniformMatrix("viewProj", glm::value_ptr(proj * view));

	for (const int& objIndex : scene.getOpaqueObjects()) {
		const SceneObject& obj = scene.getObject(objIndex);
		if (!obj.isActive) continue;

		glm::mat4 model = glm::translate(glm::mat4(1), obj.position) * obj.model.transform;
		depthShader.setUniformMatrix("model", glm::value_ptr(model));
		
		obj.model.mesh.use();
		glDrawArrays(GL_TRIANGLES, 0, obj.model.mesh.vertexCount);
	}

	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}