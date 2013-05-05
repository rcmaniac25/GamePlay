#include "Base.h"
#include "VRController.h"
#include "Game.h"
#include "HMD.h"
#include "VRSensor.h"
#include "VRLatencyTester.h"

#ifdef USE_OCULUS
#include <OVR.h>
#endif

namespace gameplay
{

//Helper class
#ifdef USE_OCULUS
//VR Log
class OculusLog : public OVR::Log
{
public:
	OculusLog() : OVR::Log() //Go with defaults for now
	{
	}

	~OculusLog()
	{
	}

	void LogMessageVarg(OVR::LogMessageType messageType, const char* message, va_list args)
	{
		// Taken from Logger

		// Could use OVR's logging system to format the logs, but we've already formatted the text and don't want to reformat it

		// Declare a moderately sized buffer on the stack that should be
		// large enough to accommodate most log requests.
		int size = 1024;
		char stackBuffer[1024];
		std::vector<char> dynamicBuffer;
		char* str = stackBuffer;
		for ( ; ; )
		{
			// Pass one less than size to leave room for NULL terminator
			int needed = vsnprintf(str, size-1, message, args);

			// NOTE: Some platforms return -1 when vsnprintf runs out of room, while others return
			// the number of characters actually needed to fill the buffer.
			if (needed >= 0 && needed < size)
			{
				// Successfully wrote buffer. Added a NULL terminator in case it wasn't written.
				str[needed] = '\0';
				break;
			}

			size = needed > 0 ? (needed + 1) : (size * 2);
			dynamicBuffer.resize(size);
			str = &dynamicBuffer[0];
		}

		//Write to log
		Logger::Level level = Logger::LEVEL_INFO;
		const char* logMessage = "OculusVR: %s";
		switch(messageType)
		{
			//Handle some special log cases
			case OVR::Log_Error:
				level = Logger::LEVEL_ERROR;
				logMessage = "OculusVR-Error: %s\n";
				break;
			case OVR::Log_Debug:
				level = Logger::LEVEL_WARN;
				logMessage = "OculusVR-Debug: %s\n";
				break;
			case OVR::Log_Assert:
				level = Logger::LEVEL_WARN;
				logMessage = "OculusVR-Assert: %s\n";
				break;
		}
		Logger::log(level, logMessage, str);
	}
};

//VR Data
class OculusDeviceHandler;

class OculusData
{
public:
	OculusData() : log(NULL), ovrSystem(NULL), holdsLocks(false), devices(), ovrDevices(), devManager(), devHandler(NULL)
	{
	}

	~OculusData()
	{
	}

	OculusLog* log;
	OVR::System* ovrSystem;
	bool holdsLocks;
	std::map<VRDevice::VRTypes, std::vector<VRDevice*> > devices;
	std::vector<OVR::Ptr<OVR::DeviceBase> > ovrDevices;
	OVR::Ptr<OVR::DeviceManager> devManager;
	OculusDeviceHandler* devHandler;
};

//VR Device handler
class OculusDeviceHandler : public OVR::MessageHandler
{
public:
	OculusDeviceHandler(OculusData* d) : OVR::MessageHandler(), data(d)
	{
	}

	~OculusDeviceHandler()
	{
		RemoveHandlerFromDevices();
	}

	void OnMessage(const OVR::Message& msg)
	{
		//Determine message type
		VRDevice::VREvent evt = VRDevice::DISCONNECTED_EVENT;
		if(msg.Type == OVR::Message_DeviceAdded)
		{
			evt = VRDevice::CONNECTED_EVENT;
		}
		//Platform::vrEventInternal(evt, ??);
	}

