#ifndef VR_LATENCY_TESTER_H_
#define VR_LATENCY_TESTER_H_

#include "Vector4.h"

#include "VRDevice.h"

namespace gameplay
{

/**
 * A VR latency tester.
 */
class VRLatencyTester : public VRDevice
{
	friend class VRController;

public:

	/**
     * @see VRDevice::getType
     */
	VRTypes getType() const;

	/**
	 * Get the configured threshold for triggering a detected color change.
	 * 
	 * @return The threshold for detected color change.
	 */
	const Vector4& getConfigurationThreshold() const;

	/**
	 * Get the configured for streaming samples read in by the tester.
	 * 
	 * @return If streaming samples is enabled by for the tester.
	 */
	bool getConfigurationSendSamples() const;

	/**
	 * Set latency tester configuration settings.
	 * 
	 * @param threshold The threshold value for triggering a detected color change.
	 * @param streaming A flag for determining if the samples read by the tester 
	 *   should be streamed.
	 * @param wait If the configuration should be applied synchronously.
	 * @return If the latency tester's configuration setting were set.
	 */
	bool setConfiguration(const Vector4& threshold, bool streaming, bool wait = false);

	/**
	 * Get calibration information from the tester.
	 * 
	 * @return The calibration information.
	 */
	const Vector4& getCalibrate() const;

	/**
	 * Set calibration information before the start of a test.
	 * 
	 * Calibration information is lost when power is removed from the device.
	 * 
	 * @param calibrationSetting Calibration information for the tester.
	 * @param wait If the caibration information should be applied synchronously.
	 * @return If the latency tester's calibration has been performed successfully.
	 */
	bool setCalibrate(const Vector4& calibrationSetting, bool wait = false);

	/**
	 * Triggers the start of a measurement.
	 * 
	 * This starts the millisecond timer on the device and causes it to respond with 
	 * the 'LATENCY_TESTER_STARTED_EVENT' event.
	 * 
	 * Note: If events are not enabled for this device, then no event will ever be recieved.
	 *   If a utility function has been used, then setEventsEnabled must be set in order to 
	 *   enable this function. If setEventsEnabled is set to true, then it will send events.
	 *   If setEventsEnabled is set to false then it disables the utility functions for the
	 *   execution of this function.
	 * 
	 * @param targetColor The color the latency tester should stop the timer on.
	 * @param wait If the test should be preformed synchronously.
	 * @return If the test was started successfully.
	 */
	bool startTest(const Vector4& targetColor, bool wait = false);

	/**
	 * Set the value displayed on the LED display panel.
	 * 
	 * @param mode The display mode for the LED display.
	 * @param value The value to display.
	 * @param wait If the display should be set synchronously.
	 * @return If the display was set successfully.
	 */
	bool setDisplay(unsigned char mode, unsigned int value, bool wait = false);

	/**
	 * Get if events are enabled for this device.
	 * 
	 * If a utility function is used then this will return false.
	 * 
	 * @return If events are enabled.
	 */
	bool getEventsEnabled() const;

	/**
	 * Enable or disable events for this device.
	 * 
	 * If enabled, Game::vrEvent will be called when this device produces an event.
	 *   If disabled, no events will be sent even if this device produces an event.
	 *   Regardless of enabling or disabling events, if a utility function was used,
	 *   then this will disable it's functionality. It can be reenabled if the function
	 *   is used again.
	 * 
	 * @param enabled If events are enabled or disabled for this device.
	 */
	void setEventsEnabled(bool enabled);

	/**
	 * Utility function to time device latency.
	 * 
	 * This should be called at the same place in the code where the game engine
	 *   reads the headset orientation from such as with a VRSensor. Calling this 
	 *   at the right time enables us to measure the same latency that occurs for 
	 *   headset orientation changes.
	 * 
	 * This will disable events.
	 */
	void utilityProcessInputs();

	/**
	 * Utility function to display a screen color.
	 * 
	 * The latency tester works by sensing the color of the pixels directly
	 *   beneath it. The color of these pixels can be set by drawing a small
	 *   quad at the end of the rendering stage. The quad should be small
	 *   such that it doesn't significantly impact the rendering of the scene,
	 *   but large enough to be 'seen' by the sensor.
	 * 
	 * This will disable events.
	 * 
	 * @return If the test ran successfully.
	 */
	bool setUtilityDisplayScreenColor(const Vector4& colorToDisplay);

	/**
	 * Utility function to get results of running utilityProcessInputs.
	 * 
	 * Call this to get a string containing the most recent results.
	 *   If the string has already been gotten then NULL will be returned.
	 *   The string pointer will remain valid until the next time this 
	 *   method is called.
	 * 
	 * @return The utility results.
	 */
	const char* getUtilityResults();

private:

    VRLatencyTester(void* handle);
	~VRLatencyTester();

	VRLatencyTester(const VRLatencyTester& copy);
    VRLatencyTester& operator=(const VRLatencyTester&);

	void utilityUsage();

	Vector4 _threshold;
	bool _sendSamples;
	Vector4 _calibrate;

	void* _testUtility;

};

}

#endif
