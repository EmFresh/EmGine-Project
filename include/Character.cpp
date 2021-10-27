#include "Character.h"


FT_Library Character::m_ft;
FT_Face Character::m_face ;

Character Character::loadCharacter(const char c, cstring font)
{
	

	static Character tmp;

	if(FT_Init_FreeType(&m_ft))
		printf("ERROR::FREETYPE: Could not init FreeType Library\n");

	if(FT_New_Face(m_ft, "fonts/arial.ttf", 0, &m_face))
		printf("ERROR::FREETYPE: Failed to load font \"%s\"\n", font);

	FT_Set_Pixel_Sizes(m_face, 0, CHARACTER_SAMPLE_SIZE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	
	// Load character glyph 
	if(FT_Load_Char(m_face, c, FT_LOAD_RENDER))
	{
		printf("ERROR::FREETYTPE: Failed to load Glyph\n");
		return tmp;
	}

	// Generate texture
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		(m_face)->glyph->bitmap.width,
		(m_face)->glyph->bitmap.rows,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		(m_face)->glyph->bitmap.buffer
	);
	// Set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	//FT_Done_Face(m_face);
	//FT_Done_FreeType(m_ft);

	return	tmp = {
		   texture,
		   util::Coord2D<int>((m_face)->glyph->bitmap.width, (m_face)->glyph->bitmap.rows),
		   util::Coord2D<int>((m_face)->glyph->bitmap_left, (m_face)->glyph->bitmap_top),
		   (GLuint)(m_face)->glyph->advance.x
	};

}
