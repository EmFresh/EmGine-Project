#pragma once
#include <string>
#include "Shader.h"

class ShaderCombiner
{
public:
	ShaderCombiner();
	~ShaderCombiner();

	void combine(std::string base, std::string addition, std::string dir, std::string ending);

	std::string getCombinedShaderLocation();

private:
	std::string m_shaderLocal;
};

