#pragma once

#include "Engine.h"
#include <string>
#include <map>

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

class TextRenderer {
public:
	bool initialize(int canvasW, int canvasH);
	void draw(std::string text, float x, float y, float scale, glm::vec3 color);

private:
    Shader shader;
    unsigned int VAO, VBO;
    std::map<char, Character> characters;
};