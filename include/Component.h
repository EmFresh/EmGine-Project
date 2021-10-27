#pragma once
#include <list>
#include <string>
#include <unordered_map>
#include <memory>
#include "Utilities.h"



class Component
{
public:
	enum COMP_TYPE:uchar
	{
		UNKNOWN,		//	0
		TRANSFORMER,	//	1
		MODEL,			//	2
		MESH,			//	3
		PRIMITIVE_MESH,	//	4
		TEXT,			//	5
		CAMERA,			//	6
		LIGHT,			//	7
	};

	//typedef std::string COMP_TYPE;
	typedef uint CompID;
	static bool m_exit;

	//	void setID(CompID id) { m_ID = id; }

private:
	std::vector<Component*>m_children;
	Component* m_parent;

protected:
	Component(Component* = nullptr);
	Component(COMP_TYPE type, Component* parent = nullptr);
	virtual ~Component();

	CompID m_ID = 0;
	COMP_TYPE m_type;
	static uint m_countID;
	static std::list<std::pair<COMP_TYPE, Component*>>
		m_compList;

public:

	COMP_TYPE getCompType() { return m_type; }

	static const std::list<std::pair<COMP_TYPE, Component*>>& getComponentList();

	virtual void addChild(Component* child);
	virtual void removeChild(Component* child);
	virtual void removeChild(unsigned index);

	virtual void setParent(Component* parent);
	virtual void removeParent(Component* parent);

	virtual Component* getChild(unsigned int index);
	virtual Component* getParent();
	virtual std::vector<Component*>& getChildren();

	CompID getID();
};


