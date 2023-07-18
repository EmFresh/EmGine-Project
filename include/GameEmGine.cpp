#include "EmGineAudioPlayer.h"
#include "GameEmGine.h"
using namespace util;

#pragma region Static Variables
void(*m_compileShaders)();
std::function<void(FrameBuffer* gbuff, FrameBuffer* post, float dt)> m_customRender;
//std::function<void()>GameEmGine::m_render;
std::function<void(double)>m_gameLoop;
Camera* m_mainCamera;
std::vector<Camera*>m_cameras;
Shader
* m_gBufferShader/*, * m_postProcessShader,* m_forwardRender*/;

FrameBuffer
* m_gBuff, * m_postBuffer;


WindowCreator* m_window;	//must be init in the constructor
ColourRGBA m_colour{58, 58, 58};

std::unordered_map<std::string, FrameBuffer*> m_frameBuffers;
std::unordered_map<void*, Model*> m_models;

bool exitGame = false;
float m_fps;
short m_fpsLimit;
Scene* m_mainScene;
std::vector<Text*> m_text;
Coord2D<int> m_screenSize;
bool m_bloom;


//GLuint GameEmGine::colorCustom;
//int GameEmGine::LUTsize = 0;

bool GameEmGine::lutActive = false;


#pragma endregion


void GLAPIENTRY
GameEmGine::MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	source, id, length, userParam;
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

Texture3D GameEmGine::tmpLUT;
Texture2D tmpRamp;
std::string LUTpath;

void GameEmGine::init(std::string name, int width, int height, int x, int y, int monitor, bool fullScreen, bool visable)
{
	createNewWindow(name, width, height, x, y, monitor, fullScreen, visable);
	AudioPlayer::init();
	InputManager::init();




	//LUTpath = "Texture/IWLTBAP_Aspen_-_Standard.cube";
	///////////////////////////////////////Bind Custom 3D Texture////////////////////////////////////////////
	//
	//tmpLUT = ResourceManager::getTextureLUT(LUTpath.c_str());
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	///////////////////////////////////////Bind Custom 2D Texture////////////////////////////////////////////
	//
	//tmpRamp = ResourceManager::getTexture2D("Texture/pinkRamp.png");
	//
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////
}

void GameEmGine::createNewWindow(std::string name, int width, int height, int x, int y, int monitor, bool fullScreen, bool visable)
{
	glfwInit();

	printf("Creating The Window...\n");

	if(m_window)
		delete m_window;

	m_window = new WindowCreator(name, {width, height}, Coord2D<int>{x, y}, monitor, fullScreen, visable);
	m_window->m_onWindowResizeCallback = changeViewport;


	if(m_window)
		puts("Window Creation Successful\n");
	else
	{
		puts("Window Creation Unsuccessful\n");
		return;
	}

	m_mainCamera = new Camera(Camera::FRUSTUM, {(float)getWindowWidth(), (float)getWindowHeight(), (float)getWindowWidth()});



	initShader();

	printf("created the window\n");

	m_postBuffer = new FrameBuffer(1, "Post Process Buffer");
	m_postBuffer->initColourTexture(0, getWindowWidth(), getWindowHeight());
	m_postBuffer->initDepthTexture(getWindowWidth(), getWindowHeight());
	if(!m_postBuffer->checkFBO())
	{
		puts("FBO failed Creation");
		system("pause");
		return;
	}

	m_gBuff = new FrameBuffer(7, "Main Buffer");


	m_gBuff->initColourTexture(0, getWindowWidth(), getWindowHeight(), GL_RGBA, GL_RGBA16F, GL_FLOAT);//this buffer is broken for some reason
	m_gBuff->initColourTexture(1, getWindowWidth(), getWindowHeight(), GL_RGBA, GL_RGBA16F, GL_FLOAT);
	m_gBuff->initColourTexture(2, getWindowWidth(), getWindowHeight(), GL_RGBA, GL_RGBA16F, GL_FLOAT);
	m_gBuff->initColourTexture(3, getWindowWidth(), getWindowHeight(), GL_RGBA, GL_RGBA16F, GL_FLOAT);
	m_gBuff->initColourTexture(4, getWindowWidth(), getWindowHeight(), GL_RGB, GL_RGB8);
	m_gBuff->initColourTexture(5, getWindowWidth(), getWindowHeight(), GL_RGBA, GL_RGBA8);
	m_gBuff->initColourTexture(6, getWindowWidth(), getWindowHeight(), GL_RED_INTEGER, GL_R32UI, GL_UNSIGNED_INT, GL_NEAREST);
	m_gBuff->initDepthTexture(getWindowWidth(), getWindowHeight());
	if(!m_gBuff->checkFBO())
	{
		puts("FBO failed Creation");
		system("pause");
		return;
	}



	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

}

