#include "Model.h" 
#include <ctime>

#include <GLFW/glfw3.h>


using namespace util;


Model::Model(Model& model, cstring tag):
	Transformer(model, MODEL),
	m_tag(tag)
{
	//glfwInit();

	create(model, tag);
}

Model::Model(const Model& model, cstring tag):
	Transformer(model, MODEL),
	m_tag(tag)
{
	//glfwInit();
	create(model, tag);
}

Model::Model(PrimitiveMesh* mesh, cstring tag):
	Transformer(MODEL),
	m_tag(tag)
{
	//glfwInit();
	create(mesh, tag);
}

Model::Model(cstring path, cstring tag):
	Transformer(MODEL),
	m_tag(tag)
{
	//glfwInit();
	create(path, tag);
}

Model::~Model()
{
#if _DEBUG
	//	printf("Deleted %s\n", m_type.c_str());
#endif // _DEBUG

}


void Model::create(const Model& model, cstring tag)
{
	*this = model;
	m_ID = createID();

	m_tag = strlen(tag) ? tag : "";

	boundingBoxInit();
	boundingBoxUpdate();
}

void Model::create(PrimitiveMesh* mesh, cstring tag)
{
	m_ID = createID();

	m_meshes.clear();
	m_meshes.push_back(std::shared_ptr<Mesh>(new Mesh()));
	if(strlen(tag))
		m_tag = tag;
	if(m_meshes.back()->loadPrimitive(mesh))
	{
		m_shaderBB = ResourceManager::getShader("Shaders/BoundingBox.vtsh", "Shaders/BoundingBox.fmsh");


		float top = m_meshes[0]->top.y,
			bottom = m_meshes[0]->bottom.y,
			left = m_meshes[0]->left.x,
			right = m_meshes[0]->right.x,
			front = m_meshes[0]->front.z,
			back = m_meshes[0]->back.z;

		for(auto& a : m_meshes)
		{
			top = top < a->top.y ? a->top.y : top,
				bottom = bottom>a->bottom.y ? a->bottom.y : bottom,
				left = left > a->left.x ? a->left.x : left,
				right = right < a->right.x ? a->right.x : right,
				front = front< a->front.z ? a->front.z : front,
				back = back > a->back.z ? a->back.z : back;
		}


		(m_bounds.m_topLeftBack = {left,top,back});
		(m_bounds.m_topRightBack = {right,top,back});
		(m_bounds.m_topLeftFront = {left,top,front});
		(m_bounds.m_topRightFront = {right,top,front});
		(m_bounds.m_bottomLeftBack = {left,bottom,back});
		(m_bounds.m_bottomRightBack = {right,bottom,back});
		(m_bounds.m_bottomLeftFront = {left,bottom,front});
		(m_bounds.m_bottomRightFront = {right,bottom,front});


		boundingBoxInit();
		boundingBoxUpdate();
	}
}

void Model::create(cstring path, cstring tag)
{
	m_ID = createID();
	m_meshes.clear();

	if(strlen(tag))
		m_tag = tag;
	if(strlen(path))
		m_path = m_path;

	if(loadModel(path))
	{
		m_shaderBB = ResourceManager::getShader("Shaders/BoundingBox.vtsh", "Shaders/BoundingBox.fmsh");


		float top = m_meshes[0]->top.y,
			bottom = m_meshes[0]->bottom.y,
			left = m_meshes[0]->left.x,
			right = m_meshes[0]->right.x,
			front = m_meshes[0]->front.z,
			back = m_meshes[0]->back.z;

		for(auto& a : m_meshes)
		{
			top = top < a->top.y ? a->top.y : top,
				bottom = bottom>a->bottom.y ? a->bottom.y : bottom,
				left = left > a->left.x ? a->left.x : left,
				right = right < a->right.x ? a->right.x : right,
				front = front< a->front.z ? a->front.z : front,
				back = back > a->back.z ? a->back.z : back;
		}

		(m_bounds.m_topLeftBack = {left,top,back});
		(m_bounds.m_topRightBack = {right,top,back});
		(m_bounds.m_topLeftFront = {left,top,front});
		(m_bounds.m_topRightFront = {right,top,front});
		(m_bounds.m_bottomLeftBack = {left,bottom,back});
		(m_bounds.m_bottomRightBack = {right,bottom,back});
		(m_bounds.m_bottomLeftFront = {left,bottom,front});
		(m_bounds.m_bottomRightFront = {right,bottom,front});

		boundingBoxInit();
		boundingBoxUpdate();
	}
}


void Model::setActive(bool active)
{
	m_activators.m_active = active;
}

