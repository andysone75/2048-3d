#pragma once

#include "Engine.h"
#include <string>

class ImageRenderer {
public:
    void initialize(int canvasW, int canvasH);
    void reinitialize(int canvasW, int canvasH);
    void draw(GLuint texture, float texW, float texH, float x, float y, glm::vec2 scale, glm::vec4 color, float alignmentX = 0.0f, float alignmentY = 0.0f) const;

private:
    Shader shader;
    unsigned int VAO, VBO;
};