void GameEmGine::run()
{

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	glEnable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	while(!glfwWindowShouldClose(m_window->getWindow()) && !exitGame)//update loop
	{
		glClearColor((float)m_colour.r / 255, (float)m_colour.g / 255, (float)m_colour.b / 255, (float)m_colour.a / 255);//BG colour
		//glClearColor(0.f, 0.f, 0.f, 0.f);
		glCullFace(GL_BACK);

		InputManager::update();
		update();

		if(true)//fps calculation
		{
			calculateFPS();
			char str[20];
			sprintf_s(str, "fps: %.2f", m_fps);

			glClear(GL_DEPTH_BUFFER_BIT);

			static Text fps;
			static OrthoPeramiters ortho{ 0, (float)getWindowWidth(), (float)getWindowHeight(), 0, 0, 500 };
			static Camera cam(&ortho);
			cam.update();

			fps.setColour(1, 0, 0);
			fps.setText(str);
			fps.setTextSize(35);

			fps.translate(0, fps.getHeight(), 0);
			fps.rotate(180, 0, 0);


			glClearDepth(1.f);
			glClear(GL_DEPTH_BUFFER_BIT);
			fps.render(&cam);

			//glfwSetWindowTitle(m_window->getWindow(), (m_window->getTitle() + "--> " + str).c_str());
		}

		glfwSwapBuffers(m_window->getWindow());
		glFlush();
		fpsLimiter();
	}

	Component::m_exit = true;
	glfwInit();
	glfwTerminate();
}

void GameEmGine::exit()
{
	exitGame = true;
}

void GameEmGine::setFPSLimit(short limit)
{
	m_fpsLimit = limit;
}

short GameEmGine::getFPSLimit()
{
	return m_fpsLimit;
}

void GameEmGine::vsync(bool enable)
{
	glfwSwapInterval(enable);
}

void GameEmGine::updateControllerConnections()
{
	InputManager::controllerUpdate();
}

int GameEmGine::controllersConnected()
{
	return InputManager::controllersConnected();
}

bool GameEmGine::isControllerConnected(int m_index)
{
	return InputManager::isControllerConnected(m_index);
}

XinputDevice* GameEmGine::getController(int m_index)
{
	return InputManager::getController(m_index);
}

WindowCreator* GameEmGine::getWindow()
{
	return m_window;
}

void GameEmGine::initShader()
{
	m_gBufferShader = ResourceManager::getShader("Shaders/DeferredRender.vtsh", "Shaders/DeferredRender.fmsh");

	//m_postProcessShader = ResourceManager::getShader("Shaders/Main Buffer.vtsh", "Shaders/PassThrough.frag");
	//m_forwardRender = ResourceManager::getShader("Shaders/DeferredRender.vtsh", "Shaders/ForwardRender.fmsh");

	Shader::enableUniformErrors(false);
}

void GameEmGine::calculateFPS()
{
	static const int SAMPLE = 15;
	static short count = 0;
	static float frameTimes[SAMPLE]{};

	frameTimes[count++] = 1 / float(glfwGetTime());
	if(count == SAMPLE)
	{
		count = 0;
		m_fps = 0;
		for(auto& a : frameTimes)
			m_fps += a;
		m_fps /= SAMPLE;
	}

	glfwSetTime(0);
}

void GameEmGine::fpsLimiter()
{
	static bool enter = false;
	static clock_t frameStart = 0;


	//way 1: 
	if(enter)
		if(m_fpsLimit > 0)
			while((CLOCKS_PER_SEC / m_fpsLimit) > (clock() - frameStart));

	////way 2: puts the thread to sleep 
	//if(enter)
	//	if(m_fpsLimit > 0)
	//		sleep((CLOCKS_PER_SEC / m_fpsLimit) - (clock() - frameStart));

	frameStart = clock();

	enter = true;
}

void GameEmGine::clearObjectList() { m_models.clear(); }

