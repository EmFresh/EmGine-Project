#include "Animation.h"
#include <filesystem>
namespace fs = std::filesystem;
using std::shared_ptr;

Animation::Animation()
{}

Animation::~Animation()
{}

//adds model to the end of the frame list
void Animation::addFrame(Model* frame, float speed)
{
	m_frames.push_back(shared_ptr<Model>(new Model(*frame))), speed;
}

//time between frames(will fix later)
void Animation::setAnimationSpeed(float speed)
{
	m_speed = speed;
}

//adds directory of Morph-Target models in Name order
void Animation::addDir(cstring dir)
{
	std::string path(dir);
	//path += fileName;
	auto filePathData = fs::directory_iterator(path);
	//for(auto& a : m_frames)
	//	delete a;
	//m_frames.clear();
	for(auto& a : filePathData)
	{
		std::wstring tmpPath = a.path();
		int check = (int)tmpPath.find(L".obj");
		if(check < 0)continue;

		std::string str;
		for(auto& b : tmpPath)//wstring to sring
			str += (char)b;

		m_frames.push_back(shared_ptr<Model>(new Model(str.c_str())));
	}
}

void Animation::update(Shader* shader, Model* mesh)
{
	float time = (float)clock() / CLOCKS_PER_SEC;

	if(!init)
	{
		m_lastTime = time;
		init = true;
	}

	if(m_speed <= 0)
		return;

	if(!m_pause && !m_stop)
	{
		if(mesh)
			if((time = (time - m_lastTime)) >= m_speed)
			{
				if(m_repeat)
				{
					mesh->editVerts(m_frames[m_frame = int(time / m_speed) % m_frames.size()].get(), m_frames[m_frameNext = (m_frame + 1) % m_frames.size()].get());
				}
				else
				{
					m_frame = int(time / m_speed);
					m_frame = m_frame >= m_frames.size() - 1 ? unsigned((m_frames.size() - 2) % m_frames.size()) : m_frame;

					if(m_frame < m_frames.size() - 2)
						mesh->editVerts(m_frames[m_frame].get(), m_frames[m_frameNext = (m_frame + 1) % m_frames.size()].get());
					else
						mesh->editVerts(m_frames[m_frame].get(), m_frames[m_frameNext = m_frame].get());
				}

			}
	}
	else
	{
		if(mesh)
			if((time = (time - m_lastTime)) >= m_speed)
			{
				if(m_pause)
				{
					mesh->editVerts(m_frames[m_frame].get(), m_frames[m_frame].get());
					m_lastTime = time;
				}
				else
					if(m_stop)
					{
						mesh->editVerts(m_frames[0].get(), m_frames[0].get());
						m_lastTime = time;
					}
			}
	}

	//printf("%f\n\n", fmodf(time / m_speed, 1));
	shader->enable();
	glUniform1f(shader->getUniformLocation("uTime"), fmodf(time / m_speed, 1));
	shader->disable();
}

int Animation::getFrameNumber()
{
	return m_frame;
}
int Animation::getTotalFrames()
{
	return(int) m_frames.size();
}
//checks if animation has ended if repeat is disabled
bool Animation::hasEnded()
{
	return m_frame == m_frames.size() && !m_repeat;
}

bool Animation::checkPlay()
{
	return !m_pause;
}

void Animation::stop()
{
	m_frame = 0;
	m_stop = true;
}

void Animation::play()
{
	m_pause = m_stop = false;
}

void Animation::pause()
{
	m_pause = true;
}

void Animation::repeat(bool repeat)
{
	m_repeat = repeat;
}
