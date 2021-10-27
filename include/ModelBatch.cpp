#include "ModelBatch.h"

using namespace std;

ModelBatch::ModelBatch()
{
	createAttribPointers();
}


ModelBatch::~ModelBatch()
{}

void ModelBatch::init()
{
	createAttribPointers();
}

void ModelBatch::begin()
{}

void ModelBatch::draw(const char * file, Transformer* transformation, ColourRGBA colour)
{
	loadModel(file);
	for(int a = 0; a < m_meshes.size(); a++)
		addGlyph(new Glyph3D(*m_meshes[a], transformation, colour));
	m_meshes.clear();
}

void ModelBatch::end()
{
	createRenderBatches();
}

void ModelBatch::render(GLSLCompiler & shader, Camera & cam)
{
	shader.enable();
	GLuint diffuseNr = 0, specularNr = 0;
	glUniformMatrix4fv(shader.getUniformLocation("camera"), 1, GL_FALSE, &(cam.getCameraMatrix()[0][0]));
	glUniformMatrix4fv(shader.getUniformLocation("object"), 1, GL_FALSE, &((cam.getObjectMatrix())[0][0]));

	GLuint counter = 0;
		glBindVertexArray(m_vaoID);
	for(int a = 0; a < m_renderBatches.size(); a++)
	{
		for(int b = 0; b < m_renderBatches[a].textures.size(); b++)
		{
			glActiveTexture(GL_TEXTURE0 + b);

			string number;
			string name = m_renderBatches[a].textures[b].type;
			if(name == "diffuseTexture")
				number = to_string(++diffuseNr);
			else if(name == "specularTexture")
				number = to_string(++specularNr);

			glUniform1i(shader.getUniformLocation(("material." + name + number).c_str()), b);

			glBindTexture(GL_TEXTURE_2D, m_renderBatches[a].textures[b].id);
		}
		glUniform1i(shader.getUniformLocation("material.shininess"), 16.f);

		glDrawElements(GL_TRIANGLES, m_renderBatches[a].numIndicies, GL_UNSIGNED_INT, &counter);
		counter += m_renderBatches[a].numIndicies;
	}
		glBindVertexArray(0);
	shader.disable();
}

void ModelBatch::addGlyph(Glyph3D * glyph)
{
	m_glyphs.push_back(glyph);
}

void ModelBatch::createRenderBatches()
{
	static vector<Vertex3D>verts;
	static vector<GLuint>indicies;
	verts.clear(), indicies.clear();//clear all data

	if(!m_glyphs.size())
		return;

	unsigned long offsetV = 0, offsetI = 0;

	for(GLuint cg = 0; cg < m_glyphs.size(); cg++)
	{
		if(!cg)
			addRenderBatch({offsetV,offsetI,
						   m_glyphs[cg]->mesh.m_verts.size(),
						   m_glyphs[cg]->mesh.m_indicies.size(),
						   m_glyphs[cg]->trans, m_glyphs[cg]->mesh.m_textures});
		else if(m_glyphs[cg]->mesh.m_textures == m_glyphs[cg - 1]->mesh.m_textures)
			addRenderBatch({offsetV,offsetI,
						   m_glyphs[cg]->mesh.m_verts.size(),
						   m_glyphs[cg]->mesh.m_indicies.size(),
						   m_glyphs[cg]->trans, m_glyphs[cg]->mesh.m_textures});
		else
			m_renderBatches[m_renderBatches.size() - 1].numVerts += m_glyphs[cg]->mesh.m_verts.size(),
			m_renderBatches[m_renderBatches.size() - 1].numIndicies += m_glyphs[cg]->mesh.m_indicies.size();

		verts.insert(verts.end(), m_glyphs[cg]->mesh.m_verts.begin(), m_glyphs[cg]->mesh.m_verts.end());
		offsetV += m_glyphs[cg]->mesh.m_verts.size();
		indicies.insert(indicies.end(), m_glyphs[cg]->mesh.m_indicies.begin(), m_glyphs[cg]->mesh.m_indicies.end());
		offsetI += m_glyphs[cg]->mesh.m_indicies.size();
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vaoID);
	//orphan data
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3D)*(offsetV), nullptr, GL_STATIC_DRAW);
	//upload data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex3D)*(offsetV), verts.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboID);
	//orphan data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*(offsetI), nullptr, GL_STATIC_DRAW);
	//upload data
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint)*(offsetI), indicies.data());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void ModelBatch::addRenderBatch(RenderBatch3D batch)
{
	m_renderBatches.push_back(batch);
}

