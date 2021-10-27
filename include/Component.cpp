#include "Component.h"
#include <typeinfo>

std::list<std::pair<Component::COMP_TYPE, Component*>>
Component::m_compList = std::list<std::pair<Component::COMP_TYPE, Component*>>();
bool Component::m_exit = false;
uint Component::m_countID = 0;



Component::Component(Component* parent): m_type(UNKNOWN)
{
	setParent(parent);
	m_compList.push_back(std::pair{m_type,this});
}

Component::Component(COMP_TYPE type, Component* parent): m_parent(parent), m_type(type)
{
	setParent(parent);
	m_compList.push_back(std::pair{m_type,(this)});
}

bool validityTest(Component* mod)
{
	try { auto a = mod->getChildren(); a; } catch(...) { return false; }

	return true;
}
Component::~Component()
{
	////text is cursed for some reason
	//if(m_type == "TEXT")
	//	return;

	//if(m_exit)return;
	auto a = std::find(m_compList.begin(), m_compList.end(), std::pair{m_type,(this)});
	m_compList.erase(a);
}

const std::list<std::pair<Component::COMP_TYPE, Component*>>& Component::getComponentList()
{
	return m_compList;
}

void Component::addChild(Component* child)
{
	if(!child)return;

	if(std::find(m_children.begin(), m_children.end(), child) == m_children.end())
		m_children.push_back(child);
	child->m_parent = this;
}

void Component::removeChild(Component* child)
{
	if(!child)return;

	child->m_parent = nullptr;

	std::vector<Component*>::iterator ref;
	if((ref = std::find(m_children.begin(), m_children.end(), child)) != m_children.end())
		m_children.erase(ref);


}

void Component::removeChild(unsigned index)
{
	if(index >= m_children.size())return;

	m_children[index]->m_parent = nullptr;
	m_children.erase(m_children.begin() + index);

}

void Component::setParent(Component* parent)
{
	if(!parent)return;

	parent->addChild(this);

}

void Component::removeParent(Component* parent)
{
	if(!parent)return;

	parent->removeChild(this);
}

Component::CompID Component::getID()
{
	return m_ID;
}

Component* Component::getChild(unsigned int index)
{
	return m_children[index];
}

Component* Component::getParent()
{
	return m_parent;
}

std::vector<Component*>& Component::getChildren()
{
	return m_children;
}
