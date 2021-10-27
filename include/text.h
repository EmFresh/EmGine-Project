#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Shader.h"
#include "Camera.h"
#include "Transformer.h"
#include "FrameBuffer.h"
//#include "Utilities.h"

class Camera;

class Text:public Transformer
{
public:
	Text();

	Text(Text& text);
	Text(const Text& text);

	Text(cstring font);

	~Text();

	void setText(cstring text);

	std::string getText();

	void setTextSize(short s);

	void setColour(float r, float g, float b, float a = 1);

	void setColour(util::ColourRGBA colour);

	util::Vec2 getSize();

	float getWidth();

	float getHeight();



	void render(Camera* cam, Shader* shader = nullptr, bool texture = false);

	void toTexture(uint width = 720);

	GLuint getTexture();

	bool isTransparent();
	//static Character loadCharacter(cstring c, cstring font);

private:
	void create(cstring font);
	void testSize();

	/*SET SCALE*/

	void scaleBy(float scale) { Transformer::scaleBy(scale); }
	void scaleBy(float x, float y, float z) { Transformer::scaleBy(x, y, z); }
	void scale(util::Vec3 scale) { Transformer::scale(scale); }
	void scale(float scale) { Transformer::scale(scale); }
	void scale(float x, float y, float z) { Transformer::scale(x, y, z); }

	void renderInit();

	std::shared_ptr<FrameBuffer> m_texture;
	util::ColourRGBA m_colour = {};
	cstring m_font = "";
	std::string m_text = "";
	unsigned int m_length = 0;
	GLuint m_vaoID = 0, m_vboID = 0;
	util::Vec2 m_size = {};

	float m_initY = 0;
};

