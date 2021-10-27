#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include "Utilities.h"

struct WindowInfo
{
	std::string title;
	util::Coord2D<int> position,
		size;
	int monitor;
	void print()
	{
		printf("Title    : %s\n\n", title.c_str());
		printf("Position : (%d, %d)\n", position.x, position.y);
		printf("Size     : (%d, %d)\n", size.width, size.height);
		printf("Monitor  : %d\n\n", monitor);
	}
};

class WindowCreator
{
public:
	enum
	{
		WINDOW_CREATED,
		WINDOW_FAILED
	};

	WindowCreator();
	WindowCreator(std::string name, util::Coord2D<int>, util::Coord2D<int> = {}, int monitor = 0, bool fullScreeen = false, bool visable = true);
	~WindowCreator();

	static int	createWindow(std::string name, util::Coord2D<int>, util::Coord2D<int> = {}, int monitor = 0, bool fullScreeen = false, bool visable = true);

	static void setVisable(bool);

	static void setFullScreen(bool);

	GLFWwindow* getWindow();

	std::string& getTitle();
	static util::Coord2D<int>& getScreenSize();
	static int getScreenWidth();
	static int getScreenHeight();
	static void(*m_onWindowResizeCallback)(GLFWwindow*, int, int);

private:
	static void onWindowResize(GLFWwindow* glfw, int w, int h);




};

