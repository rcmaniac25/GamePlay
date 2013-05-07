#include "Base.h"
#include "VRController.h"
#include "Game.h"
#include "HMD.h"
#include "VRSensor.h"
#include "VRLatencyTester.h"
#include "Camera.h"
#include "Node.h"
#include "Scene.h"

#include <iterator>

#ifdef USE_OCULUS
#include <OVR.h>
#endif

namespace gameplay
{

//Helper classes
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

//VR Allocator
class OculusAllocator : public OVR::Allocator
{
public:
	OculusAllocator() : OVR::Allocator(), allocatedMemory(0)
	{
	}

	~OculusAllocator()
	{
	}

	//Not sure if this is the proper way to do this (allocating an array, that is)...

	void* Alloc(OVR::UPInt size)
	{
		char* data = new char[size + sizeof(size_t)];
		*((size_t*)data) = (size_t)size;
		allocatedMemory += size;
		return (void*)(data + sizeof(size_t));
	}

	void* AllocDebug(OVR::UPInt size, const char* file, unsigned line)
	{
#ifdef GAMEPLAY_MEM_LEAK_DETECTION
		char* data = new(file, line) char[size + sizeof(size_t)];
		*((size_t*)data) = (size_t)size;
		allocatedMemory += size;
		return (void*)(data + sizeof(size_t));
#else
		return Alloc(size);
#endif
	}

	void* Realloc(void* p, OVR::UPInt newSize)
	{
		void* nd = Alloc(newSize);
		if(p)
		{
			char* d =  (char*)p;
			d -= sizeof(size_t);
			size_t orgSize = *((size_t*)d);
			memcpy(nd, p, orgSize);
			delete [] d;
#ifdef GAMEPLAY_MEM_LEAK_DETECTION
			if(allocatedMemory < orgSize)
			{
				gameplay::print("OculusVR is trying to reallocate a pointer that has an invalid size.\n");
			}
#endif
			allocatedMemory -= orgSize;
		}
		return nd;
	}

	void Free(void* p)
	{
		if(p)
		{
			char* d = (char*)p;
			d -= sizeof(size_t);
			size_t orgSize = *((size_t*)d);
			delete [] d;
#ifdef GAMEPLAY_MEM_LEAK_DETECTION
			if(allocatedMemory < orgSize)
			{
				gameplay::print("OculusVR is trying to free more memory then is allocated.\n");
			}
#endif
			allocatedMemory -= orgSize;
		}
	}

	//Use default AllocAligned
	//void* AllocAligned(OVR::UPInt size, OVR::UPInt align)
	//{
	//	//TODO
	//}

	//void FreeAligned(void* p)
	//{
	//	//TODO
	//}

protected:
	void onSystemShutdown()
	{
#ifdef GAMEPLAY_MEM_LEAK_DETECTION
		if(allocatedMemory > 0)
		{
			gameplay::print("OculusVR still has %u bytes allocated.\n", (unsigned int)allocatedMemory);
		}
#endif
	}

private:
	size_t allocatedMemory;
};

//VR Data
class OculusDeviceHandler;
class OculusLatencyHandler;

class OculusData
{
public:
	OculusData() : log(NULL), alloc(NULL), ovrSystem(NULL), holdsLocks(false), devices(), ovrDevices(), devManager(), 
		devHandler(NULL), latencyHandler(NULL),
		activeHMD(NULL), activeCamera(NULL), oriViewport()
	{
	}

	~OculusData()
	{
	}

	OculusLog* log;
	OculusAllocator* alloc;
	OVR::System* ovrSystem;
	bool holdsLocks;
	std::map<VRDevice::VRTypes, std::vector<VRDevice*> > devices;
	std::vector<OVR::Ptr<OVR::DeviceBase> > ovrDevices;
	OVR::Ptr<OVR::DeviceManager> devManager;
	OculusDeviceHandler* devHandler;
	OculusLatencyHandler* latencyHandler;

	HMD* activeHMD;
	Camera* activeCamera;
	Rectangle oriViewport;
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

class OculusLatencyHandler : public OVR::MessageHandler, public VRHandler
{
public:
	OculusLatencyHandler() : OVR::MessageHandler(), _testDevices(), _testers()
	{
	}

	~OculusLatencyHandler()
	{
		RemoveHandlerFromDevices();
	}

