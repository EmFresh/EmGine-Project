#pragma once
#include <vector>
#include "ResourceManager.h"
#include "Transformer.h"
#include "Model.h"
#include "Camera.h"

enum class LIGHT_TYPE
{
	DEFAULT,
	POINT,
	DIRECTIONAL,
	SPOTLIGHT
};

struct LightInfo:public Transformer
{
	LIGHT_TYPE type;
	
	ColourRGBA diffuse, specular;
	Coord3D<> direction = {0,-1,0};
	float angleConstraint = 45,
		specularExponent = 1,
		attenuationConst = 50,
		attenuationLinear = 50,
		attenuationQuadratic = 50;
	bool enable = true;

};

class LightSource
{
public:

	static void setLightType(LIGHT_TYPE type, unsigned m_index);

	static void translate(Coord3D<> pos, unsigned m_index);

	static void setSceneAmbient(ColourRGBA ambi);

	static void setDiffuse(ColourRGBA diff, unsigned m_index);

	static void setSpecular(ColourRGBA spec, unsigned m_index);

	static void setSpecularExponent(float specEx, unsigned m_index);

	static void setAttenuationConstant(float attenConst, unsigned m_index);

	static void setAttenuationLinear(float attenLinear, unsigned m_index);

	static void setAttenuationQuadratic(float attenQuad, unsigned m_index);

	static void setParent(Transformer* parent, unsigned index);

	static void setCamera(Camera* cam);

	static void setShader(Shader* shad);

	static void setLightAmount(unsigned size);

	static void enableLight(int index, bool enable);

	static unsigned size();

	static std::vector<FrameBuffer*> shadowBuffers(unsigned w, unsigned h, std::map<void*, Model*>&, unsigned index);

	static LightInfo getLightInfo(unsigned index);

	static void update();

	static void setFramebuffer(FrameBuffer* buff);
private:
	//Coord3D m_coord, m_spec;
	static ColourRGBA m_ambient;
	static std::vector<LightInfo >m_lights;
	static FrameBuffer* m_framebuffer;
	static std::vector<std::vector<FrameBuffer*>>m_shadows;
	static unsigned m_size;

	static Shader* m_shader;
	static Camera* m_cam;
	static LightInfo m_info;

};

