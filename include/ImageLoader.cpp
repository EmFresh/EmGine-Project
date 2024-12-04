#include "ImageLoader.h"
#include <filesystem>
namespace fs = std::filesystem;
using namespace util;

Texture2D ImageLoader::loadImage2D(cstring path)
{
	Texture2D texture;

	unsigned char* image = SOIL_load_image(path, &texture.size.width, &texture.size.height, nullptr, SOIL_LOAD_RGBA);

	if(image == nullptr)
	{
		printf("Image \"%s\", returned with null pointer\n", path);
		return texture;
	}

	//texture path
	texture.path = path;

	//set the name of the texture	
	texture.name = std::string(path).substr(std::string(path).find_last_of('/') + 1);


	//Bind texture to model
	glGenTextures(1, &texture.id);
	texture.bindTexture();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.size.width, texture.size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	texture.unbindTexture();


	//texture.width = width;
	//texture.height = height;
	return texture;
}

#include <vector>
#include <fstream>
Texture3D ImageLoader::loadImage3D(cstring LUTfile)
{
	Texture3D texture = Texture3D(TEXTURE_TYPE3D::LUT);

	std::vector<Vec3> LUT{};

	//LUTfile = "Texture/CUSTOM.cube";
	std::ifstream LUTfile2(LUTfile);

	if(!LUTfile2.is_open())
	{
		printf("Image \"%s\", returned with null pointer\n", LUTfile);
		return texture;
	}


	std::string LUTline;
	while((getline(LUTfile2, LUTline), !LUTfile2.eof()))
	{

		if(LUTline.empty()) continue;
		if(LUTline[0] == ('#'))continue;

		if(strstr(LUTline.c_str(), "LUT_3D_SIZE"))
			sscanf_s(LUTline.c_str(), "LUT_3D_SIZE %d", &texture.lutSize);


		float r, g, b;
		if(sscanf_s(LUTline.c_str(), "%f %f %f", &r, &g, &b) == 3) LUT.push_back({r, g, b});
	}
	glEnable(GL_TEXTURE_3D);

	glGenTextures(1, &texture.id);
	texture.bindTexture();


	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, texture.lutSize, texture.lutSize, texture.lutSize, 0, GL_RGB, GL_FLOAT, LUT.data());

	texture.unbindTexture();
	glDisable(GL_TEXTURE_3D);

	LUT.clear();
	return texture;
}

/// <summary>
/// Images must have a _left, _right, _top, _bottom, _front, _backat the end of iiimage to be regestered
/// </summary>
/// <param name="SBpath">the Sky Box Path</param>
/// <returns>A Texture3D with all the information. else a default Texture3D</returns>
Texture3D ImageLoader::createCubeMap(cstring SBpath)
{
	Texture3D texture = Texture3D(TEXTURE_TYPE3D::CUBE);
	std::string pos[6]{"_right.", "_left.", "_top.", "_bottom.", "_front.", "_back."};
	unsigned char* data;

	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &texture.id);

	texture.bindTexture();

	for(unsigned a = 0; a < 6; ++a)
		for(auto& b : fs::directory_iterator(SBpath))
		{
			std::string path = b.path().generic_string();
			std::string tmp;


			tmp = util::tolower(path);

			if(tmp.find(pos[a]) != std::wstring::npos)
			{

				data = SOIL_load_image(tmp.c_str(), &texture.size.width, &texture.size.height, nullptr, SOIL_LOAD_RGB);
				if(data)
					glTexImage2D(
						GL_TEXTURE_CUBE_MAP_POSITIVE_X + a,
						0, GL_RGBA, texture.size.width, texture.size.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
					);
				printf("%s\n\n", SOIL_last_result());
				SOIL_free_image_data(data);
				break;
			}
		}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	texture.unbindTexture();
	glDisable(GL_TEXTURE_CUBE_MAP);


	return texture;
}
