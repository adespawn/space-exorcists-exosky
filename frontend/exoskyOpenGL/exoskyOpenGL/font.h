#ifndef FONT_H
#define FONT_H

#include <map>
#include <string>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "shader.h"

// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Horizontal offset to advance to next glyph
};

class FontRenderer {
public:
    FontRenderer(const std::string& fontPath, unsigned int screenWidth, unsigned int screenHeight);
    ~FontRenderer();

    // Renders text on the screen at the specified position
    void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);

private:
    std::map<GLchar, Character> Characters;
    unsigned int VAO, VBO;
    void LoadCharacters(FT_Face face);
};

#endif