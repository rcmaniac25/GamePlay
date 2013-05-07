#include "Base.h"
#include "VRLatencyTester.h"
#include "Game.h"

#ifdef USE_OCULUS
#include <OVR.h>
#endif

namespace gameplay
{

VRLatencyTester::VRLatencyTester(void* handle) : VRDevice(handle), 
	_threshold(Vector4::zero()), _sendSamples(false), _calibrate(Vector4::zero()),
	_testUtility(NULL)
{
#ifdef USE_OCULUS
	if(handle)
	{
		const OVR::Color& tCol = OVR::Color();

		OVR::LatencyTestConfiguration config = OVR::LatencyTestConfiguration(tCol);
		OVR::LatencyTestDevice* testDev = static_cast<OVR::LatencyTestDevice*>(handle);
		testDev->GetConfiguration(&config);

		config.Threshold.GetRGBA(&_threshold.x, &_threshold.y, &_threshold.z, &_threshold.w);
		_sendSamples = config.SendSamples;

		OVR::LatencyTestCalibrate cali = OVR::LatencyTestCalibrate(tCol);
		testDev->GetCalibrate(&cali);
		cali.Value.GetRGBA(&_calibrate.x, &_calibrate.y, &_calibrate.z, &_calibrate.w);
	}
#endif
}

VRLatencyTester::~VRLatencyTester()
{
#ifdef USE_OCULUS
	OVR::Util::LatencyTest* testUtility = static_cast<OVR::Util::LatencyTest*>(_testUtility);
	SAFE_DELETE(testUtility);
#endif
	_testUtility = NULL;
}

VRDevice::VRTypes VRLatencyTester::getType() const
{
	return VRDevice::LATENCY_TESTER_TYPE;
}

const Vector4& VRLatencyTester::getConfigurationThreshold() const
{
	return _threshold;
}

bool VRLatencyTester::getConfigurationSendSamples() const
{
	return _sendSamples;
}

bool VRLatencyTester::setConfiguration(const Vector4& threshold, bool streaming, bool wait)
{
#ifdef USE_OCULUS
	if(_deviceHandle)
	{
		if(streaming != _sendSamples || threshold != _threshold)
		{
			OVR::LatencyTestConfiguration config = OVR::LatencyTestConfiguration(OVR::Color());
			OVR::LatencyTestDevice* testDev = static_cast<OVR::LatencyTestDevice*>(_deviceHandle);
			testDev->GetConfiguration(&config);

			if(streaming != _sendSamples)
			{
				config.SendSamples = streaming;
				_sendSamples = streaming;
			}

			if(threshold != _threshold)
			{
				config.Threshold = OVR::Color((unsigned char)(threshold.x * 255.0f), (unsigned char)(threshold.y * 255.0f), (unsigned char)(threshold.z * 255.0f), (unsigned char)(threshold.w * 255.0f));
				_threshold.set(threshold);
			}

			return testDev->SetConfiguration(config, wait);
		}
		//Values aren't different. It's still valid settings
		return true;
	}
#endif
	return false;
}

const Vector4& VRLatencyTester::getCalibrate() const
{
	return _calibrate;
}

bool VRLatencyTester::setCalibrate(const Vector4& calibrationSetting, bool wait)
{
#ifdef USE_OCULUS
	if(_deviceHandle)
	{
		OVR::LatencyTestDevice* testDev = static_cast<OVR::LatencyTestDevice*>(_deviceHandle);

		OVR::Color& col = OVR::Color((unsigned char)(calibrationSetting.x * 255.0f), (unsigned char)(calibrationSetting.y * 255.0f), 
			(unsigned char)(calibrationSetting.z * 255.0f), (unsigned char)(calibrationSetting.w * 255.0f));

		_calibrate.set(calibrationSetting);

		OVR::LatencyTestCalibrate cali = OVR::LatencyTestCalibrate(col);

		return testDev->SetCalibrate(cali, wait);
	}
#endif
	return false;
}

bool VRLatencyTester::startTest(const Vector4& targetColor, bool wait)
{
#ifdef USE_OCULUS
	if(_deviceHandle)
	{
		//Check if test utility is currently enabled, if so, don't run the test
		if(_testUtility && static_cast<OVR::Util::LatencyTest*>(_testUtility)->HasDevice())
		{
			return false;
		}

		//Run test
		OVR::Color& col = OVR::Color((unsigned char)(targetColor.x * 255.0f), (unsigned char)(targetColor.y * 255.0f), 
			(unsigned char)(targetColor.z * 255.0f), (unsigned char)(targetColor.w * 255.0f));
		OVR::LatencyTestStartTest test = OVR::LatencyTestStartTest(col);
		OVR::LatencyTestDevice* testDev = static_cast<OVR::LatencyTestDevice*>(_deviceHandle);
		return testDev->SetStartTest(test, wait);
	}
#endif
	return false;
}

bool VRLatencyTester::setDisplay(unsigned char mode, unsigned int value, bool wait)
{
#ifdef USE_OCULUS
	if(_deviceHandle)
	{
		OVR::LatencyTestDisplay display = OVR::LatencyTestDisplay((OVR::UByte)mode, (OVR::UInt32)value);
		OVR::LatencyTestDevice* testDev = static_cast<OVR::LatencyTestDevice*>(_deviceHandle);
		return testDev->SetDisplay(display, wait);
	}
#endif
	return false;
}

bool VRLatencyTester::getEventsEnabled() const
{
	//Return true if message handler (but not the utility class) is set
#ifdef USE_OCULUS
	if(_deviceHandle)
	{
		if(_testUtility && static_cast<OVR::Util::LatencyTest*>(_testUtility)->HasDevice())
		{
			return false;
		}
		return static_cast<OVR::LatencyTestDevice*>(_deviceHandle)->GetMessageHandler() != NULL;
	}
#endif
	return false;
}

void VRLatencyTester::setEventsEnabled(bool enabled)
{
#ifdef USE_OCULUS
	if(_deviceHandle)
	{
		OVR::LatencyTestDevice* testDev = static_cast<OVR::LatencyTestDevice*>(_deviceHandle);
		if(_testUtility)
		{
			//If a test utility exists, then events will be disabled
			static_cast<OVR::Util::LatencyTest*>(_testUtility)->SetDevice(NULL);
		}
		else if(!enabled)
		{
			//Disable events
			void* ovrHandler = static_cast<void*>(testDev->GetMessageHandler());
			if(ovrHandler)
			{
				VRHandler* handler = static_cast<VRHandler*>(ovrHandler);
				handler->RemoveDevice(this);
			}
		}
		if(enabled)
		{
			//Enable events
			VRHandler* handler = Game::getInstance()->getVRController()->getLatencyHandler();
			handler->AddDevice(this);
		}
	}
#endif
}

void VRLatencyTester::utilityUsage()
{
#ifdef USE_OCULUS
	if(_deviceHandle)
	{
		OVR::LatencyTestDevice* testDev = static_cast<OVR::LatencyTestDevice*>(_deviceHandle);

		//Disable events if they are enabled
		if(!_testUtility || !(static_cast<OVR::Util::LatencyTest*>(_testUtility)->HasDevice()))
		{
			void* ovrHandler = static_cast<void*>(testDev->GetMessageHandler());
			if(ovrHandler)
			{
				VRHandler* handler = static_cast<VRHandler*>(ovrHandler);
				handler->RemoveDevice(this);
			}
		}

		if(_testUtility)
		{
			//Set the test utility if it's not already set
			if(!(static_cast<OVR::Util::LatencyTest*>(_testUtility)->HasDevice()))
			{
				static_cast<OVR::Util::LatencyTest*>(_testUtility)->SetDevice(testDev);
			}
		}
		else
		{
			//Create a new latency test utility
			_testUtility = new OVR::Util::LatencyTest(testDev);
		}
	}
#endif
}

void VRLatencyTester::utilityProcessInputs()
{
#ifdef USE_OCULUS
	//Use utility
	utilityUsage();

	//Run utility
	if(_testUtility)
	{
		static_cast<OVR::Util::LatencyTest*>(_testUtility)->ProcessInputs();
	}
#endif
}

bool VRLatencyTester::setUtilityDisplayScreenColor(const Vector4& colorToDisplay)
{
#ifdef USE_OCULUS
	//Use utility
	utilityUsage();

	//Run utility
	if(_testUtility)
	{
		OVR::Color& col = OVR::Color((unsigned char)(colorToDisplay.x * 255.0f), (unsigned char)(colorToDisplay.y * 255.0f), 
			(unsigned char)(colorToDisplay.z * 255.0f), (unsigned char)(colorToDisplay.w * 255.0f));
		return static_cast<OVR::Util::LatencyTest*>(_testUtility)->DisplayScreenColor(col);
	}
#endif
	return false;
}

const char* VRLatencyTester::getUtilityResults()
{
#ifdef USE_OCULUS
	//We don't need to "use" the utility, just access it
	if(_testUtility)
	{
		OVR::Util::LatencyTest* util = static_cast<OVR::Util::LatencyTest*>(_testUtility);
		if(util->HasDevice())
		{
			return util->GetResultsString();
		}
	}
#endif
	return NULL;
}

}
