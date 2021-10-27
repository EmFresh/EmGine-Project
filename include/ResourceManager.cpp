#include "ResourceManager.h"

using namespace util;

std::map<std::string, Texture2D> Texture2DCache::m_textures;
std::map<std::string, Texture3D> Texture3DCache::m_textures;
std::map<std::pair<char, std::string>, Character> CharacterCache::m_characters;
std::map<std::vector<std::string>, Shader*>  ShaderCache::m_shaders;



/// Resource Caches ///

Character& CharacterCache::getCharacter(char c, cstring font)
{
	std::string dFont = tolower((char*)font);
	auto it = m_characters.find({c, dFont});

	if(it == m_characters.end())
	{
		Character tmp = Character::loadCharacter(c, dFont.c_str());
		m_characters.insert({{c, dFont},tmp});
		return m_characters[{c, dFont}];
	}
	//printf("cashed image loaded\n");
	return it->second;
}

Texture2D& Texture2DCache::getTexture(cstring path)
{
	std::string dPath = tolower((char*)path);

	auto it = m_textures.find(dPath);

	if(it == m_textures.end())
	{
		Texture2D tmp = ImageLoader::loadImage2D(dPath.c_str());
		m_textures.insert({dPath,tmp});
		return m_textures[dPath];
	}
	//printf("cashed image loaded\n");
	return it->second;
}

Texture3D& Texture3DCache::getTextureLUT(cstring path)
{
	std::string dPath = tolower((char*)path);

	auto it = m_textures.find(dPath);

	if(it == m_textures.end())
	{
		Texture3D tmp = ImageLoader::loadImage3D(dPath.c_str());
		tmp.type = TEXTURE_TYPE3D::LUT;
		m_textures.insert({dPath,tmp});
		return m_textures[dPath];
	}
	return it->second;
}

Texture3D& Texture3DCache::getTextureCubeMap(cstring path)
{
	std::string dPath = tolower((char*)path);

	auto it = m_textures.find(dPath);

	if(it == m_textures.end())
	{
		Texture3D tmp = ImageLoader::createCubeMap(dPath.c_str());
		tmp.type = TEXTURE_TYPE3D::CUBE;
		m_textures.insert({dPath,tmp});
		return m_textures[dPath];
	}
	return it->second;
}

Shader* ShaderCache::getShader(cstring vtsh, cstring fmsh)
{
	std::string dvtsh = tolower((char*)vtsh), dfmsh = tolower((char*)fmsh);

	auto it = m_shaders.find(std::vector<std::string>{dvtsh, dfmsh});

	if(it == m_shaders.end())
	{
		Shader* tmp = new Shader;
		tmp->create(dvtsh, dfmsh);
		{
			m_shaders[{ (std::string)dvtsh, (std::string)dfmsh }] = tmp;

			return m_shaders[{ (std::string)dvtsh, (std::string)dfmsh }];
		}
		//return nullptr;
	}

	return it->second;
}



/// Resource Management ///

Texture2D& ResourceManager::getTexture2D(cstring path)
{
	return Texture2DCache::getTexture(path);
}

Texture3D& ResourceManager::getTextureLUT(cstring path)
{
	return Texture3DCache::getTextureLUT(path);
}

Texture3D& ResourceManager::getTextureCubeMap(cstring path)
{
	return Texture3DCache::getTextureCubeMap(path);
}

Shader* ResourceManager::getShader(cstring vtsh, cstring fmsh)
{
	return ShaderCache::getShader(vtsh, fmsh);
}

Character& ResourceManager::getCharacter(char c, cstring font)
{
	return CharacterCache::getCharacter(c, font);
}

