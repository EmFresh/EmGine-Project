#include "ShaderCombiner.h"



ShaderCombiner::ShaderCombiner()
{
}


ShaderCombiner::~ShaderCombiner()
{
}
void cDir(std::string s)
{
	for (auto &a : s)
		if (a == '\\')
			a = '/';
}

void ShaderCombiner::combine(std::string base, std::string addition, std::string dir, std::string ending)
{
	cDir(base);
	cDir(addition);

	FILE *basef, *addf, *combinef;

	fopen_s(&basef, base.c_str(), "r");
	fopen_s(&addf, addition.c_str(), "r");
	fopen_s(&combinef, (m_shaderLocal =
		(dir + addition.substr(addition.find_last_of('/') + 1,
			addition.find_last_of('.') - addition.find_last_of('/') - 1) + "Combined." + ending)).c_str(), "w+");

	char  baseStr[255], addStr[255];
	//char* check;
	bool inmain = false;
	while (fgets(baseStr, 255, basef))
	{
		if (strstr(baseStr, "main()") || (( addStr[0] != '}') && inmain))
		{
			if (!inmain)
				inmain = strstr(baseStr, "main()");
			
			while (fgets(addStr, 255, addf))
				if (!strstr(addStr, "main()") && ((addStr[0] != '{' && addStr[0] != '}') && inmain))
					fputs(addStr, combinef);
				else
					break;
			fputs(baseStr, combinef);
		}
		else
			fputs(baseStr, combinef);
	}

	fclose(basef);
	fclose(addf);
	fclose(combinef);

}

std::string ShaderCombiner::getCombinedShaderLocation()
{
	return m_shaderLocal;
}
