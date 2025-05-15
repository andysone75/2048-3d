#pragma once

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

#define WHITE Color{ 255, 255, 255, 255 }
#define GET_UNIFORM_LOCATION(programID, uniformName) glGetUniformLocation(programID, uniformName)


struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

struct Mesh {
	GLuint vao;
    int vertexCount;

	static Mesh GenCube(Color color = WHITE);
    static Mesh GenPlane(float width, float height, int slices);
    static Mesh GenFullscreenQuad();
	inline void use() const { glBindVertexArray(vao); }
};

struct Shader {
	GLuint id;

    static Shader Load(const std::string& vsPath, const std::string& fsPath);
    static Shader Load(const std::string& vsPath, const std::string& fsPath, const std::vector<std::string>& attribNames);

	inline void use() const { glUseProgram(id); }
    inline void unload() const { glDeleteProgram(id); }
	
	inline void setUniformMatrix(const GLchar* uniformName, const GLfloat* value, int count = 1) const { glUniformMatrix4fv(GET_UNIFORM_LOCATION(id, uniformName), count, GL_FALSE, value); }
	inline void setUniform1f(const GLchar* uniformName, float value) const { glUniform1f(GET_UNIFORM_LOCATION(id, uniformName), value); }
	inline void setUniform1i(const GLchar* uniformName, int value) const { glUniform1i(GET_UNIFORM_LOCATION(id, uniformName), value); }
	inline void setUniformVec2(const GLchar* uniformName, const glm::vec2& value, int count = 1) const { glUniform2fv(GET_UNIFORM_LOCATION(id, uniformName), count, &value[0]); }
	inline void setUniformVec3(const GLchar* uniformName, const glm::vec3& value, int count = 1) const {glUniform3fv(GET_UNIFORM_LOCATION(id, uniformName), count, &value[0]); }
	inline void setUniformVec4(const GLchar* uniformName, const glm::vec4& value, int count = 1) const {glUniform4fv(GET_UNIFORM_LOCATION(id, uniformName), count, &value[0]); }
};

struct Model {
	Mesh mesh;
	Shader shader;
	glm::mat4 transform;

	static Model Load(const Mesh& mesh);
};

struct Camera {
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    float fovY;
    float aspect;
    float nearZ;
    float farZ;

    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 target = glm::vec3(0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float fovY = 45.0f,
        float aspect = 4.0f / 3.0f,
        float nearZ = 0.1f,
        float farZ = 100.0f
    )
        : position(position), target(target), up(up),
        fovY(fovY), aspect(aspect), nearZ(nearZ), farZ(farZ)
    {
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, target, up);
    }

    glm::mat4 getProjectionMatrix() const {
        return glm::perspective(glm::radians(fovY), aspect, nearZ, farZ);
    }

    glm::mat4 getProjectionMatrixOrtho() const {
        float halfH = fovY * 0.5f;
        float halfW = halfH * aspect;
        return glm::ortho(-halfW, halfW, -halfH, halfH, nearZ, farZ);
    }
};

struct Light : Camera {
    inline glm::vec3 getDirection() const {
        return target - position;
    }
};

class RenderPass {
public:
    virtual void initialize(GLsizei width, GLsizei height, const void* arg = nullptr) {
        this->width = width;
        this->height = height;
    };

    virtual void render(const void* arg = nullptr) const = 0;
    virtual void unload() {};

    inline const GLuint getTexture() const { return texture; }
    inline const GLsizei getWidth() const { return width; }
    inline const GLsizei getHeight() const { return height; }

protected:
    GLsizei width, height;
    GLuint texture;
    GLuint fbo;
};