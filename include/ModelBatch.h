#pragma once
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include "Camera.h"
#include "Mesh.h"
#include "Transformer.h"
#include "StructInfo.h"
#include "ResourceManager.h"

struct Glyph3D
{
	Glyph3D() {}
	Glyph3D(Mesh mesh, Transformer* trans, ColourRGBA colour):mesh(mesh), colour(colour), trans(trans)
	{}

	Mesh mesh;
	ColourRGBA colour;
	Transformer* trans;



};

struct RenderBatch3D
{
	GLuint	offsetVert, offsetIndicies,
		numVerts, numIndicies;
	Transformer* trans;
	std::vector<Texture2D>	textures;
};

class ModelBatch
{
public:
	ModelBatch();
	~ModelBatch();
	void init();


	void begin();
	void draw(const char* file, Transformer *transformation = new Transformer(), ColourRGBA colour = {});
	void end();

	void render(GLSLCompiler& shader, Camera& cam);
	void addGlyph(Glyph3D *glyph);
	
private:
	//template<class T> friend bool operator==(std::vector<T>& v1, std::vector<T> v2);
	
	void loadModel(const char * path);

	void processNode(aiNode * node, const aiScene * scene);

	void processMesh(aiMesh * mesh, const aiScene * scene);

	std::vector< Texture2D> loadMaterialTextures(aiMaterial *mat, aiTextureType texType, std::string typeName);

	void createRenderBatches();
	void addRenderBatch(RenderBatch3D);
	void createAttribPointers();

	GLuint m_vaoID = 0, m_vboID = 0, m_iboID = 0;
	std::string dir;
	std::vector<RenderBatch3D>m_renderBatches;
	std::vector<Mesh*> m_meshes;
	std::vector <Glyph3D*> m_glyphs;

};

	//works on most vectors
template<class T>
bool operator==(std::vector<T>& v1, std::vector<T> v2)
{
	if(v1.size() == v2.size())
	{
		for(int a = 0; a < v1.size(); a++)
			if(v1[a] == v2[a])
				return false;
	} else
		return false;
	return true;
}