bool Model::isActive()
{
	return m_activators.m_active;
}

/// - Collision Function - ///

bool Model::collision2D(Model* box2, Vec3 ignore)
{
	return collision2D(this, box2, ignore);
}

bool Model::collision2D(Model* box1, Model* box2, Vec3 RPos)
{

	RPos.normalize();
	RPos = (Vec3{1, 1, 1} - RPos);

	RPos = (box1->m_bounds.m_center - box2->m_bounds.m_center) * RPos;
	Vec3 AxisX{1,0,0}, AxisY{0,1,0}, AxisZ{0,0,1};

	glm::mat4
		* rotLocal1 = (glm::mat4*)&box1->getLocalRotationMatrix(),
		* rotLocal2 = (glm::mat4*)&box2->getLocalRotationMatrix(),

		* rotWorld1 = (glm::mat4*)&box1->getWorldRotationMatrix(),
		* rotWorld2 = (glm::mat4*)&box2->getWorldRotationMatrix();

	static Vec3 AxisX1, AxisY1, AxisZ1, AxisX2, AxisY2, AxisZ2;
	AxisX1 = (*rotWorld1 * (*rotLocal1 * glm::vec4(*(glm::vec3*)&AxisX, 1)));
	AxisY1 = (*rotWorld1 * (*rotLocal1 * glm::vec4(*(glm::vec3*)&AxisY, 1)));
	AxisZ1 = (*rotWorld1 * (*rotLocal1 * glm::vec4(*(glm::vec3*)&AxisZ, 1)));

	AxisX2 = (*rotWorld2 * (*rotLocal2 * glm::vec4(*(glm::vec3*)&AxisX, 1)));
	AxisY2 = (*rotWorld2 * (*rotLocal2 * glm::vec4(*(glm::vec3*)&AxisY, 1)));
	AxisZ2 = (*rotWorld2 * (*rotLocal2 * glm::vec4(*(glm::vec3*)&AxisZ, 1)));

	return !(
		getSeparatingPlane(RPos, AxisX1, *box1, *box2) ||
		getSeparatingPlane(RPos, AxisY1, *box1, *box2) ||
		getSeparatingPlane(RPos, AxisZ1, *box1, *box2) ||
		getSeparatingPlane(RPos, AxisX2, *box1, *box2) ||
		getSeparatingPlane(RPos, AxisY2, *box1, *box2) ||
		getSeparatingPlane(RPos, AxisZ2, *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisX1, AxisX2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisX1, AxisY2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisX1, AxisZ2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisY1, AxisX2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisY1, AxisY2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisY1, AxisZ2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisZ1, AxisX2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisZ1, AxisY2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisZ1, AxisZ2), *box1, *box2));

}


///~ 3D Collision Function ~///

bool Model::collision3D(Model* k)
{
	return collision3D(this, k);
}

bool Model::collision3D(Model* box1, Model* box2)
{
	static Vec3 RPos;
	RPos = box1->m_bounds.m_center - box2->m_bounds.m_center;
	Vec3 AxisX{1,0,0}, AxisY{0,1,0}, AxisZ{0,0,1};

	glm::mat4
		* rotLocal1 = (glm::mat4*)&box1->getLocalRotationMatrix(),
		* rotLocal2 = (glm::mat4*)&box2->getLocalRotationMatrix(),

		* rotWorld1 = (glm::mat4*)&box1->getWorldRotationMatrix(),
		* rotWorld2 = (glm::mat4*)&box2->getWorldRotationMatrix();

	static Vec3 AxisX1, AxisY1, AxisZ1, AxisX2, AxisY2, AxisZ2;
	AxisX1 = (*rotWorld1 * (*rotLocal1 * glm::vec4(*(glm::vec3*)&AxisX, 1)));
	AxisY1 = (*rotWorld1 * (*rotLocal1 * glm::vec4(*(glm::vec3*)&AxisY, 1)));
	AxisZ1 = (*rotWorld1 * (*rotLocal1 * glm::vec4(*(glm::vec3*)&AxisZ, 1)));

	AxisX2 = (*rotWorld2 * (*rotLocal2 * glm::vec4(*(glm::vec3*)&AxisX, 1)));
	AxisY2 = (*rotWorld2 * (*rotLocal2 * glm::vec4(*(glm::vec3*)&AxisY, 1)));
	AxisZ2 = (*rotWorld2 * (*rotLocal2 * glm::vec4(*(glm::vec3*)&AxisZ, 1)));

	return !(
		getSeparatingPlane(RPos, AxisX1, *box1, *box2) ||
		getSeparatingPlane(RPos, AxisY1, *box1, *box2) ||
		getSeparatingPlane(RPos, AxisZ1, *box1, *box2) ||
		getSeparatingPlane(RPos, AxisX2, *box1, *box2) ||
		getSeparatingPlane(RPos, AxisY2, *box1, *box2) ||
		getSeparatingPlane(RPos, AxisZ2, *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisX1, AxisX2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisX1, AxisY2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisX1, AxisZ2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisY1, AxisX2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisY1, AxisY2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisY1, AxisZ2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisZ1, AxisX2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisZ1, AxisY2), *box1, *box2) ||
		getSeparatingPlane(RPos, Vec3::crossProduct(AxisZ1, AxisZ2), *box1, *box2));
}

