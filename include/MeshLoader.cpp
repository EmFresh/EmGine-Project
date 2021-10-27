#include "MeshLoader.h"
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>

using namespace util;
namespace fs = std::filesystem;
using std::string;
using std::vector;

std::vector<std::shared_ptr<Mesh>> MeshLoader::m_meshes;
std::vector<std::pair<string, std::vector<Texture2D>>> MeshLoader::m_textures;
cstring modelDir = "";


/// <summary>
/// Find a string and return a string from where the string was found. 
/// If no string is found this function returns "".
/// </summary>
/// <param name="str: "> string to search</param>
/// <param name="find: "> string to be found</param>
/// <returns></returns>
string substr(cstring str, cstring find)
{
	unsigned count = (unsigned)strlen(str);
	for(; count >= 0; --count)
		if(strstr(str + count, find))
			break;

	std::unique_ptr<char> tmp(new char[count = count + 2]);
	memset(tmp.get(), 0, count);
	memmove_s(tmp.get(), count, str, count - 1);
	return string(tmp.get());
}

std::vector<std::shared_ptr<Mesh>> MeshLoader::loadMesh(string path)
{
	string tmp(modelDir + string("BIN/")), tmp2 = tmp;
	if(!fs::exists(tmp))
	{
		tmp = "mkdir \"" + tmp + "\"";
		system(tmp.c_str());
	}

	cleanup();
	if(!load(modelDir + path, tmp2))return std::vector<std::shared_ptr<Mesh>>();

	for(auto& a : m_meshes)
		a->init();


	//for(auto& a : m_indicieData)
	//	a.second.clear();
	//m_indicieData.clear();

	return m_meshes;
}

void MeshLoader::setDirectory(cstring dir)
{
	modelDir = dir;
}

