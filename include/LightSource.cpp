#include "LightSource.h"

#pragma region Static Variables
ColourRGBA LightSource::m_ambient;
std::vector<LightInfo >LightSource::m_lights;
FrameBuffer* LightSource::m_framebuffer;
std::vector<std::vector<FrameBuffer*>>LightSource::m_shadows;

Shader* LightSource::m_shader;
Camera* LightSource::m_cam;
LightInfo LightSource::m_info;
unsigned LightSource::m_size;
#pragma endregion



void LightSource::setLightType(LIGHT_TYPE type, unsigned index)
{
	m_lights[index].type = type;

	if(type == LIGHT_TYPE::POINT)
	{
		//m_shadows[index].resize(6, new FrameBuffer(0, "Shadow"));
		for(int a = 0; a < 6; a++)
		{

			//	m_shadows[index][a]->initDepthTexture(500, 500);
			//
			//	if(!m_shadows[index][a]->checkFBO())
			//	{
			//		printf("%s FBO failed Creation", m_shadows[index][a]->getTag().c_str());
			//		system("pause");
			//	}
		}
	}
	//else
	//	m_shadows[index].resize(0);


}

void LightSource::translate(Coord3D<> pos, unsigned m_index)
{
	m_lights[m_index].translate(pos.x, pos.y, pos.z);
}

void LightSource::setSceneAmbient(ColourRGBA ambi)
{
	m_ambient = ambi;
}

void LightSource::setDiffuse(ColourRGBA diff, unsigned m_index)
{
	m_lights[m_index].diffuse = diff;
}

void LightSource::setSpecular(ColourRGBA spec, unsigned m_index)
{
	m_lights[m_index].specular = spec;
}

void LightSource::setSpecularExponent(float specEx, unsigned m_index)
{
	m_lights[m_index].specularExponent = specEx;
}

void LightSource::setAttenuationConstant(float attenConst, unsigned m_index)
{
	m_lights[m_index].attenuationConst = attenConst;
}

void LightSource::setAttenuationLinear(float attenLinear, unsigned m_index)
{
	m_lights[m_index].attenuationLinear = attenLinear;
}

void LightSource::setAttenuationQuadratic(float attenQuad, unsigned m_index)
{
	m_lights[m_index].attenuationQuadratic = attenQuad;
}

void LightSource::setParent(Transformer* parent, unsigned index)
{
	parent->addChild(&m_lights[index]);
}

void LightSource::setCamera(Camera* cam)
{
	m_cam = cam;
}

void LightSource::setShader(Shader* shad)
{
	m_shader = shad;
}

void LightSource::setLightAmount(unsigned size)
{
	m_size = size > 0 ? size : 0;
	m_lights.resize(m_size);
	//m_shadows.resize(m_size, std::vector<FrameBuffer*>(0, new FrameBuffer(0, "shadow")));
}

void LightSource::enableLight(int index, bool enable)
{
	m_lights[index].enable = enable;
}

unsigned LightSource::size()
{
	return m_size;
}

