#pragma once
#include <GL\glew.h>
#include <SOIL2\SOIL2.h>
#include <cstdlib>
#include "Texture.h"
//#include "fileIO.h"

class ImageLoader
{
public:
	static Texture2D loadImage2D(cstring file);
	static Texture3D loadImage3D(cstring LUTfile);
	static Texture3D createCubeMap(cstring path);
private:
};