#include <chrono>
#include <iomanip>
bool MeshLoader::load(string path, string binPath)
{

	if(!strstr(path.c_str(), ".obj"))return false;

#pragma region Local Variables
	//std::vector < std::pair<string, std::vector<Coord3D<unsigned>[3]>>> faces;
	std::fstream bin;

	cDir((char*)path.c_str());

	//path.insert(path.begin(), '\"');
	//path.insert(path.end(), '\"');

	std::vector<Vec3>verts;
	std::vector<UV>uvs;
	std::vector<Vec3>norms;

	std::map<Indicie, GLuint> indicieMap;
	unsigned indicieCount = 0;

	bool initFace = true;
#pragma endregion

	//convert to bin file path
	binPath = binPath + path.substr(strlen(modelDir), path.find_first_of('.') - strlen(modelDir) + 1) + "bin";
	auto lasttime = fs::exists(binPath) ? fs::last_write_time(binPath) : fs::file_time_type();
	auto lasttime2 = fs::last_write_time(path);
	if(!fs::exists(binPath) || (lasttime < lasttime2))
	{
		//	puts("Load from File\n");

	#pragma region Open Meshes 

		if(!fs::exists(path))
		{
			printf("Could not find file: \"%s\"\n\n", path.c_str());
			return false;
		}

		loadMaterials(path.c_str());


		std::string inputBuff;

		//	char* MeshCheck = nullptr;
		std::ifstream f(path);


		while(std::getline(f, inputBuff))
		{
			//checks for comments
			if(strchr(inputBuff.c_str(), '#'))
				inputBuff = inputBuff.substr(0, inputBuff.find('#'));
			if(strstr(inputBuff.c_str(), "mtllib"))
				//IDK what this is
				continue;
			if(strstr(inputBuff.c_str(), "usemtl "))
			{

				char str[CHAR_BUFF_SIZE];
				sscanf_s(inputBuff.c_str(), "usemtl %s", str, CHAR_BUFF_SIZE);
				m_meshes.back()->matNames.push_back(str);

				for(auto& a : m_textures)
					if(strcmp(str, a.first.c_str()) == 0)
					{
						auto& tmp = a.second;//refernce original
						((vector<Texture2D>)m_meshes.back()->getTextures()).insert(m_meshes.back()->getTextures().end(), tmp.begin(), tmp.end());
						((vector<Texture2D>)m_meshes.back()->getReplaceTextures()).resize(m_meshes.back()->getTextures().size());
					}

				//	indicieMap.clear();
				//	indicieCount = 0;

			}

			else if(strstr(inputBuff.c_str(), "vt "))
			{
				//UV data

				UV tmp;
				sscanf_s(inputBuff.c_str(), "vt %f %f", &tmp.u, &tmp.v);
				uvs.push_back(tmp);
			}

			else if(strstr(inputBuff.c_str(), "vn "))
			{
				//Normal data
				Vec3 tmp;
				sscanf_s(inputBuff.c_str(), "vn %f %f %f", &tmp.x, &tmp.y, &tmp.z);
				norms.push_back(tmp);
			}

			else if(strstr(inputBuff.c_str(), "o "))
			{
				char str[CHAR_BUFF_SIZE];
				sscanf_s(inputBuff.c_str(), "o %s", str, CHAR_BUFF_SIZE);

				//object
				m_meshes.push_back(std::shared_ptr<Mesh>(new Mesh));
				m_meshes.back()->meshName = str;
				initFace = true;
				indicieMap.clear();
				indicieCount = 0;
			}

			else if(strstr(inputBuff.c_str(), "s "))
				continue;
			else if(strstr(inputBuff.c_str(), "f "))//Collect Face Data
			{
				//Face data

				Indicie tmp[3];

				char check = 0;
				unsigned counter = 0, count = 0;
				while(bool(check = inputBuff[counter++]))
					count += (check == '/');

				count = unsigned((float)count * .5f);
				string	faceTmp[2][2]
				{{ " %d/%d/%d"," %*d/%*d/%*d" },
					{ " %d//%d"," %*d//%*d" }};

				std::vector<string > format = std::vector<string>(count);
				string formatStr;
				std::function<void(int)> reformat = [&](int type)
				{
					for(unsigned a = 0; a < count; a++)
						if(a < 3)//read 3 indicies
							format[a] = faceTmp[type][0];
						else
							format[a] = faceTmp[type][1];
				};
				short type = 0;
				reformat(type);
				formatStr = "f";

				for(unsigned a = 0; a < count; a++)
					formatStr += format[a];


				memset(tmp, 0, sizeof(tmp));

				if(9 != sscanf_s(inputBuff.c_str(), formatStr.c_str(),
				   //Coord         UV        Normal
				   &tmp[0][0], &tmp[0][1], &tmp[0][2],
				   &tmp[1][0], &tmp[1][1], &tmp[1][2],
				   &tmp[2][0], &tmp[2][1], &tmp[2][2]))
				{
					reformat(++type);
					formatStr = "f";
					for(unsigned a = 0; a < count; a++)
						formatStr += format[a];

					sscanf_s(inputBuff.c_str(), formatStr.c_str(),
							 //Coord        Normal
							 &tmp[0][0], &tmp[0][2],
							 &tmp[1][0], &tmp[1][2],
							 &tmp[2][0], &tmp[2][2]);
				}

				for(unsigned b = 0; b < 3; ++b)
				{
					bool inuv = (bool)tmp[b].uv;
					tmp[b].correct();
					auto thing = indicieMap.find(tmp[b]);

					if(thing == indicieMap.end())//new indicie
					{
						indicieMap[tmp[b]] = indicieCount;
						Vertex3D tmp2;
						tmp2.coord = verts[tmp[b].coord];
						tmp2.norm = norms[tmp[b].norm];
						if(inuv)
							tmp2.uv = uvs[tmp[b].uv];

						((vector<Vertex3D>)m_meshes.back()->getUnpackedData()).push_back(tmp2);
						((vector<uint>)m_meshes.back()->getIndicieData()).push_back(indicieCount++);
					}
					else//reacouring indicie
						((vector<uint>)m_meshes.back()->getIndicieData()).push_back(indicieMap[tmp[b]]);
				}

				//tmp.correct();
				//m_unpackedData.push_back(Vertex3D());
				//m_unpackedData.back().coord = verts[tmp[0][0]];
				//m_indicieData.back().second.push_back(tmp);

				for(unsigned a = 1; a < count - 2; a++)
				{
					formatStr = "f";
					swap(format[a], format[(a + 2)]);
					for(unsigned i = 0; i < count; i++)
						formatStr += format[i];
					if(type == 0)
						sscanf_s(inputBuff.c_str(), formatStr.c_str(),
								 //Coord         UV        Normal
								 &tmp[0][0], &tmp[0][1], &tmp[0][2],
								 &tmp[1][0], &tmp[1][1], &tmp[1][2],
								 &tmp[2][0], &tmp[2][1], &tmp[2][2]);
					else
						sscanf_s(inputBuff.c_str(), formatStr.c_str(),
								 //Coord       Normal
								 &tmp[0][0], &tmp[0][2],
								 &tmp[1][0], &tmp[1][2],
								 &tmp[2][0], &tmp[2][2]);

					for(unsigned b = 0; b < 3; ++b)
					{
						if(type == 1)//make sure the UV index is zero (may randomly become a value for no reason)
							tmp[b][1] = 0;

						bool inuv = (bool)tmp[b].uv;

						tmp[b].correct();
						auto thing = indicieMap.find(tmp[b]);

						if(thing == indicieMap.end())//new indicie
						{
							indicieMap[tmp[b]] = indicieCount;
							Vertex3D tmp2;
							tmp2.coord = verts[tmp[b].coord];
							tmp2.norm = norms[tmp[b].norm];
							if(inuv)
								tmp2.uv = uvs[tmp[b].uv];

							((vector<Vertex3D>)m_meshes.back()->getUnpackedData()).push_back(tmp2);
							((vector<uint>)m_meshes.back()->getIndicieData()).push_back(indicieCount++);
						}
						else//reacouring indicie
							((vector<uint>)	m_meshes.back()->getIndicieData()).push_back(indicieMap[tmp[b]]);
					}


				}

			}

			else if(strstr(inputBuff.c_str(), "v "))//Collects Vertex Data
			{
				//Vertex Data

				Vec3 tmp;
				sscanf_s(inputBuff.c_str(), "v %f %f %f", &tmp.x, &tmp.y, &tmp.z);
				verts.push_back(tmp);
				if(initFace)
				{
					m_meshes.back()->front = m_meshes.back()->back =
						m_meshes.back()->left = m_meshes.back()->right =
						m_meshes.back()->top = m_meshes.back()->bottom = tmp;

					initFace = false;
				}
				else
				{
					m_meshes.back()->front = tmp.z > m_meshes.back()->front.z ? tmp : m_meshes.back()->front;
					m_meshes.back()->back = tmp.z < m_meshes.back()->back.z ? tmp : m_meshes.back()->back;
					m_meshes.back()->left = tmp.x < m_meshes.back()->left.x ? tmp : m_meshes.back()->left;
					m_meshes.back()->right = tmp.x > m_meshes.back()->right.x ? tmp : m_meshes.back()->right;
					m_meshes.back()->top = tmp.y > m_meshes.back()->top.y ? tmp : m_meshes.back()->top;
					m_meshes.back()->bottom = tmp.y < m_meshes.back()->bottom.y ? tmp : m_meshes.back()->bottom;
				}
			}
		}

		f.close();
	#pragma endregion

	#pragma region PACK DATA

		string binFile = binPath;
		//open bin file
		//fopen_s(&bin, (binFile).c_str(), "wb");

		//create Directory (you need to create the directory before making a file)
		{
			string tmp = ("mkdir \"" + binFile.substr(0, binFile.find_last_of('/')) + "\"").c_str();
			if(!fs::exists(tmp))
				system(tmp.c_str());
		}

		bin.open(binFile, std::ios::out | std::ios::binary);//deletes file contents before writing (I think... in the end it doesn't mater)


		unsigned meshSize = (uint)m_meshes.size(), dataSize = 0;

		//Ammount of Meshes (size of int)
		bin.write((char*)&meshSize, sizeof(meshSize));

		for(unsigned int a = 0; a < meshSize; a++)
		{
			//Mesh Name (size of name length + 1 then string)
			dataSize = (uint)m_meshes[a]->meshName.length() + 1;
			bin.write((char*)&dataSize, sizeof(unsigned));
			bin.write((char*)m_meshes[a]->meshName.data(), dataSize);

			//Amount of Vertecies (size of unsigned)
			dataSize = (uint)m_meshes[a]->getUnpackedData().size();
			bin.write((char*)&dataSize, sizeof(unsigned));

			//Store Vertex Data (size of vertex3D * vertex amount)
			bin.write((char*)m_meshes[a]->getUnpackedData().data(), sizeof(Vertex3D) * dataSize);

			//Amount of Indicies (size of unsigned)
			dataSize = (uint)m_meshes[a]->getIndicieData().size();
			bin.write((char*)&dataSize, sizeof(unsigned));

			//Indicie Data (size of unsigned * indice amount)
			bin.write((char*)m_meshes[a]->getIndicieData().data(), sizeof(unsigned) * dataSize);

			//Amount of Materials (size of unsigned)
			dataSize = (uint)m_meshes[a]->matNames.size();
			bin.write((char*)&dataSize, sizeof(unsigned));

			//Material Names
			for(auto& b : m_meshes[a]->matNames)
			{
				dataSize = (uint)b.length() + 1;
				bin.write((char*)&dataSize, sizeof(uint));
				bin.write((char*)b.data(), dataSize);
			}
			//m_meshes[a]->matNames.clear();

			//Bounds Data 
			bin.write((char*)&m_meshes[a]->top/*first bound*/, sizeof(Vec3) * 6);
		}
		bin.close();
	#pragma endregion

	}
	else
	{
		//	puts("Load from BIN\n");

		if(!fs::exists(path))
		{
			printf("Could not find file: \"%s\"\n\n", path.c_str());
			return false;
		}
		loadMaterials(path.c_str());

	#pragma region UNPACK DATA

		string binFile = binPath;
		//open bin file
		//fopen_s(&bin, (binFile).c_str(), "rb");
		bin.open(binFile, std::ios::in | std::ios::binary);//deletes file contents before writing

		unsigned meshSize = 0, dataSize = 0;

		//ammount of meshes
		bin.read((char*)&meshSize, sizeof(unsigned));

		//m_meshes[0]->getUnpackedData();

		for(unsigned int a = 0; a < meshSize; a++)
		{
			m_meshes.push_back(std::shared_ptr<Mesh>(new Mesh));

			//Mesh Name (size of name length + 1 then string)
			bin.read((char*)&dataSize, sizeof(unsigned));
			cstring name = new char[dataSize];
			bin.read((char*)(void*)name, dataSize);
			m_meshes[a]->meshName = name;

			//Amount of Vertecies (size of unsigned)
			bin.read((char*)&dataSize, sizeof(unsigned));
			m_meshes[a]->getUnpackedData().resize(dataSize);

			//Store Vertex Data (size of vertex3D * vertex amount)
			bin.read((char*)m_meshes[a]->getUnpackedData().data(), sizeof(Vertex3D) * dataSize);

			//Amount of Indicies (size of unsigned)
			bin.read((char*)&dataSize, sizeof(unsigned));
			m_meshes[a]->getIndicieData().resize(dataSize);

			//Indicie Data (size of unsigned * indice amount)
			bin.read((char*)m_meshes[a]->getIndicieData().data(), sizeof(unsigned) * dataSize);

			//Amount of Materials (size of unsigned)
			bin.read((char*)&dataSize, sizeof(unsigned));
			m_meshes[a]->matNames.resize(dataSize);

			//Material Names
			for(unsigned b = 0; b < dataSize; ++b)
			{
				static unsigned size;
				bin.read((char*)&size, sizeof(unsigned));
				std::unique_ptr<char>str(new char[size]);
				bin.read((char*)str.get(), size);

				for(auto& c : m_textures)
					if(std::string(str.get()) == c.first)
					{
						auto& tmp = c.second;//refernce original
						m_meshes[a]->getTextures().insert(m_meshes[a]->getTextures().end(), tmp.begin(), tmp.end());
						m_meshes[a]->getReplaceTextures().resize(m_meshes[a]->getTextures().size());
					}
			}

			//bounds data
			bin.read((char*)&m_meshes[a]->top, sizeof(Vec3) * 6);


		}
	#pragma endregion

		bin.close();
	}


	return true;
}