void ModelBatch::createAttribPointers()
{
	if(!m_vaoID)
		glGenVertexArrays(1, &m_vaoID);
	if(!m_vboID)
		glGenBuffers(1, &m_vboID);
	if(!m_iboID)
		glGenBuffers(1, &m_iboID);

	glBindVertexArray(m_vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	//vertex     atributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*) offsetof(Vertex3D, coord));
	//ColourRGBA atributes
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex3D), (void*) offsetof(Vertex3D, colour));
	//UV         atributes
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*) offsetof(Vertex3D, uv));
	//normal     atributes
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*) offsetof(Vertex3D, norm));

	glBindVertexArray(0);
}

void ModelBatch::loadModel(const char * path)

{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		printf("Load Model Failed: %s\n", importer.GetErrorString());
		return;
	}

	dir = std::string(path).substr(0, strrchr(path, '/') - path);
	printf("%s\n", dir.c_str());

	processNode(scene->mRootNode, scene);
}

void ModelBatch::processNode(aiNode * node, const aiScene * scene)

{
	for(GLuint a = 0; a < node->mNumMeshes; a++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[a]];
		//	m_meshes.push_back(processMesh(mesh, scene));
		processMesh(mesh, scene);
	}

	for(GLuint a = 0; a < node->mNumChildren; a++)
		processNode(node->mChildren[a], scene);

}

void ModelBatch::processMesh(aiMesh * mesh, const aiScene * scene)


{
	std::vector<Vertex3D> verts;
	std::vector<GLuint> indicies;
	std::vector<Texture2D> textures;
	for(int a = 0; a < mesh->mNumVertices; a++)
	{
		Vertex3D vert;
		Coord3D vec;

		vec.x = mesh->mVertices[a].x;
		vec.y = mesh->mVertices[a].y;
		vec.z = mesh->mVertices[a].z;
		//glm::vec4 tmp = getTransformer().getTransformation()* glm::vec4(mesh->mVertices[a].x, mesh->mVertices[a].y, mesh->mVertices[a].z, 1);
		//vec = {tmp.x,tmp.y,tmp.z};
		vert.coord = vec;


		vec.x = mesh->mNormals[a].x;
		vec.y = mesh->mNormals[a].y;
		vec.z = mesh->mNormals[a].z;
		vert.norm = vec;

		if(mesh->mTextureCoords[0])
		{
			UV uv;

			uv.u = mesh->mTextureCoords[0][a].x;
			uv.v = mesh->mTextureCoords[0][a].y;

			vert.uv = uv;

		}

		verts.push_back(vert);
	}
	for(int a = 0; a < mesh->mNumFaces; a++)
	{
		aiFace norm = mesh->mFaces[a];
		for(int b = 0; b < norm.mNumIndices; b++)
			indicies.push_back(norm.mIndices[b]);
	}

	if(mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		//some stuff about speular and dufused textures
		std::vector<Texture2D>
			diffuseMaps = loadMaterialTextures(material,
											   aiTextureType_DIFFUSE, "diffuseTexture");

		std::vector<Texture2D>
			specularMaps = loadMaterialTextures(material,
												aiTextureType_SPECULAR, "specularTexture");


		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		for(auto &a : specularMaps)
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	m_meshes.push_back(new Mesh(verts, indicies, textures));
}

std::vector<Texture2D> ModelBatch::loadMaterialTextures(aiMaterial * mat, aiTextureType texType, std::string typeName)

{
	std::vector<Texture2D> textures;
	for(unsigned int a = 0; a < mat->GetTextureCount(texType); a++)
	{
		aiString str;
		mat->GetTexture(texType, a, &str);

		Texture2D texture;
		texture = ResourceManager::getTexture2D((std::string(dir) + '/' + str.C_Str()).c_str());
		texture.type = typeName;
		texture.path = str.C_Str();
		textures.push_back(texture);
		//printf("loaded Material: %s\n", str.C_Str());
		//loadedTextures.push_back(texture);

	}

	return textures;
}
