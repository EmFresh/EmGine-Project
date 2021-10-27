#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H 
#include "Utilities.h"

#define CHARACTER_SAMPLE_SIZE 48
struct Character
{
	GLuint    textureID;  // ID handle of the glyph texture
	util::Coord2D<int> size;   // Size of glyph
	util::Coord2D<int> bearing; // Offset from baseline to left/top of glyph
	GLuint    advance;    // Offset to advance to next glyph

	static Character loadCharacter(const char c, cstring font);

private:
	static FT_Library m_ft;
	static FT_Face m_face;
};