bool Model::getSeparatingPlane(const Vec3& RPos, const Vec3& plane, Model& box1, Model& box2)
{

	//RPos;
	Vec3 AxisX{1,0,0}, AxisY{0,1,0}, AxisZ{0,0,1};

	glm::mat4
		* trans1 = (glm::mat4*)&box1.getLocalRotationMatrix(),
		* trans2 = (glm::mat4*)&box2.getLocalRotationMatrix();

	Vec3 AxisX1 = (*trans1 * glm::vec4(*(glm::vec3*)&AxisX, 1));
	Vec3 AxisY1 = (*trans1 * glm::vec4(*(glm::vec3*)&AxisY, 1));
	Vec3 AxisZ1 = (*trans1 * glm::vec4(*(glm::vec3*)&AxisZ, 1));

	Vec3 AxisX2 = (*trans2 * glm::vec4(*(glm::vec3*)&AxisX, 1));
	Vec3 AxisY2 = (*trans2 * glm::vec4(*(glm::vec3*)&AxisY, 1));
	Vec3 AxisZ2 = (*trans2 * glm::vec4(*(glm::vec3*)&AxisZ, 1));

	//float w, h;
	//glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);

	return (fabs(Vec3::dotProduct(RPos, plane)) >
			(
			fabs(Vec3::dotProduct((AxisX1 * (box1.m_bounds.m_dimentions.width / 2)), plane)) +
			fabs(Vec3::dotProduct((AxisY1 * (box1.m_bounds.m_dimentions.height / 2)), plane)) +
			fabs(Vec3::dotProduct((AxisZ1 * (box1.m_bounds.m_dimentions.depth / 2)), plane)) +

			fabs(Vec3::dotProduct((AxisX2 * (box2.m_bounds.m_dimentions.width / 2)), plane)) +
			fabs(Vec3::dotProduct((AxisY2 * (box2.m_bounds.m_dimentions.height / 2)), plane)) +
			fabs(Vec3::dotProduct((AxisZ2 * (box2.m_bounds.m_dimentions.depth / 2)), plane))
			));
}

