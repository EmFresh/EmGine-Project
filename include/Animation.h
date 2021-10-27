#pragma once
#include <vector>
#include <string>
#include <ctime>
#include <memory>
#include "Model.h"
#include "Shader.h"

class Model;

class Animation
{
public:
	Animation();
	~Animation();


	void addFrame(Model* frame, float speed = 1);
	void addDir(cstring dir);

	//setts the delay between the animations
	void setAnimationSpeed(float speed);

	void update(Shader* shader, Model* mesh);

	int getFrameNumber();

	int getTotalFrames();

	bool hasEnded();

	bool checkPlay();

	void stop();

	void play();

	void pause();

	void repeat(bool repeat);
private:
	bool m_pause = 0, m_stop = 0, m_repeat = 0;

	std::vector<std::shared_ptr<Model>> m_frames;
	//std::vector<std::vector<std::vector<Vertex3D>>> m_unpackedData;

	bool init = false;
	float m_speed, m_lastTime, m_time;
	unsigned m_frame = 0, m_frameNext;
};