void GameEmGine::setScene(Scene* scene)
{
	if(m_mainScene)
		m_mainScene->onSceneExit();

	LightManager::clear();
	m_models.clear();
	m_frameBuffers.clear();
	m_frameBuffers[m_gBuff->getTag()] = m_gBuff;
	scene->setParent(m_mainScene);//set the parent to the previous scene
	m_mainScene = scene;
	scene->init();
	InputManager::setKeyPressedCallback(scene->keyPressed);
	InputManager::setKeyReleasedCallback(scene->keyReleased);
	InputManager::setKeyAllCallback(scene->keyInput);
	InputManager::mouseButtonPressedCallback(scene->mousePressed);
	InputManager::mouseButtonReleasedCallback(scene->mouseReleased);
	InputManager::mouseButtonAllCallback(scene->mouseInput);

	customRenderCallback([&](FrameBuffer* gbuff, FrameBuffer* post, float dt)->void { if(m_mainScene->customPostEffects)m_mainScene->customPostEffects(gbuff, post, dt); });

	//m_render = scene->render;
	m_gameLoop = [&](double a)->void { m_mainScene->update(a); };
}

void GameEmGine::setBackgroundColour(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	m_colour = {GLubyte(r * 255), GLubyte(g * 255), GLubyte(b * 255), GLubyte(a * 255)};
}

int GameEmGine::getWindowWidth()
{
	return m_window->getScreenWidth();
}

int GameEmGine::getWindowHeight()
{
	return m_window->getScreenHeight();
}

Coord2D<int> GameEmGine::getWindowSize()
{
	return m_window->getScreenSize();
}

Camera* GameEmGine::getMainCamera()
{
	return m_mainCamera;
}

bool GameEmGine::mouseCollision(Model* model)
{
	Model* id = getMouseCollisionObject();
	if(!id)return false;

	return id->getID() == model->getID();
}

#include <cmath>
Model* GameEmGine::getMouseCollisionObject()
{

	Vec2 mPos = InputManager::getMousePosition();
	mPos.y = getWindowHeight() - mPos.y;

	uint id = 0;

	m_gBuff->enable();

	glReadBuffer(GL_COLOR_ATTACHMENT0 + 6);

	glReadPixels(int(mPos.x), int(mPos.y), 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &id);

	m_gBuff->disable();


	for(auto& a : m_models)
		if(a.second->getID() == id)
			return a.second;

	return nullptr;
}

void GameEmGine::setCameraType(Camera::CAM_TYPE type)
{
	m_mainCamera->setType(type, nullptr);
}

void GameEmGine::setCameraType(ProjectionPeramiters* proj)
{
	m_mainCamera->setType(proj);
}

void GameEmGine::translateCameraBy(Vec3 pos)
{
	m_mainCamera->translateBy(pos);
}

void GameEmGine::translateCamera(Vec3 pos)
{
	m_mainCamera->translate(pos);
}

void GameEmGine::rotateCameraBy(Vec3 direction)
{
	m_mainCamera->rotateBy(direction);
}

void GameEmGine::rotateCamera(Vec3 direction)
{
	m_mainCamera->rotate(direction);
}

void GameEmGine::addModel(Model* model)
{
	m_models[model] = model;
}

void GameEmGine::addText(Text* text)
{
	addModel(reclass(Model*, text));
}

void GameEmGine::removeModel(Model* model)
{
	if(model)
		m_models.erase(model);
}

void GameEmGine::removeText(Text* text)
{
	removeModel(reclass(Model*, text));
}

void GameEmGine::addCamera(Camera* cam)
{
	cam;

	//realloc(m_cameras, sizeof(Camera3D*)*++_numCameras);
	//m_cameras[_numCameras - 1] = cam;
}

std::unordered_map<void*, Model*>& GameEmGine::getObjectList()
{
	return m_models;
}


