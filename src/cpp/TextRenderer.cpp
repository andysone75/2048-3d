#include "TextRenderer.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cstdint>

#define FONT_PATH "fonts/Nunito-Black.ttf"

std::u32string utf8_to_utf32(const std::string& utf8) {
    std::u32string utf32;
    size_t i = 0;
    while (i < utf8.size()) {
        uint8_t c = utf8[i++];
        char32_t codepoint = 0;

        if ((c & 0x80) == 0x00) {  // 1-byte character (ASCII)
            codepoint = c;
        }
        else if ((c & 0xE0) == 0xC0) {  // 2-byte character
            codepoint = (c & 0x1F) << 6;
            codepoint |= (utf8[i++] & 0x3F);
        }
        else if ((c & 0xF0) == 0xE0) {  // 3-byte character
            codepoint = (c & 0x0F) << 12;
            codepoint |= (utf8[i++] & 0x3F) << 6;
            codepoint |= (utf8[i++] & 0x3F);
        }
        else if ((c & 0xF8) == 0xF0) {  // 4-byte character
            codepoint = (c & 0x07) << 18;
            codepoint |= (utf8[i++] & 0x3F) << 12;
            codepoint |= (utf8[i++] & 0x3F) << 6;
            codepoint |= (utf8[i++] & 0x3F);
        }
        else {
            throw std::runtime_error("Invalid UTF-8");
        }
        utf32.push_back(codepoint);
    }
    return utf32;
}

bool TextRenderer::initialize(int canvasW, int canvasH) {
    shader = Shader::Load("shaders/text-vs.glsl", "shaders/text-fs.glsl", {});
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

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft, FONT_PATH, 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return false;
    }

    if (FT_Set_Pixel_Sizes(face, 0, 48)) {
        std::cerr << "Failed to set pixel size!" << std::endl;
        return false;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    // load ASCII (0-127)
    for (char32_t c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        generateGlyphTexture(face, c);
    }

    // load Cyrillic (U+0400 - U+04FF)
    for (char32_t c = 0x0400; c < 0x04FF; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        generateGlyphTexture(face, c);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return true;
}

void TextRenderer::draw(std::string text, float x, float y, float scale, glm::vec3 color, float alignmentX) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // activate corresponding render state	
    shader.use();
    shader.setUniformVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    float totalWidth = 0.0f;
    //std::string::const_iterator c;
    std::u32string utf32 = utf8_to_utf32(text);

    for (char32_t c : utf32) {
        if (characters.find(c) == characters.end())
            continue;

        Character ch = characters.at(c);
        totalWidth += (ch.Advance >> 6) * scale;
    }

    x -= totalWidth * alignmentX;

    for (char32_t c : utf32) {
        if (characters.find(c) == characters.end())
            continue;

        Character ch = characters.at(c);

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
}

void TextRenderer::generateGlyphTexture(FT_Face face, char32_t codepoint) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_LUMINANCE,
        face->glyph->bitmap.width,
        face->glyph->bitmap.rows,
        0,
        GL_LUMINANCE,
        GL_UNSIGNED_BYTE,
        face->glyph->bitmap.buffer
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character character = {
        texture,
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        static_cast<unsigned int>(face->glyph->advance.x)
    };

    characters.insert({ codepoint, character });
}