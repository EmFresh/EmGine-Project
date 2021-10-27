#pragma once
#include <cstdio>
#include "Utilities.h"
class fileIO
{
public:
	//No longer needed but good resource for how to read in image files in C binary
	static int readFileToBuffer(cstring path,unsigned char * buffer);
};