	void OnMessage(const OVR::Message& msg)
	{
		//Standard C++ method of doing the below loop
		//unsigned int i = std::distance(std::find(_testDevices.begin(), _testDevices.end(), static_cast<OVR::LatencyTestDevice*>(msg.pDevice)), _testDevices.begin());

		OVR::LatencyTestDevice* testDev = static_cast<OVR::LatencyTestDevice*>(msg.pDevice);

		//Determine the index of the device we are testing
		unsigned int i = 0;
		for(std::vector<OVR::LatencyTestDevice*>::iterator it = _testDevices.begin(); it != _testDevices.end(); it++, i++)
		{
			if(*it == testDev)
			{
				break;
			}
		}

		//Handle the message
		VRDevice::VREvent ev = VRDevice::DISCONNECTED_EVENT;
		switch(msg.Type)
		{
			case OVR::Message_LatencyTestSamples:
				ev = VRDevice::LATENCY_TESTER_SAMPLES_EVENT;
				break;
			case OVR::Message_LatencyTestColorDetected:
				ev = VRDevice::LATENCY_TESTER_COLOR_DETECTED_EVENT;
				break;
			case OVR::Message_LatencyTestStarted:
				ev = VRDevice::LATENCY_TESTER_STARTED_EVENT;
				break;
			case OVR::Message_LatencyTestButton:
				ev = VRDevice::LATENCY_TESTER_BUTTON_EVENT;
				break;
		}
		if(ev != VRDevice::DISCONNECTED_EVENT)
		{
			Platform::vrEventInternal(ev, _testers[i]);
		}
	}

	bool SupportsMessageType(OVR::MessageType type) const
	{
		return type == OVR::Message_LatencyTestSamples || type == OVR::Message_LatencyTestColorDetected || type == OVR::Message_LatencyTestStarted || type == OVR::Message_LatencyTestButton;
	}

	void AddDevice(VRDevice* device)
	{
		if(device)
		{
			OVR::Lock::Locker lockedScope(this->GetHandlerLock());
			VRLatencyTester* tester = static_cast<VRLatencyTester*>(device);

			std::vector<VRLatencyTester*>::iterator it = std::find(_testers.begin(), _testers.end(), tester);
			if(it == _testers.end())
			{
				//Tester doesn't exist
				_testers.push_back(tester);

				//It's expected that any currently set listener will be removed
				OVR::LatencyTestDevice* testDev = static_cast<OVR::LatencyTestDevice*>(VRController::getDeviceHandle(tester));
				testDev->SetMessageHandler(this);
				_testDevices.push_back(testDev);
			}
		}
	}

	void RemoveDevice(VRDevice* device)
	{
		if(device)
		{
			OVR::Lock::Locker lockedScope(this->GetHandlerLock());
			std::vector<VRLatencyTester*>::iterator it = std::find(_testers.begin(), _testers.end(), static_cast<VRLatencyTester*>(device));
			if(it != _testers.end())
			{
				//Tester exists
				unsigned int index = std::distance(_testers.begin(), it);

				//Remove from testers
				_testers.erase(it);

				//Remove from OVR devices
				std::vector<OVR::LatencyTestDevice*>::iterator ovrIt = _testDevices.begin() + index;
				(*ovrIt)->SetMessageHandler(NULL);
				_testDevices.erase(ovrIt);
			}
		}
	}

	void RemoveAllDevices()
	{
		OVR::Lock::Locker lockedScope(this->GetHandlerLock());
		for(int i = _testDevices.size() - 1; i >= 0; i--)
		{
			_testDevices[i]->SetMessageHandler(NULL);
			_testDevices.erase(_testDevices.begin() + i);
			_testers.erase(_testers.begin() + i);
		}
	}

private:
	std::vector<OVR::LatencyTestDevice*> _testDevices;
	std::vector<VRLatencyTester*> _testers;
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

void* VRController::getDeviceHandle(VRDevice* device)
{
#ifdef USE_OCULUS
	if(device)
	{
		return device->_deviceHandle;
	}
#endif
	return NULL;
}

void VRController::disableAllEvents()
{
#ifdef USE_OCULUS
	//TODO

	//Disable latency tester events
	if(_data->latencyHandler)
	{
		_data->latencyHandler->RemoveAllDevices();
	}
#endif
}

void VRController::initialize()
{
#ifdef USE_OCULUS
	//Setup log
	_data->log = new OculusLog();

	//Setup allocator
	_data->alloc = new OculusAllocator();

	//Load Oculus VR support
	_data->ovrSystem = new OVR::System(_data->log, _data->alloc);

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

	SAFE_DELETE(_data->latencyHandler);
	SAFE_DELETE(_data->devHandler);
	_data->devManager.Clear();
	SAFE_DELETE(_data->ovrSystem);
	SAFE_DELETE(_data->alloc);
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
							//TODO: remove event
							//TODO: Remove the _data->devices device and make sure that if this is the active HMD that it is reset it (don't forget activeCamera)
							//TODO: If a latency tester, make sure it's removed from latency handler (if it's setup)
							//TODO: remove from _devices
							cDev.Clear();
							_data->ovrDevices.erase(it);
							break;
						}
					}
				}
			}*/
		}
	} while(em.Next());

	/* We have the ability to get cameras from the HMD, don't remove them
	//Check for removed cameras
	std::vector<VRDevice*>& hmds = _data->devices[VRDevice::HMD_TYPE];
	for(std::vector<VRDevice*>::iterator it = hmds.begin(); it != hmds.end(); it++)
	{
		HMD* hmd = static_cast<HMD*>(*it);

		//Find unused cameras
		std::vector<unsigned int> toRemove;
		unsigned int index = 0;
		for(std::vector<HMD::HMDCamera>::iterator cit = hmd->_cameras.begin(); cit != hmd->_cameras.end(); cit++, index++)
		{
			if((*cit).cam->getRefCount() == 1)
			{
				//If there is only one reference, then we control the reference, so we can remove it since no one will ever reactivate it
				toRemove.push_back(index);
			}
		}

		//Remove unused cameras
		for(int i = toRemove.size() - 1; i >= 0; i--)
		{
			hmd->_cameras.erase(hmd->_cameras.begin() + i);
		}
	}
	*/
