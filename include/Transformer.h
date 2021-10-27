#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Component.h"
#include "Quat.h"
#include "Utilities.h"



class Transformer:public Component
{
public:
	Transformer();
	Transformer(Transformer&, COMP_TYPE type = TRANSFORMER);
	Transformer(const Transformer&, COMP_TYPE type = TRANSFORMER);
	Transformer(COMP_TYPE type);

	virtual ~Transformer();

	void reset();
	void enableFPSMode(bool enable = true);

	/*SET ROTATION*/

	virtual void rotate(util::Vec3 angles);
	virtual void rotate(float x, float y, float z);
	virtual void rotateBy(util::Vec3 angles);
	virtual void rotateBy(float x, float y, float z);

	/*SET POSITION*/

	virtual void translate(float x, float y, float z);
	virtual void translate(util::Vec3 pos);
	virtual void translateBy(float x, float y, float z);
	virtual void translateBy(util::Vec3 pos);


	/*SET SCALE*/

	virtual void scaleBy(float scale);
	virtual void scaleBy(util::Vec3 scale);
	virtual void scaleBy(float x, float y, float z);
	virtual void scale(float scale);
	virtual void scale(util::Vec3 scale);
	virtual void scale(float x, float y, float z);

	/*GETTERS*/
	virtual util::Vec3 getLocalPosition();
	virtual util::Vec3 getLocalRotation();
	virtual util::Vec3 getScale();
	util::Vec3 getForward();
	util::Vec3 getUp();
	util::Vec3 getRight();

	virtual const glm::mat4& getLocalRotationMatrix();
	virtual const glm::mat4& getLocalScaleMatrix();
	virtual const glm::mat4& getLocalTranslationMatrix();

	virtual const glm::mat4& getWorldRotationMatrix();
	virtual const glm::mat4& getWorldScaleMatrix();
	virtual const glm::mat4& getWorldTranslationMatrix();

	/*Gets a combination of the rotation, scale, and translation matricies*/

	virtual glm::mat4 getLocalTransformation();
	virtual glm::mat4 getWorldTransformation();

	virtual void resetUpdated();
	virtual bool isUpdated();
	virtual bool isScaleUpdated();
	virtual bool isRotationUpdated();
	virtual bool isTranslatinUpdated();

	struct TransformationData
	{
		util::Vec3 m_forward = {0,0,1}, m_up = {0,1,0}, m_right = {1,0,0};
		util::Vec3 m_posDat, m_rotDat, m_scaleDat;
	};
	struct ActivatorData
	{

		bool  m_updatedRot = true,
			m_updatedTrans = true,
			m_updatedScale = true,
			//first person movement
			m_fps = false,
			m_rotateBy = false;
	};
	struct MatrixData
	{
		glm::mat4
			m_localTranslate = glm::mat4(1),
			m_localRotate = glm::mat4(1),
			m_localScale = glm::mat4(1),

			m_worldTranslate = glm::mat4(1),
			m_worldRotate = glm::mat4(1),
			m_worldScale = glm::mat4(1);
	};

	TransformationData getTransformationData() { return m_transforms; }
	Transformer::ActivatorData getActivatorData() { return m_activators; }
	MatrixData getMatrixData() { return m_matricies; }

	void setTransformationData(TransformationData dat) { m_transforms = dat; }
	void setActivatorData(Transformer::ActivatorData dat) { m_activators = dat; }
	void setMatrixData(MatrixData dat) { m_matricies = dat; }

private:
	void calculateWorldRotationMatrix();
	void calculateWorldScaleMatrix();
	void calculateWorldTranslationMatrix();

	TransformationData m_transforms;
	ActivatorData m_activators;

protected:
	MatrixData m_matricies;
	Component::CompID createID()
	{
		CompID tmp = 0;
		CompID id = 1;
		using std::pair;
		m_compList.sort([](pair<COMP_TYPE, Component*> a,
						pair<COMP_TYPE, Component*> b)->bool
		{return a.second->getID() < b.second->getID(); });
		for(auto& a : m_compList)
		{
			if(!a.second->getID())continue;

			if((a.second->getID() - tmp) < 2)
				tmp = a.second->getID();
			else
			{
				id = ++tmp;
				return id;
			}
		}

		id = tmp ? tmp + 1 : id;
		return id;
	}

};

