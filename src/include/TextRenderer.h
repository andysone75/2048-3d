#pragma once

#include "Engine.h"
#include <string>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

class TextRenderer {
public:
	bool initialize(int canvasW, int canvasH);
	void draw(std::string text, float x, float y, float scale, glm::vec3 color, float alignmentX = 0.0f);

private:
    Shader shader;
    unsigned int VAO, VBO;
    std::map<char32_t, Character> characters;

    void generateGlyphTexture(FT_Face face, char32_t codepoint);
};