	bool SupportsMessageType(OVR::MessageType type) const
	{
		return type == OVR::Message_DeviceAdded || type == OVR::Message_DeviceRemoved;
	}

private:
	OculusData* data;
};
#endif

VRController::VRController() : 
#ifdef USE_OCULUS
	_data(new OculusData())
#else
	_data(NULL)
#endif
{
}

VRController::~VRController()
{
#ifdef USE_OCULUS
	SAFE_DELETE(_data);
#endif
}

void VRController::disableAllEvents()
{
	//TODO
}

void VRController::initialize()
{
#ifdef USE_OCULUS
	//Setup log
	_data->log = new OculusLog();

	//Load Oculus VR support
	_data->ovrSystem = new OVR::System(_data->log);

	//Get device manager (note: make sure to have the * in there. "= OVR::DeviceManager::Create();" will have an incorrect reference count and when the program exits, it will not work)
	_data->devManager = *OVR::DeviceManager::Create();

	//Setup device handler (XXX not useful right now, see explanation in pollDevices)
	//_data->devHandler = new OculusDeviceHandler(_data);
	//_data->devManager->SetMessageHandler(_data->devHandler);
#endif
}

void VRController::finalize()
{
#ifdef USE_OCULUS
	releaseMessages(); //Precaution

	//Cleanup devices
	for(std::map<VRDevice::VRTypes, std::vector<VRDevice*> >::iterator dIt = _data->devices.begin(); dIt != _data->devices.end(); dIt++)
	{
		for(std::vector<VRDevice*>::iterator it = dIt->second.begin(); it != dIt->second.end(); it++)
		{
			SAFE_DELETE(*it);
		}
		dIt->second.clear();
	}

	//Cleanup ovrDevices
	_data->ovrDevices.clear();

	SAFE_DELETE(_data->devHandler);
	_data->devManager.Clear();
	SAFE_DELETE(_data->ovrSystem);
	SAFE_DELETE(_data->log);
#endif
}

void VRController::pollDevices()
{
#ifdef USE_OCULUS
	OVR::Ptr<OVR::DeviceBase> dev;
	OVR::DeviceInfo info;

	//Check for devices (XXX this mimics a DeviceManager's message handler but actually does the polling instead of "hoping" the DeviceManager does it. Hopefully this changes in a future SDK update. If so, update this code appropriately)
	OVR::DeviceEnumerator<OVR::DeviceBase> em = _data->devManager->EnumerateDevices<OVR::DeviceBase>(false);
	do
	{
		OVR::DeviceType type = em.GetType();
		if(type == OVR::Device_HMD || type == OVR::Device_Sensor || type == OVR::Device_LatencyTester)
		{
			if(em.IsAvailable())
			{
				if(!em.IsCreated())
				{
					//Create OVR device
					dev = *em.CreateDevice();
					void* voidDev = static_cast<void*>(dev.GetPtr());

					//Create GamePlay VR device
					VRDevice* vrDev = NULL;
					VRDevice::VRTypes vrType = VRDevice::ALL_TYPES;
					switch(type)
					{
						case OVR::Device_HMD:
							vrType = VRDevice::HMD_TYPE;
							vrDev = new HMD(voidDev);
							break;
						case OVR::Device_Sensor:
							vrType = VRDevice::SENSOR_TYPE;
							vrDev = new VRSensor(voidDev);
							break;
						case OVR::Device_LatencyTester:
							vrType = VRDevice::LATENCY_TESTER_TYPE;
							vrDev = new VRLatencyTester(voidDev);
							break;
					}
					if(vrDev)
					{
						//Save results if successful
						_data->ovrDevices.push_back(dev);
						_data->devices[vrType].push_back(vrDev);
						Platform::vrEventInternal(VRDevice::CONNECTED_EVENT, vrDev);
					}
				}
			}
			//TODO: Need to determine how to remove devices
			/*else if(em.IsCreated())
			{
				dev = *em.CreateDevice(); //Since the device is already created, this will simply add an additional reference which will be released when the Ptr is disposed or the value is replaced.
				if(dev.GetPtr()) //Avoid NULL pointes
				{
					for(std::vector<OVR::Ptr<OVR::DeviceBase> >::iterator it = _data->ovrDevices.begin(); it != _data->ovrDevices.end(); it++)
					{
						OVR::Ptr<OVR::DeviceBase>& cDev = *it;
						if(cDev.GetPtr() == dev.GetPtr())
						{
							//TODO: remove
							cDev.Clear();
							_data->ovrDevices.erase(it);
							break;
						}
					}
				}
			}*/
		}
	} while(em.Next());
#endif
}

bool VRController::shouldRenderStereo()
{
#ifdef USE_OCULUS
	//TODO
	return false;
#else
	return false;
#endif
}

void VRController::prepareRender(unsigned int index)
{
#ifdef USE_OCULUS
	//TODO: Get cameras, determine if any are active. Find which HMD it matches with. Get adjustment info from HMD. Adjust matricies on active camera
#endif
}

void VRController::finalizeRender(unsigned int index)
{
#ifdef USE_OCULUS
	//TODO: distort camera to screen backbuffer
#endif
}

void VRController::lockMessages()
{
#ifdef USE_OCULUS
	if(!_data->holdsLocks)
	{
		_data->holdsLocks = true;
		//_data->devHandler->GetHandlerLock()->DoLock(); //XXX see explanation in pollDevices
		//TODO: lock message handlers so that we can update without values changing
	}
#endif
}

void VRController::releaseMessages()
{
#ifdef USE_OCULUS
	if(_data->holdsLocks)
	{
		//TODO: release message handlers so that we can update without values changing
		//_data->devHandler->GetHandlerLock()->Unlock(); //XXX see explanation in pollDevices
		_data->holdsLocks = false;
	}
#endif
}

unsigned int VRController::getDeviceCount(VRDevice::VRTypes type) const
{
#ifdef USE_OCULUS
	std::map<VRDevice::VRTypes, std::vector<VRDevice*> >::iterator it;
	if(type == VRDevice::ALL_TYPES)
	{
		unsigned int c = 0;
		for(it = _data->devices.begin(); it != _data->devices.end(); it++)
		{
			c += it->second.size();
		}
		return c;
	}
	it = _data->devices.find(type);
	if(it == _data->devices.end())
	{
		return 0;
	}
	return it->second.size();
#else
	return 0;
#endif
}

VRDevice* VRController::getDevice(unsigned int index, VRDevice::VRTypes type) const
{
#ifdef USE_OCULUS
	if(type == VRDevice::ALL_TYPES)
	{
		unsigned int c = 0;
		for(std::map<VRDevice::VRTypes, std::vector<VRDevice*> >::iterator it = _data->devices.begin(); it != _data->devices.end(); it++)
		{
			if(index >= c && index < (c + it->second.size()))
			{
				return it->second[index - c];
			}
			c += it->second.size();
		}
		return NULL;
	}
	std::map<VRDevice::VRTypes, std::vector<VRDevice*> >::iterator it = _data->devices.find(type);
	if(it == _data->devices.end())
	{
		return NULL;
	}
	if(it->second.size() <= index)
	{
		return NULL;
	}
	return it->second[index];
#else
	return NULL;
#endif
}

}