#include <thread>
void Model::render(Shader& shader, Camera* cam)
{
	if(!m_activators.m_active)return;

	float colour[4]{(float)m_colour.r / 255,(float)m_colour.g / 255,(float)m_colour.b / 255,(float)m_colour.a / 255};
	m_camera = cam;
	m_shader = &shader;
	shader.enable();

	shader.sendUniform("uLocalModel", getLocalTransformation());
	shader.sendUniform("uWorldModel", getWorldTransformation());
	shader.sendUniform("colourMod", reclass(glm::vec4, colour));
	shader.sendUniform("flip", true);
	shader.sendUniform("colourID", m_ID);
	shader.disable();

	if(m_animations[m_animation])
		m_animations[m_animation]->update(&shader, this);

	// update the position of the object
	boundingBoxUpdate();

	if(m_activators.m_render)
	{
		if(m_activators.m_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//std::list<std::thread> threads;
		//render the meshes
		for(auto& a : m_meshes)
			a->render(shader, m_activators.m_useTex);
		//threads.push_back(std::thread([&](){a->render(shader, m_activators.m_useTex);}));//


		if(m_activators.m_enableBB)
			drawBoundingBox();

		static Shader* shader2;
		//render child meshes
		for(auto& a : getChildren())
			switch(a->getCompType())
			{
			case MODEL:
				reclass(Model*, a)->render(shader, cam);
				break;
			case TEXT:
			{
				reclass(Text*, a)->render(cam);
			}
			break;
			default:
				break;
			}

		//	for(auto& a : threads) a.join();//rejoin all loose threads

		resetUpdated();
		if(m_activators.m_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void Model::drawBoundingBox()
{

	m_shaderBB->enable();
	m_shaderBB->sendUniform("colourMod", {0, 0, 0, 1});
	//m_shaderBB->disable();

	glBindVertexArray(m_BBVaoID);
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
	glBindVertexArray(0);

	m_shaderBB->disable();
}

void Model::setColour(float r, float g, float b, float a)
{
	m_colour.set((GLubyte)(255 * clamp(0.f, 1.f, r)), (GLubyte)(255 * clamp(0.f, 1.f, g)), (GLubyte)(255 * clamp(0.f, 1.f, b)), (GLubyte)(255 * clamp(0.f, 1.f, a)));
}

void Model::setColour(float r, float g, float b)
{
	m_colour.set((GLubyte)(255 * clamp(0.f, 1.f, r)), (GLubyte)(255 * clamp(0.f, 1.f, g)), (GLubyte)(255 * clamp(0.f, 1.f, b)));
}

void Model::setColour(ColourRGBA colour)
{
	m_colour = colour;
}

ColourRGBA Model::getColour()
{
	return m_colour;
}

bool Model::loadModel(cstring path)
{

	m_meshes.clear();
	m_meshes = MeshLoader::loadMesh(path);
	return !!m_meshes.size();
}

void Model::enableBoundingBox(bool enable)
{
	m_activators.m_enableBB = enable;
}

void Model::addAnimation(std::string tag, Animation* animation)
{
	m_animations[tag] = animation;
}

void Model::editVerts(Model* first, Model* second)
{
	for(unsigned a = 0; a < first->m_meshes.size(); a++)
		m_meshes[a]->editVerts(first->m_meshes[a].get(), second->m_meshes[a].get());

}

float Model::getWidth()
{
	return m_bounds.m_dimentions.width;
}

float Model::getHeight()
{
	return m_bounds.m_dimentions.height;
}

float Model::getDepth()
{

	return m_bounds.m_dimentions.depth;
}

Vec3 Model::getDimentions()
{

	return m_bounds.m_dimentions;
}

Vec3 Model::getCenter()
{

	return m_bounds.m_center;
}

cstring Model::getTag()
{
	return m_tag.c_str();
}

void Model::setTag(cstring tag)
{
	m_tag = tag;
}

cstring Model::getPath()
{
	return m_path.c_str();
}

void Model::setPath(cstring path)
{
	m_path = path;
}

void Model::boundingBoxUpdate()
{
	if(m_activators.m_enableBB && m_shaderBB)
	{
		m_shaderBB->enable();
		m_shaderBB->sendUniform("uLocalModel", getLocalTransformation());
		m_shaderBB->sendUniform("uWorldModel", getWorldTransformation());
		glUniformMatrix4fv(m_shaderBB->getUniformLocation("uView"), 1, GL_FALSE, &(m_camera->getViewMatrix()[0][0]));
		glUniformMatrix4fv(m_shaderBB->getUniformLocation("uProj"), 1, GL_FALSE, &(m_camera->getProjectionMatrix()[0][0]));
		m_shaderBB->disable();
	}




	std::vector<glm::vec4> bounds =
	{
	{*(glm::vec3*)&m_bounds.m_bottomRightBack,1},
	{*(glm::vec3*)&m_bounds.m_bottomLeftBack,1},
	{*(glm::vec3*)&m_bounds.m_topLeftBack,1},
	{*(glm::vec3*)&m_bounds.m_bottomLeftBack,1},
	{*(glm::vec3*)&m_bounds.m_topLeftFront,1},
	{*(glm::vec3*)&m_bounds.m_topLeftBack,1}
	};


	for(auto& a : bounds)
		a = getWorldScaleMatrix() * (getLocalScaleMatrix() * a);


	m_bounds.m_dimentions.width = abs(bounds[0].x - bounds[1].x);
	m_bounds.m_dimentions.height = abs(bounds[2].y - bounds[3].y);
	m_bounds.m_dimentions.depth = abs(bounds[4].z - bounds[5].z);


	bounds =
	{
	{*(glm::vec3*)&m_bounds.m_bottomRightBack,1},
	{*(glm::vec3*)&m_bounds.m_bottomLeftBack,1},
	{*(glm::vec3*)&m_bounds.m_topLeftBack,1},
	{*(glm::vec3*)&m_bounds.m_bottomLeftBack,1},
	{*(glm::vec3*)&m_bounds.m_topLeftFront,1},
	{*(glm::vec3*)&m_bounds.m_topLeftBack,1}
	};


	for(auto& a : bounds)
		a = getWorldTranslationMatrix() * (getLocalTransformation() * a);

	m_bounds.m_center =
		(Vec3(
		bounds[0].x + bounds[1].x,
		bounds[2].y + bounds[3].y,
		bounds[4].z + bounds[5].z) / 2);
}

Animation* Model::getAnimation(cstring tag)
{
	return m_animations[tag];
}

Animation* Model::getCurrentAnimation()
{
	return m_animations[m_animation];
}

void Model::setAnimation(cstring tag)
{
	m_animation = tag;
}

void Model::addMesh(Mesh* mesh)
{
	m_meshes.push_back(std::shared_ptr<Mesh>(mesh));
}

Mesh* Model::getMesh(const uint index)
{

	return index < m_meshes.size() ? m_meshes[index].get() : nullptr;
}

std::vector<std::shared_ptr<Mesh>>& Model::getMeshList()
{
	return m_meshes;
}

Shader* Model::getShader()
{
	return m_shader;
}

void Model::replaceTexture(int mesh, int index, Texture2D tex)
{
	m_meshes[mesh]->replaceTexture(index, tex);
}

void Model::enableTexture(bool enable)
{
	m_activators.m_useTex = enable;
}

bool Model::isTextureEnabled()
{
	return m_activators.m_useTex;
}

void Model::setToRender(bool render)
{
	m_activators.m_render = render;
}

void Model::setTransparent(bool trans)
{
	m_activators.m_transparent = trans;
}

void Model::setWireframe(bool wire)
{
	m_activators.m_wireframe = wire;
}

bool Model::isTransparent()
{
	return m_activators.m_transparent;
}

void Model::setCastShadow(bool cast)
{
	m_activators.m_shadowCast = cast;
}

bool Model::isCastingShadow()
{
	return m_activators.m_shadowCast;
}

void Model::boundingBoxInit()
{
	if(!m_BBVaoID)
		glGenVertexArrays(1, &m_BBVaoID);
	if(!m_BBVboID)
		glGenBuffers(1, &m_BBVboID);



	Vertex3D
		topLeftBack{m_bounds.m_topLeftBack},
		topRightBack{m_bounds.m_topRightBack},
		topLeftFront{m_bounds.m_topLeftFront},
		topRightFront{m_bounds.m_topRightFront},
		bottomLeftBack{m_bounds.m_bottomLeftBack},
		bottomRightBack{m_bounds.m_bottomRightBack},
		bottomLeftFront{m_bounds.m_bottomLeftFront},
		bottomRightFront{m_bounds.m_bottomRightFront};


	Vertex3D tmp[12 * 3]{
		//top
		topLeftBack,topRightBack,topRightFront,
		topLeftBack,topRightFront,topLeftFront,
		//bottom
		 bottomRightFront, bottomRightBack,bottomLeftBack,
		bottomLeftFront,bottomRightFront,bottomLeftBack,
		//front
		topLeftFront,topRightFront,bottomRightFront,
		topLeftFront,bottomRightFront,bottomLeftFront,
		//back
		bottomRightBack, topRightBack, topLeftBack,
		bottomLeftBack, bottomRightBack, topLeftBack,
		//left
		topLeftBack,topLeftFront,bottomLeftFront,
		topLeftBack,bottomLeftFront,bottomLeftBack,
		//right
		bottomRightFront,topRightFront,topRightBack,
		bottomRightBack,bottomRightFront,topRightBack
	};

	memcpy_s(m_bounds.m_vertBBDat, sizeof(Vertex3D) * 12 * 3, tmp, sizeof(Vertex3D) * 12 * 3);

	glBindVertexArray(m_BBVaoID);
	glBindBuffer(GL_ARRAY_BUFFER, m_BBVboID);
	glBufferData(GL_ARRAY_BUFFER, 12 * 3 * sizeof(Vertex3D), m_bounds.m_vertBBDat, GL_STATIC_DRAW);


	//vertex     atributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, coord));

	//UV         atributes
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, uv));

	//normal     atributes
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, norm));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Model::print()
{
	printf(
		"Tag: %s\n"
		"Path: %s\n"
		"Width: %f\n"
		"Height: %f\n"
		"Depth: %f\n"
		"Center: (%f, %f, %f)\n\n"
		, m_tag.c_str(),m_path.c_str(), m_bounds.m_dimentions.width, m_bounds.m_dimentions.height, m_bounds.m_dimentions.depth, m_bounds.m_center.x, m_bounds.m_center.y, m_bounds.m_center.z);
}

std::vector<Vec3> Model::getBounds()
{
	return std::vector<Vec3>{
		m_bounds.m_topLeftBack,
			m_bounds.m_topRightBack,
			m_bounds.m_topLeftFront,
			m_bounds.m_topRightFront,
			m_bounds.m_bottomLeftBack,
			m_bounds.m_bottomRightBack,
			m_bounds.m_bottomLeftFront,
			m_bounds.m_bottomRightFront};
}


