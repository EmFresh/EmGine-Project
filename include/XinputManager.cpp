#include "XinputManager.h"

XinputDevice* m_controllers[4];

void XinputManager::update()
{

	for(int index = 0; index < 4; index++)
	{
		//check if controller is null
		if(!m_controllers[index])
			m_controllers[index] = new XinputDevice;

		CONTROLLER_TYPE tmpType = getControllerType(index);
		if(tmpType != m_controllers[index]->type)
		{
			delete m_controllers[index];
			switch(tmpType)
			{
			case XINPUT_CONTROLLER:
				m_controllers[index] = new XinputController;
				break;
			case XINPUT_GUITAR:
				m_controllers[index] = new XinputGuitar;
				break;
			case XINPUT_DRUM:
				m_controllers[index] = new XinputDrum;
				break;
			default:
				m_controllers[index] = new XinputDevice;
			}
			m_controllers[index]->type = tmpType;
		}

		//check if each controller is still connected
		if(controllerConnected(index))
		{
			m_controllers[index]->m_index = index;
			m_controllers[index]->update();
		}
	}
}

bool XinputManager::controllerConnected(int index)
{
	XINPUT_STATE connected;
	return XInputGetState(index, &connected) == ERROR_SUCCESS;
}

XinputDevice* XinputManager::getController(int index)
{
	return m_controllers[index];
}

CONTROLLER_TYPE XinputManager::getControllerType(int index)
{
	static XINPUT_CAPABILITIES info;
	XInputGetCapabilities(index, NULL, &info);

	switch(info.SubType)
	{
	case XINPUT_DEVSUBTYPE_GAMEPAD:
		return XINPUT_CONTROLLER;
	case XINPUT_DEVSUBTYPE_GUITAR:
		return XINPUT_GUITAR;
	case XINPUT_DEVSUBTYPE_GUITAR_ALTERNATE:
		return XINPUT_GUITAR;
	case XINPUT_DEVSUBTYPE_GUITAR_BASS:
		return XINPUT_GUITAR;
	case XINPUT_DEVSUBTYPE_DRUM_KIT:
		return XINPUT_DRUM;
	}

	return XINPUT_UNKNOWN;
}