void GameEmGine::update()
{

	glClearDepth(1.f);

	FrameBuffer::clearBackBuffer();


	m_gBuff->clear();//colour buffer must be black
	m_gBuff->clearSingleColour(m_colour, 4);//this is the colour buffer
	m_gBuff->clearSingleColour({0, 0, 0, 0}, 6);//this is the ID buffer

	m_postBuffer->clear(NULL, m_colour);
	m_postBuffer->clear(NULL);

	m_mainCamera->update();

	m_gBufferShader->enable();
	glUniformMatrix4fv(m_gBufferShader->getUniformLocation("uView"), 1, GL_FALSE, &(m_mainCamera->getViewMatrix()[0][0]));
	glUniformMatrix4fv(m_gBufferShader->getUniformLocation("uProj"), 1, GL_FALSE, &(m_mainCamera->getProjectionMatrix()[0][0]));
	m_gBufferShader->disable();

	//m_forwardRender->enable();
	//glUniformMatrix4fv(m_forwardRender->getUniformLocation("uView"), 1, GL_FALSE, &(m_mainCamera->getViewMatrix()[0][0]));
	//glUniformMatrix4fv(m_forwardRender->getUniformLocation("uProj"), 1, GL_FALSE, &(m_mainCamera->getProjectionMatrix()[0][0]));
	//m_forwardRender->disable();


	LightManager::setCamera(m_mainCamera);
	LightManager::setGBuffer(m_gBuff);
	((SkyBox*)&m_mainScene->getSkyBox())->setCamera(m_mainCamera);


	glViewport(0, 0, getWindowWidth(), getWindowHeight());

	//Opaque renders 
	m_gBuff->enable();
	m_mainCamera->render(m_gBufferShader, m_models, false);
	m_gBuff->disable();

	//send depth info before rendering transparent objects
	m_gBuff->copyDepthToBuffer(getWindowWidth(), getWindowHeight(), m_postBuffer->getFrameBufferID());

	//sky box
	m_gBuff->enable();
	if(/*m_mainScene->skyBoxEnabled*/false)
		(*(SkyBox*)&m_mainScene->getSkyBox()).render();
	m_gBuff->disable();

	//transparent renders
	m_gBuff->enable();
	m_mainCamera->render(m_gBufferShader, m_models, true);
	m_gBuff->disable();




#pragma region Light Accumulation

	m_postBuffer->enable();
	//m_postProcessShader->enable();

	glDisable(GL_DEPTH_TEST);

	//Apply lighting
	//LightManager::setShader(m_postProcessShader);
	LightManager::setFramebuffer(m_postBuffer);
	LightManager::update();


	glEnable(GL_DEPTH_TEST);


	//m_postProcessShader->disable();
	m_postBuffer->disable();

	m_postBuffer->copySingleColourToBuffer(m_gBuff->getColourWidth(0), m_gBuff->getColourHeight(0), m_gBuff, 0, 5);

	m_postBuffer->clear();

#pragma endregion



	static Shader* composite = ResourceManager::getShader("Shaders/Main Buffer.vtsh", "Shaders/BloomComposite.fmsh");

	//store data for later post process
	m_postBuffer->enable();
	composite->enable();

	composite->sendUniform("uScene", 0);
	composite->sendUniform("uBloom", 1);

	m_gBuff->getColorTexture(4).bindTexture(0);
	m_gBuff->getColorTexture(5).bindTexture(1);

	FrameBuffer::drawFullScreenQuad();

	Texture2D::bindTexture(0, GL_NONE);
	Texture2D::bindTexture(1, GL_NONE);

	composite->disable();
	m_postBuffer->disable();

	//Apply shadows
	LightManager::setGBuffer(m_gBuff);
	LightManager::setCamera(m_mainCamera);
	LightManager::shadowRender(1820, 980, 50, m_postBuffer, m_models);


	m_postBuffer->setViewport(0, 0, 0);

	//post effects
	if(m_customRender)
		m_customRender(m_gBuff, m_postBuffer, (float)glfwGetTime());


	m_postBuffer->copyColourToBackBuffer(getWindowWidth(), getWindowHeight());
	m_postBuffer->copyDepthToBackBuffer(getWindowWidth(), getWindowHeight());



	if(m_gameLoop != nullptr)
		m_gameLoop(glfwGetTime());
	glfwPollEvents();//updates the event handlers
}

void GameEmGine::customRenderCallback(std::function<void(FrameBuffer*, FrameBuffer*, float dt)>render)
{
	m_customRender = render;
}

