#pragma once

#include "Engine.h"
#include <string>

class ImageRenderer {
public:
    void initialize(int canvasW, int canvasH);
    void draw(GLuint texture, float texW, float texH, float x, float y, float scale, glm::vec3 color, float alignmentX = 0.0f, float alignmentY = 0.0f);

private:
    Shader shader;
    unsigned int VAO, VBO;
};