std::vector<FrameBuffer*> LightSource::shadowBuffers(unsigned w, unsigned h, std::map<void*, Model*>& models, unsigned index)
{
	models;
	if(m_lights[index].type == LIGHT_TYPE::POINT)
	{

		glViewport(0, 0, w, h);


		static Camera cam;
		cam.init({(float)w,(float)h,500}, ORTHOGRAPHIC);

		cam.translate(m_lights[index].getPosition());

		for(int a = 0; a < 6; a++)
		{
			//	m_shadows[index][a]->resizeColour(0, w, h);
			m_shadows[index][a]->resizeDepth(w, h);

			m_shadows[index][a]->enable();
			//m_shader->enable();
			Shader* shad = ResourceManager::getShader("Shaders/ShadowDepth.vtsh", "Shaders/ShadowDepth.fmsh");
			shad->enable();
			shad->sendUniform("lightSpaceMatrix", m_cam->getProjectionMatrix() * glm::lookAt(m_lights[index].getPosition().toVec3(),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f)));



			switch(a)
			{
			case 0:
				cam.rotate(90 * Coord3D<>{1, 0, 0});
				break;
			case 1:
				cam.rotate(90 * Coord3D<>{-1, 0, 0});
				break;
			case 2:
				cam.rotate(90 * Coord3D<>{0, 1, 0});
				break;
			case 3:
				cam.rotate(90 * Coord3D<> {0, -1, 0});
				break;
			case 4:
				cam.rotate({0, 0, 0});
				break;
			case 5:
				cam.rotate(180 * Coord3D<>{0, -1, 0});

				break;
			default:
				break;
			}



			cam.render(shad, models);

			//Shader* shad = ResourceManager::getShader("Shaders/ShadowShader.vtsh", "Shaders/ShadowShader.frag");
			//shad->enable();
			//shad->sendUniform("depthMap", 0);
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, m_shadows[index][a]->getDepthHandle());
			//FrameBuffer::drawFullScreenQuad();
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, GL_NONE);
			FrameBuffer::disable();
		}

		//m_shader->disable();
	}
	return m_shadows[index];
}

LightInfo LightSource::getLightInfo(unsigned index)
{
	return m_lights[index];
}


void LightSource::update()
{
	m_shader->enable();
	//m_shader->sendUniform("LightAmount", (int)m_lights.size());
	m_shader->sendUniform("LightAmbient", Coord3D<>{m_ambient[0] / 255.0f, m_ambient[1] / 255.0f, m_ambient[2] / 255.0f});

	if(m_framebuffer)
		m_framebuffer->enable();

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_framebuffer->getColorHandle(0));


	for(unsigned a = 0; a < m_lights.size(); a++)
	{

		if(!m_lights[a].enable)
		{
			m_shader->sendUniform("LightEnable", false);
			continue;
		}
		m_shader->sendUniform("LightEnable", true);

		glm::vec4 pos(0, 0, 0, 1.0f);
		glm::vec4 dir{m_lights[a].direction.toVec3(), 1.0f};


		pos = m_lights[a].getWorldTranslationMatrix() * (m_lights[a].getLocalTranslationMatrix() * pos);
		pos = m_cam->getProjectionMatrix() * m_cam->getViewMatrix()  * pos;
		pos /= pos.w;

		m_shader->sendUniform("LightPosition", pos);


		dir = m_lights[a].getWorldRotationMatrix() * (m_lights[a].getLocalRotationMatrix() * dir);
		dir = glm::normalize(dir);
		dir = m_cam->getWorldRotationMatrix() * (m_cam->getLocalRotationMatrix() * dir);
		dir = glm::normalize(dir);

		pos = {0, 0, 0, 1.0f};
		m_shader->sendUniform("uViewPos", (m_cam->getProjectionMatrix() * m_cam->getViewMatrix() * pos) / pos.w);

		m_shader->sendUniform("LightType", (int)m_lights[a].type);

		m_shader->sendUniform("LightDiffuse", m_lights[a].diffuse[0] / 255.0f, m_lights[a].diffuse[1] / 255.0f, m_lights[a].diffuse[2] / 255.0f);

		m_shader->sendUniform("LightSpecular", m_lights[a].specular[0] / 255.0f, m_lights[a].specular[1] / 255.0f, m_lights[a].specular[2] / 255.0f);

		m_shader->sendUniform("LightDirection", dir);

		m_shader->sendUniform("LightSpecularExponent", 50.0f);

		m_shader->sendUniform("Attenuation_Constant", m_lights[a].attenuationConst);

		m_shader->sendUniform("Attenuation_Linear", m_lights[a].attenuationLinear);

		m_shader->sendUniform("Attenuation_Quadratic", m_lights[a].attenuationQuadratic);

		FrameBuffer::drawFullScreenQuad();
	}
	m_shader->sendUniform("LightEnable", false);

	m_shader->disable();
	if(m_framebuffer)
		m_framebuffer->disable();

}

void LightSource::setFramebuffer(FrameBuffer* buff)
{
	m_framebuffer = buff;
}
