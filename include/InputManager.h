#pragma once
#include "XinputManager.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Utilities.h"
#include <functional>
#include <unordered_map>

/***Enums***/

enum  KeyState
{
	KEY_RELEASED,
	KEY_PRESSED,
	KEY_HELD
};

enum ModKey
{
	KEY_MOD_SHIFT = 1,
	KEY_MOD_CONTROL = 2,
	KEY_MOD_ALT = 4,
	KEY_MOD_SUPER = 8
};

enum MouseState
{
	MOUSE_RELEASED,
	MOUSE_PRESSED
};

enum MouseButton
{
	MOUSE_BUTTON_1,
	MOUSE_BUTTON_2,
	MOUSE_BUTTON_3,
	MOUSE_BUTTON_4,
	MOUSE_BUTTON_5,
	MOUSE_BUTTON_6,
	MOUSE_BUTTON_7,
	MOUSE_BUTTON_8,

	MOUSE_LEFT_BUTTON = 0,
	MOUSE_RIGHT_BUTTON = 1,
	MOUSE_MIDDLE_BUTTON = 2,
	MOUSE_LAST_BUTTON = 7
};


/***Class***/
class InputManager
{
public:

	//must be called to work
	static void init();

	static void mouseButtonPressedCallback(std::function<void(int, int)> mouseButton);

	static void mouseButtonReleasedCallback(std::function<void(int, int)> mouseButton);

	static void mouseButtonAllCallback(std::function<void(int, int, int)> mouseButton);

	static bool mouseDown(MouseButton button);

	static bool mouseUp(MouseButton button);

	static bool mouseStroke(MouseButton button);

	static util::Vec2 getMousePosition();

	/*
	Callback for whenever any key is pressed
	*/
	static void setKeyPressedCallback(std::function<void(int, int)> key);

	/*
	Callback for whenever any key is released
	*/
	static void setKeyReleasedCallback(std::function<void(int, int)> key);

	/*
	Callback for any key pressed, held, or released
	*/
	static void setKeyAllCallback(std::function<void(int, int, int)> key);


	/*
	* key - The key which is pressed. You can either
	use GLFW_KEY_'KEY' (i.e. GLFW_KEY_RIGHT) or characters
	to be checked if key is pressed and then released.

	(Note: characters must be in uppercase)
	*/
	static bool keyStroke(int key);

	/*
	* key - The key which is pressed. You can either
	use GLFW_KEY_'KEY' (i.e. GLFW_KEY_RIGHT) or characters
	to be checked if key is pressed.

	Note: characters must be in uppercase
	*/
	static bool keyDown(int key);

	/*
	* key - The key which is pressed. You can either
	use GLFW_KEY_'KEY' (i.e. GLFW_KEY_RIGHT) or characters
	to be checked if key is released.

	Note: characters must be in uppercase
	*/
	static bool keyUp(int key);


	static int controllersConnected();

	static bool isControllerConnected(unsigned int m_index);

	static XinputDevice* getController(unsigned int m_index);

	static void controllerUpdate();

	static void update();

private:

	static void keyUpdate(GLFWwindow*, int key, int scancode, int action, int mods);
	static void mouseButtonUpdate(GLFWwindow*, int button, int action, int mods);
	static void mousePositionUpdate(GLFWwindow*, double x, double y);

	//static void xinputConnectionUpdate(int controller, int connected);


};

