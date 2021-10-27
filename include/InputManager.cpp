#include "InputManager.h"

#pragma region Static Variables
//std::function<void(int)>
//InputManager::m_controllerConneced,
//InputManager::m_controllerDisconnected;

std::unordered_map<int, bool> m_keyStroke, m_keyState;
std::unordered_map<MouseButton, bool> m_mouseStroke, m_mouseState;

util::Vec2 m_mousePos;

std::function<void(int, int)>
m_keyUp,
m_keyInitDown,
m_keyHeldDown,
m_mouseButtonPress,
m_mouseButtonRelease;

std::function<void(double, double)>
m_mousePosition;

std::function<void(int, int, int)>
m_keyAll,
m_mouseButtonAll;
#pragma endregion

void InputManager::init()
{
	glfwInit();
	glfwSetKeyCallback(glfwGetCurrentContext(), keyUpdate);
	glfwSetMouseButtonCallback(glfwGetCurrentContext(), mouseButtonUpdate);
	glfwSetCursorPosCallback(glfwGetCurrentContext(), mousePositionUpdate);
	//glfwSetJoystickCallback(xinputConnectionUpdate);
}

void InputManager::mouseButtonPressedCallback(std::function<void(int, int)>mouseButton)
{
	m_mouseButtonPress = mouseButton;
}

void InputManager::mouseButtonReleasedCallback(std::function<void(int, int)>mouseButton)
{
	m_mouseButtonRelease = mouseButton;
}

void InputManager::mouseButtonUpdate(GLFWwindow*, int button, int state, int mods)
{
	if(m_mouseButtonAll != nullptr)
		m_mouseButtonAll(state, button, mods);

	if(state == MOUSE_PRESSED)
	{
		if(m_mouseButtonPress != nullptr)
			m_mouseButtonPress(button, mods);

		m_mouseState[MouseButton(button)] = true;
	}
	if(state == MOUSE_RELEASED)
	{
		if(m_mouseButtonRelease != nullptr)
			m_mouseButtonRelease(button, mods);

		(m_mouseStroke[MouseButton(button)] = true);
		m_mouseState[MouseButton(button)] = !m_mouseStroke[MouseButton(button)];
	}
}

void InputManager::mousePositionUpdate(GLFWwindow*, double x, double y)
{
	x, y;
}

void InputManager::keyUpdate(GLFWwindow*, int key, int scancode, int state, int mods)
{
	scancode;
	if(m_keyAll != nullptr)
		m_keyAll(state, key, mods);

	if(state == KEY_PRESSED)  //Key has been pressed initially
	{
		if(m_keyInitDown != nullptr)
			m_keyInitDown(key, mods);

		m_keyState[key] = true;
	}
	if(state == KEY_HELD)
		if(m_keyHeldDown != nullptr)
			m_keyHeldDown(key, mods);

	if(state == KEY_RELEASED) //more of a stroke since release is only called if a key is pressed then released
	{
		if(m_keyUp != nullptr)
			m_keyUp(key, mods);

		(m_keyStroke[key] = true);
		m_keyState[key] = !m_keyStroke[key];
	}
}

//void InputManager::xinputConnectionUpdate(int controller, int connected)
//{
//	controllerUpdate();
//	if(m_controllerConnection != nullptr)
//		m_controllerConnection(controller, connected);
//
//	if(m_controllerConneced != nullptr)
//		if(connected == GLFW_CONNECTED)
//			m_controllerConneced(controller);
//
//	if(m_controllerDisconnected != nullptr)
//		if(connected == GLFW_DISCONNECTED)
//			m_controllerDisconnected(controller);
//}

void InputManager::mouseButtonAllCallback(std::function<void(int, int, int)>mouseButton)
{
	m_mouseButtonAll = mouseButton;
}

bool InputManager::mouseDown(MouseButton button)
{
	return m_mouseState[button];
}

bool InputManager::mouseUp(MouseButton button)
{
	return	!m_mouseState[button];
}

bool InputManager::mouseStroke(MouseButton button)
{
	return m_mouseStroke[button];
}

util::Vec2 InputManager::getMousePosition()
{
	return m_mousePos;
}

void InputManager::setKeyPressedCallback(std::function<void(int, int)>key)
{
	m_keyInitDown = key;
}

void InputManager::setKeyReleasedCallback(std::function<void(int, int)>key)
{
	m_keyUp = key;
}

void InputManager::setKeyAllCallback(std::function<void(int, int, int)>key)
{
	m_keyAll = key;
}

bool InputManager::keyDown(int key)
{
	return m_keyState[key];
}

bool InputManager::keyUp(int key)
{
	return !m_keyState[key];
}

bool InputManager::keyStroke(int key)
{
	return m_keyStroke[key];
}

int InputManager::controllersConnected()
{
	int count = 0;
	for(int a = 0; a < 4; a++)
		count += XinputManager::controllerConnected(a);
	return count;
}

bool InputManager::isControllerConnected(unsigned int m_index)
{
	return XinputManager::controllerConnected(m_index);
}

XinputDevice* InputManager::getController(unsigned int m_index)
{
	return XinputManager::getController(m_index);
}

void InputManager::controllerUpdate()
{
	XinputManager::update();
}

void InputManager::update()
{
	controllerUpdate();
	static double x, y;
	glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);
	m_mousePos = {(float)x,(float)y};

	for(auto& a : m_mouseStroke)
		a.second = false;
	for(auto& a : m_keyStroke)
		a.second = false;

}