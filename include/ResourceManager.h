#pragma once
#include <GL\glew.h>
#include <map>
#include <string>
#include <vector>
#include "ImageLoader.h"
#include "Shader.h"
#include "Character.h"


struct CharacterCache
{
	//creates a character
	static Character& getCharacter(const char, cstring);

private:
	static std::map<std::pair<char, std::string>, Character> m_characters;
};

struct Texture2DCache
{
	//creates a 2D texture
	static Texture2D& getTexture(cstring);

private:
	static std::map<std::string, Texture2D> m_textures;
};


struct Texture3DCache
{
	//loads a 3D texture
	static Texture3D& getTextureLUT(cstring);
	//creates a 3D texture
	static Texture3D& getTextureCubeMap(cstring);

private:
	static std::map<std::string, Texture3D> m_textures;
};

struct ShaderCache
{
	//creates a Shader
	static Shader* getShader(cstring, cstring);

private:
	static std::map<std::vector<std::string>, Shader*> m_shaders;
};

class ResourceManager
{
public:
	static Texture2D& getTexture2D(cstring);
	static Texture3D& getTextureLUT(cstring);
	static Texture3D& getTextureCubeMap(cstring);
	static Shader* getShader(cstring, cstring);
	static Character& getCharacter(char, cstring);

	//static GLSLCompiler& createShader(cstring, cstring);

};

