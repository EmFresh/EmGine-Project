#include "Mesh.h"
#include <functional>
//#include <filesystem>
//namespace fs = std::filesystem;

using namespace util;
Mesh::Mesh(): Component(MESH)
{

}

Mesh::Mesh(Mesh& mesh) : Component(MESH)
{
	*this = mesh;
}

Mesh::~Mesh()
{
#if _DEBUG
	//	printf("Deleted Mesh\n");
#endif // _DEBUG

	if(!ani)
		unload();
}

bool Mesh::loadPrimitive(PrimitiveMesh* mesh)
{

	if(!mesh)
		return false;
	unload();

	mesh->createMesh();
	m_unpackedData = (mesh->getData());
	m_indicieData = (mesh->getIndices());
	m_textures = {Texture2D(),Texture2D()};
	m_replaceTex = m_textures;

	m_textures[0].type = TEXTURE_TYPE2D::DIFFUSE;
	m_textures[1].type = TEXTURE_TYPE2D::SPECULAR;

	top = mesh->m_top;
	bottom = mesh->m_bottom;
	left = mesh->m_left;
	right = mesh->m_right;
	front = mesh->m_front;
	back = mesh->m_back;


	init();

	return true;
}

void Mesh::render(Shader& shader, bool enableTex)
{
	shader.enable();

	bool textured = false;

	int  b = 0;

	if(enableTex)
		for(uint a = 0; a < m_textures.size(); ++a)
			if(m_textures[a].type == TEXTURE_TYPE2D::DIFFUSE)
				if(m_textures[a].id || m_replaceTex[a].id)
				{
					textured = true;
					Texture2D::bindTexture(b, m_replaceTex[a].id ? m_replaceTex[a].id : m_textures[a].id);
					shader.sendUniform("uTex", b++);
				}

	glUniform1i(shader.getUniformLocation("textured"), textured);

	glBindVertexArray(m_vaoID);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elemID);

	//Draw indicies
	glDrawElements(
		GL_TRIANGLES,						// mode
		(GLsizei)m_indicieData.size(),		// count
		GL_UNSIGNED_INT,					// data type
		(void*)0							// element array buffer offset
	);

	glBindVertexArray(0);

	for(; b >= 0; b--)
	{
		glActiveTexture(GL_TEXTURE0 + b);
		glBindTexture(GL_TEXTURE_2D, 0);
		//glActiveTexture(0);
	}


	shader.disable();
}

void Mesh::init()
{
	m_vaoID = 0;
	m_vboID = {0,0};

	glGenVertexArrays(1, &m_vaoID);
	glGenBuffers(2, &m_vboID.first);

	glBindVertexArray(m_vaoID);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	//UnpackedData 1
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID.first);
	glBufferData(GL_ARRAY_BUFFER, m_unpackedData.size() * sizeof(Vertex3D), m_unpackedData.data(), GL_STATIC_DRAW);

	//vertex 1   attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, coord));
	//UV         attributes
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, uv));
	//normal 1   attributes
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, norm));

	//UnpackedData 2 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID.second);
	glBufferData(GL_ARRAY_BUFFER, m_unpackedData.size() * sizeof(Vertex3D), m_unpackedData.data(), GL_STATIC_DRAW);

	//vertex 2   attributes
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, coord));
	//normals 2   attributes
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, norm));

	//create indicies
	m_elemID = 0;
	glGenBuffers(1, &m_elemID);

	//indicies 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elemID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicieData.size() * sizeof(unsigned), m_indicieData.data(), GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::replaceTexture(int index, Texture2D tex)
{
	m_replaceTex[index] = tex;
}

void Mesh::editVerts(Mesh* verts1, Mesh* verts2)
{
	glBindVertexArray(m_vaoID);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	//UnpackedData 1
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID.first);
	glBufferData(GL_ARRAY_BUFFER, verts1->m_unpackedData.size() * sizeof(Vertex3D), verts1->m_unpackedData.data(), GL_DYNAMIC_DRAW);

	//vertex 1   attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, coord));
	//UV         attributes
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, uv));
	//normal 1   attributes
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, norm));

	//UnpackedData 2 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID.second);
	glBufferData(GL_ARRAY_BUFFER, verts2->m_unpackedData.size() * sizeof(Vertex3D), verts2->m_unpackedData.data(), GL_DYNAMIC_DRAW);

	//vertex 2   attributes
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, coord));
	//normals 2   attributes
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, norm));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::unload()
{
	{
		if(m_vaoID)
			glDeleteVertexArrays(1, &m_vaoID);

		if(m_vboID.first)
			glDeleteBuffers(2, &m_vboID.first);


		if(m_elemID)
			glDeleteBuffers(1, &m_elemID);
	}

	m_vaoID = 0;
	m_vboID = {0,0};
	m_elemID = 0;

	m_indicieData.clear();
	m_unpackedData.clear();
}