void MeshLoader::cleanup()
{
	//for(auto a : m_meshes)
	//	if(a)
	//		delete a;
	m_meshes.clear();
	m_textures.clear();
}

void MeshLoader::loadMaterials(cstring path)
{
	FILE* f;
	cDir((char*)path);
	fopen_s(&f, path, "r");

	if(!f)
	{
		printf("unknown material\n");
		return;
	}

	char str[CHAR_BUFF_SIZE];
	char* MeshCheck;
	string tmpDir;
	while(MeshCheck = fgets(str, CHAR_BUFF_SIZE, f),
		  //this part takes out the '\n' from the string
		  (str == nullptr ? "" : (str[strlen(str) - 1] = (str[strlen(str) - 1] == '\n' ? '\0' : str[strlen(str) - 1]), str)),
		  MeshCheck)
		if(strstr(str, "mtllib"))
		{
			//find material
			char str2[CHAR_BUFF_SIZE];
			memcpy_s(str2, CHAR_BUFF_SIZE, str + 7, CHAR_BUFF_SIZE - 7);

			//find material location
			char tmp[CHAR_BUFF_SIZE];
			unsigned count = unsigned(strrchr(path, '/') - path) + 1;
			memset(tmp, 0, CHAR_BUFF_SIZE);
			memmove_s(tmp, CHAR_BUFF_SIZE, path, count);
			tmpDir = string(tmp) + string(str2);

			//set material path
			path = tmpDir.c_str();
			break;
		}

	fclose(f);

	fopen_s(&f, path, "r");


	if(f)
		while(MeshCheck = fgets(str, CHAR_BUFF_SIZE, f),
			  //this part takes out the '\n' from the string
			  (str == nullptr ? "" : (str[strlen(str) - 1] = (str[strlen(str) - 1] == '\n' ? '\0' : str[strlen(str) - 1]), str)),
			  MeshCheck)
		{

			if(strchr(str, '#'))
				memset(strchr(str, '#'), '\0', sizeof(char));

			if(strstr(str, "newmtl "))
			{
				char str2[CHAR_BUFF_SIZE];
				sscanf_s(str, "newmtl %s", str2, CHAR_BUFF_SIZE);
				m_textures.push_back({string(str2), std::vector<Texture2D>()});
				//m_replaceTex.push_back(std::vector<GLuint>());
				if(strstr(str, "None"))
					return;
			}

			else if(strstr(str, "illum "))
			{
				continue;
			}

			else if(strstr(str, "map_Kd "))
			{
				char str2[CHAR_BUFF_SIZE];
				//sscanf_s(str, "map_Kd %s", &str2, (unsigned)_countof(str2));
				memcpy_s(str2, CHAR_BUFF_SIZE, str + 7, CHAR_BUFF_SIZE - 7);

				//path.resizeDepth(path.c_str());
				cDir(str2);
				string tmpStr(substr(path, "/") + str2);
				m_textures.back().second.push_back(ResourceManager::getTexture2D(tmpStr.c_str()));
				m_textures.back().second.back().type = TEXTURE_TYPE2D::DIFFUSE;
				m_textures.back().second.back().name = m_textures.back().first.c_str();
				//m_replaceTex.back().push_back(0);

			}

			else if(strstr(str, "map_Ks "))
			{
				char str2[CHAR_BUFF_SIZE];
				memcpy_s(str2, CHAR_BUFF_SIZE, str + 7, CHAR_BUFF_SIZE - 7);

				//path.resizeDepth(path.c_str());
				cDir(str2);
				string tmpStr(substr(path, "/") + str2);
				m_textures.back().second.push_back(ResourceManager::getTexture2D(tmpStr.c_str()));
				m_textures.back().second.back().type = TEXTURE_TYPE2D::SPECULAR;
				m_textures.back().second.back().name = m_textures.back().first.c_str();
				//m_replaceTex.back().push_back(0);

			}

			else if(strstr(str, "Ns "))
				continue;

			else if(strstr(str, "Ka "))
			{
				float a[3];
				sscanf_s(str, "Ka %f %f %f", &a[0], &a[1], &a[2]);
				for(auto& b : m_textures.back().second)
					b.colour.a = (GLubyte)(255 * a[0] * a[1] * a[2]);
			}

			else if(strstr(str, "Kd "))
			{
				float r, g, b;
				sscanf_s(str, "Kd %f %f %f", &r, &g, &b);
				for(auto& a : m_textures.back().second)
					if(a.type == TEXTURE_TYPE2D::DIFFUSE)
						a.colour.set(r, g, b);
			}

			else if(strstr(str, "Ks "))
			{
				float r, g, b;
				sscanf_s(str, "Ks %f %f %f", &r, &g, &b);
				for(auto& a : m_textures.back().second)
					if(a.type == TEXTURE_TYPE2D::DIFFUSE)
						a.colour.set(r, g, b);
				continue;
			}

			else if(strstr(str, "Ke "))
				continue;

			else if(strstr(str, "Ni "))
				continue;

			else if(strstr(str, "d "))
			{
				continue;
			}
		}

	if(f)
		fclose(f);
}