void GameEmGine::changeViewport(GLFWwindow*, int w, int h)
{
	if(!(w && h))return;
	m_screenSize = {w, h};
	glViewport(0, 0, w, h);

	//	switch(m_mainCamera->getType())
	//	{
	//	case Camera::FRUSTUM:
	//
	//		FrustumPeramiters* tmp = (FrustumPeramiters*)m_mainCamera->getProjectionData();
	//		if(tmp)
	//			tmp->aspect = (float)w / h;
	//		m_mainCamera->setType(m_mainCamera->getType(), tmp);
	//		break;
	//	}

		//Framebuffer Resizing 
	m_gBuff->resizeDepth(w, h);
	m_gBuff->resizeColour(0, w, h, GL_RGBA, GL_RGBA16F, GL_FLOAT);
	m_gBuff->resizeColour(1, w, h, GL_RGBA, GL_RGBA16F, GL_FLOAT);
	m_gBuff->resizeColour(2, w, h, GL_RGBA, GL_RGBA16F, GL_FLOAT);
	m_gBuff->resizeColour(3, w, h, GL_RGBA, GL_RGBA16F, GL_FLOAT);
	m_gBuff->resizeColour(4, w, h, GL_RGB, GL_RGB8);
	m_gBuff->resizeColour(5, w, h, GL_RGBA, GL_RGBA8);
	m_gBuff->resizeColour(6, w, h, GL_RED_INTEGER, GL_R32UI, GL_UNSIGNED_INT, GL_NEAREST);

	m_postBuffer->resizeDepth(w, h);
	m_postBuffer->resizeColour(0, w, h);

}


#define TEST false
#if TEST

class Test :public Scene
{
	void onSceneExit() {}
	//instance key is pressed
	void keyInputPressed(int key, int modifier)
	{
		modifier;
		m_left = (key == 'A' ? true : m_left);
		m_right = (key == 'D' ? true : m_right);
		m_up = (key == 'E' ? true : m_up);
		m_down = (key == 'Q' ? true : m_down);
		m_fwd = (key == 'W' ? true : m_fwd);
		m_back = (key == 'S' ? true : m_back);

		rotLeft = (key == GLFW_KEY_LEFT ? true : rotLeft);
		rotRight = (key == GLFW_KEY_RIGHT ? true : rotRight);
		rotUp = (key == GLFW_KEY_UP ? true : rotUp);
		rotDown = (key == GLFW_KEY_DOWN ? true : rotDown);

		//printf("key PRESSED code: %d\n\n", key);
	}

	ColourRGBA lastColour;

	//instance key is released
	void keyInputReleased(int key, int modifier)
	{
		modifier;
		m_left = (key == 'A' ? false : m_left);
		m_right = (key == 'D' ? false : m_right);
		m_fwd = (key == 'W' ? false : m_fwd);
		m_back = (key == 'S' ? false : m_back);
		m_up = (key == 'E' ? false : m_up);
		m_down = (key == 'Q' ? false : m_down);

		puts(GameEmGine::getMainCamera()->getLocalPosition().toString());

		rotLeft = (key == GLFW_KEY_LEFT ? false : rotLeft);
		rotRight = (key == GLFW_KEY_RIGHT ? false : rotRight);
		rotUp = (key == GLFW_KEY_UP ? false : rotUp);
		rotDown = (key == GLFW_KEY_DOWN ? false : rotDown);

		if(key == GLFW_KEY_TAB)
			++(*(char*)&currentState) %= (int)CONTROL_STATE::COUNT;//don't ask

		switch(currentState)
		{
		case CONTROL_STATE::CAMERA:
			//	modeStr = "Camera";
			break;
		case CONTROL_STATE::LEVEL:
			//	modeStr = "Level Editing";
			break;
		}

		//changes fps limit
		if(key == GLFW_KEY_KP_6)
			GameEmGine::setFPSLimit(util::clamp<short>(0, 60, GameEmGine::getFPSLimit() + 5));
		if(key == GLFW_KEY_KP_4)
			GameEmGine::setFPSLimit(util::clamp<short>(0, 60, GameEmGine::getFPSLimit() - 5));

		if(key == GLFW_KEY_F) //Toggles Full-screen
		{
			static bool full;
			GameEmGine::getWindow()->setFullScreen(full = !full);
			printf("Full Screen: %s\n", full ? "true" : "false");
		}

		if(key == GLFW_KEY_SPACE) //Changes the camera mode
		{
			static Camera::CAM_TYPE type = Camera::FRUSTUM;
			GameEmGine::setCameraType(type = type == Camera::ORTHOGRAPHIC ? Camera::FRUSTUM : Camera::ORTHOGRAPHIC);
		}

		if(key == GLFW_KEY_F5) //refreshes the shaders
			Shader::refresh();

		puts(GameEmGine::getMainCamera()->getLocalPosition().toString());

		//if(key == GLFW_KEY_Q)
		//	GameEmGine::lutActive = (GameEmGine::lutActive == false) ? true : false;
		//if(key == GLFW_KEY_T)
		//	GameEmGine::toonActive = (GameEmGine::toonActive == false) ? true : false;
		//if(key == 'R')
		//	GameEmGine::setCameraRotation({0,0,0});

		//printf("key RELEASED code: %d\n\n", key);
	}


