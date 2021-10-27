#pragma once
#include <list>
#include <vector>
#include <fstream>
#include "Model.h"

class SceneManager
{
public:
	static void saveScene(cstring file, const std::vector<Model*>& models);
	static void loadScene(cstring file, std::list<Model>& models);

private:
	static bool sendTexture(std::ofstream& out, Texture2D& obj);
	static bool recvTexture(std::ifstream& in, Texture2D& obj);

	static bool sendMesh(std::ofstream& out, Mesh* obj);
	static bool recvMesh(std:: ifstream& in, Mesh* obj);
	
	static bool sendModel(std::ofstream& out, Model* obj);
	static bool recvModel(std::ifstream& in, Model* obj);


};

