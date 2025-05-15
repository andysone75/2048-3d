#include "ImageRenderer.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

void ImageRenderer::reinitialize(int canvasW, int canvasH) {
    glm::mat4 projection = glm::ortho(0.0f, (float)canvasW, 0.0f, (float)canvasH);
    shader.use();
    shader.setUniformMatrix("projection", glm::value_ptr(projection));
}

void ImageRenderer::initialize(int canvasW, int canvasH) {
    shader = Shader::Load("shaders/image-vs.glsl", "shaders/image-fs.glsl");
    glm::mat4 projection = glm::ortho(0.0f, (float)canvasW, 0.0f, (float)canvasH);

    shader.use();
    shader.setUniformMatrix("projection", glm::value_ptr(projection));

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
}

void ImageRenderer::draw(GLuint texture, float texW, float texH, float x, float y, float scale, glm::vec3 color, float alignmentX, float alignmentY) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader.use();
    shader.setUniformVec3("color", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    x -= texW * scale * alignmentX;
    y -= texH * scale * alignmentY;

    float w = texW * scale;
    float h = texH * scale;

    float vertices[6][4] = {
        { x,     y + h,   0.0f, 0.0f },
        { x,     y,       0.0f, 1.0f },
        { x + w, y,       1.0f, 1.0f },

        { x,     y + h,   0.0f, 0.0f },
        { x + w, y,       1.0f, 1.0f },
        { x + w, y + h,   1.0f, 0.0f }
    };

    glBindTexture(GL_TEXTURE_2D, texture);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
}