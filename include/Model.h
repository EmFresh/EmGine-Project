#pragma once
#include <GL\glew.h>
#include <GLFW/glfw3.h>	  
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "Camera.h"
#include "Shader.h"
#include "Utilities.h"
#include "Transformer.h"
#include "Physics3D.h"
#include "MeshLoader.h"
#include "Animation.h"
//#include "FrameBuffer.h"

class Camera;
class Animation;

class Model: public Transformer
{
public:
	Model():Transformer(MODEL) { m_ID = createID(); };
	Model(Model& model, cstring tag = "");
	Model(const Model& model, cstring tag = "");
	Model(PrimitiveMesh* model, cstring tag = "");
	Model(cstring path, cstring tag = "");
	virtual ~Model();


	void create(const Model& model, cstring tag = "");
	void create(PrimitiveMesh* mesh, cstring tag = "");
	void create(cstring path, cstring tag = "");

	void setActive(bool active);
	bool isActive();

	bool collision2D(Model* k, util::Vec3 ignore);
	bool collision3D(Model* k);

	static bool collision2D(Model* l, Model* k, util::Vec3 ignore);

	static bool collision3D(Model* l, Model* k);

	static bool getSeparatingPlane(const util::Vec3& RPos, const util::Vec3& Plane, Model& box1, Model& box2);


	virtual void render(Shader& shader, Camera* cam);


	//sets RGBA colour with float values ranging from 0 -> 1
	void setColour(float r, float g, float b, float a);

	//sets RGB colour with float values ranging from 0 -> 1
	void setColour(float r, float g, float b);

	//sets RGBA colour with values ranging from 0 -> 255
	void setColour(util::ColourRGBA colour);

	util::ColourRGBA getColour();


	bool loadModel(cstring path);

	void enableBoundingBox(bool enable);

	void addAnimation(std::string tag, Animation* animation);

	void editVerts(Model* first, Model* second);

	bool operator==(Model& mod)const
	{
		if(!this || !&mod)return false;
		return this->m_ID == mod.m_ID;
	}


	float getWidth();
	float getHeight();
	float getDepth();
	util::Vec3 getDimentions();

	util::Vec3 getCenter();

	cstring getTag();
	void setTag(cstring tag);
	cstring getPath();
	void setPath(cstring path);

	Animation* getAnimation(cstring tag);
	Animation* getCurrentAnimation();
	void  setAnimation(cstring tag);

	void addMesh(Mesh*);
	Mesh* getMesh(uint index);
	std::vector<std::shared_ptr<Mesh>>& getMeshList();
	Shader* getShader();

	void replaceTexture(int mesh, int index, Texture2D tex);

	void enableTexture(bool enable);
	bool isTextureEnabled();
	void setToRender(bool render);
	void setTransparent(bool trans);
	bool isTransparent();
	void setCastShadow(bool cast);
	bool isCastingShadow();
	void setWireframe(bool wire);
	void print();
	std::vector<util::Vec3> getBounds();
	void boundingBoxUpdate();

	struct ActivatorData
	{
		bool m_active = true;
		bool m_useTex = true;
		bool m_render = true;
		bool m_transparent = false;
		bool m_wireframe = false;
		bool m_shadowCast = true;
		bool m_enableBB = false;
	};
	struct BoundsData
	{
		util::Vec3
			m_topLeftBack,
			m_topRightBack,
			m_topLeftFront,
			m_topRightFront,
			m_bottomLeftBack,
			m_bottomRightBack,
			m_bottomLeftFront,
			m_bottomRightFront,
			m_center;

		util::Vec3 m_dimentions;
		util::Vertex3D m_vertBBDat[12 * 3];

	};

	ActivatorData getActivatorData() { return m_activators; }
	BoundsData getBoundsData() { return m_bounds; }
	void setActivatorData(ActivatorData dat) { m_activators = dat; }
	void setBoundsData(BoundsData dat) { m_bounds = dat; }

protected:

	std::string m_path = "";
	std::string m_tag = "";
	util::ColourRGBA m_colour;

private:
	void boundingBoxInit();
	void drawBoundingBox();

	ActivatorData m_activators;

	std::string m_animation;
	std::unordered_map< std::string, Animation*>m_animations;
	std::vector<std::shared_ptr<Mesh>> m_meshes;

	Camera* m_camera = nullptr;

	GLuint m_BBVaoID = 0, m_BBVboID = 0;

	BoundsData m_bounds;

	Shader* m_shader = nullptr, * m_shaderBB = nullptr;
};