	void objectMovement(double dt)
	{
		float moveSpd = 30 * (float)dt;
		moveSpd;
		if(m_left)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!tmod)return;
				tmod->translateBy(-moveSpd, 0, 0);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->translateBy(-moveSpd, 0, 0);
				break;
			}
		if(m_right)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!tmod)return;
				tmod->translateBy(moveSpd, 0, 0);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->translateBy(moveSpd, 0, 0);
				break;
			}
		if(m_fwd)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!tmod)return;
				tmod->translateBy(0, 0, moveSpd);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->translateBy(0, 0, moveSpd);
				break;
			}
		if(m_back)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!tmod)return;
				tmod->translateBy(0, 0, -moveSpd);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->translateBy(0, 0, -moveSpd);
				break;
			}
		if(m_up)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!tmod)return;
				tmod->translateBy(0, moveSpd, 0);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->translateBy(0, moveSpd, 0);
				break;
			}
		if(m_down)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!tmod)return;
				tmod->translateBy(0, -moveSpd, 0);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->translateBy(0, -moveSpd, 0);
				break;
			}

		if(rotLeft)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!tmod)return;
				tmod->rotateBy(0, -moveSpd, 0);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->rotateBy(0, -moveSpd, 0);
				break;
			}
		if(rotRight)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!tmod)return;
				tmod->rotateBy(0, moveSpd, 0);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->rotateBy(0, moveSpd, 0);
				break;
			}
		if(rotUp)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				//this is correct
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!tmod)return;
				tmod->rotateBy(moveSpd, 0, 0);
				break;
			case CONTROL_STATE::CAMERA:
				//this is correct
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->rotateBy(moveSpd, 0, 0);
				break;
			}
		if(rotDown)
			switch(currentState)
			{
			case CONTROL_STATE::GAME:
				break;
			case CONTROL_STATE::LEVEL:
				GameEmGine::getMainCamera()->enableFPSMode(false); if(!tmod)return;
				tmod->rotateBy(-moveSpd, 0, 0);
				break;
			case CONTROL_STATE::CAMERA:
				GameEmGine::getMainCamera()->enableFPSMode();
				GameEmGine::getMainCamera()->rotateBy(-moveSpd, 0, 0);
				break;
			}


	}
	void init()
	{
		GameEmGine::addModel(tmod = new Model(new PrimitiveSphere(200, 200, 50, 20, {0, 100, 0})));
		tmod->setColour(1, 0, 0);
		GameEmGine::addModel(tmod2 = new Model(new PrimitivePlane(1000, 0, 1000)));
		tmod2->setColour(.1f, .1f, .1f);
		GameEmGine::getMainCamera()->translate(0, 0, -500);

		static Light light;
		light.setLightType(Light::TYPE::DIRECTIONAL);
		light.rotate({-45});
		LightManager::addLight(&light);

		keyPressed = [&](int a, int b) { keyInputPressed(a, b); };
		keyReleased = [&](int a, int b) { keyInputReleased(a, b); };

	}

	void update(double dt) { objectMovement(dt); }

	Model* tmod = nullptr, * tmod2 = nullptr;
	bool m_left = 0, m_right = 0, m_fwd = 0, m_back = 0, m_up = 0, m_down = 0,
		rotLeft = 0, rotRight = 0, rotUp = 0, rotDown = 0;
	enum class CONTROL_STATE :char
	{
		CAMERA,
		LEVEL,
		GAME,

		COUNT,
	}currentState = CONTROL_STATE::CAMERA;

};

int main()
{
	GameEmGine::init("Test", 1820, 980, 0, 0, 0, false);

	Test test;

	GameEmGine::setScene(&test);
	GameEmGine::run();

	return 0;
}
#endif // TEST