#pragma once
#include "Mesh.h"

class MeshLoader
{
public:
	static std::vector<std::shared_ptr<Mesh>> loadMesh(std::string path);
	static void setDirectory(cstring dir);

private:

	MeshLoader();

	//adds indices to the existing structure
	template<class _iter>
	void indiceAdder(_iter begin, _iter end, std::vector<util::Indicie>& origIndi, std::vector<util::Vertex3D>& origVerts, std::vector<util::Vertex3D> newVerts)
	{
		for(auto& i = begin; i < end; ++i)
		{
			//if the vertice dose not exist add to list
			if((std::find(origVerts.begin(), origVerts.end(), newVerts[*i])) == origVerts.end())
				origVerts.push_back(newVerts[*i]),
				origIndi.push_back(origVerts.size() - 1);

		}
	}

	static void loadMaterials(cstring path);

	static bool load(std::string path,std::string bin);

	static void cleanup();


	static std::vector<std::shared_ptr<Mesh>> m_meshes;
	static std::vector<std::pair<std::string, std::vector<Texture2D>>> m_textures;
};