#endif
}

static unsigned int getRenderIteration(HMD::RenderMode mode)
{
	switch(mode)
	{
		case HMD::MONO_MODE:
			//Just end since it's one render
			break;
		//case HMD::STEREO_MODE:
		//	return 2; //XXX Not sure yet
		case HMD::LEFT_RIGHT_MODE:
			return 2;
	}
	return 1;
}

unsigned int VRController::renderIterationCount()
{
#ifdef USE_OCULUS
	//If we already have a camera, use it
	if(_data->activeHMD)
	{
		return getRenderIteration(_data->activeHMD->getRenderMode());
	}

	//Look for cameras
	bool foundActive = false;
	std::vector<VRDevice*>& hmds = _data->devices[VRDevice::HMD_TYPE];
	for(std::vector<VRDevice*>::iterator it = hmds.begin(); !foundActive && it != hmds.end(); it++)
	{
		HMD* hmd = static_cast<HMD*>(*it);

		for(std::vector<HMD::HMDCamera>::iterator cit = hmd->_cameras.begin(); cit != hmd->_cameras.end(); cit++)
		{
			HMD::HMDCamera& hmdCam = *cit;
			Node* camNode = hmdCam.cam->getNode();
			if(camNode)
			{
				//Check to see if this is the active camera
				if(camNode->getScene()->getActiveCamera() == hmdCam.cam)
				{
					//Found the active camera
					foundActive = true;

					//Cache values
					_data->activeHMD = hmd;
					_data->activeCamera = NULL;
					_data->oriViewport = Game::getInstance()->getViewport();

					if(!hmdCam.enabled)
					{
						//Simple exit loop, just doing a single render
						break;
					}

					_data->activeCamera = hmdCam.cam;

					//Determine what kind of rendering loop needs to be performed
					return getRenderIteration(hmd->getRenderMode());
				}
			}
		}
	}
#endif
	//Only do one render
	return 1;
}

void VRController::prepareRender(unsigned int index)
{
#ifdef USE_OCULUS
	if(_data->activeHMD)
	{
		//TODO: Get adjustment info from HMD. Adjust matricies on active camera if there is one.
	}
#endif
}

void VRController::finalizeRender(unsigned int index)
{
#ifdef USE_OCULUS
	if(_data->activeHMD)
	{
		//TODO: distort camera to screen backbuffer. If last index, restore viewport
	}
#endif
}

void VRController::lockMessages()
{
#ifdef USE_OCULUS
	if(!_data->holdsLocks)
	{
		_data->holdsLocks = true;
		//_data->devHandler->GetHandlerLock()->DoLock(); //XXX see explanation in pollDevices
		if(_data->latencyHandler)
		{
			_data->devHandler->GetHandlerLock()->DoLock();
		}
		//TODO: lock message handlers so that we can update without values changing
	}
#endif
}

void VRController::releaseMessages()
{
#ifdef USE_OCULUS
	if(_data->holdsLocks)
	{
		//Reset the cached HMD so that it's recalculated next frame
		_data->activeHMD = NULL;
		_data->activeCamera = NULL;
		_data->oriViewport = Rectangle(); //Reset

		//TODO: release message handlers so that we can update without values changing
		if(_data->latencyHandler)
		{
			_data->devHandler->GetHandlerLock()->Unlock();
		}
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
		//Get the total number of devices
		unsigned int c = 0;
		for(it = _data->devices.begin(); it != _data->devices.end(); it++)
		{
			c += it->second.size();
		}
		return c;
	}

	//Get the cound based on type
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
		//Get any device, purely by index
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

	//Get the device based on type
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

VRHandler* VRController::getLatencyHandler()
{
#ifdef USE_OCULUS
	if(!_data->latencyHandler)
	{
		_data->latencyHandler = new OculusLatencyHandler();
	}
	return _data->latencyHandler;
#else
	return NULL;
#endif
}

}
