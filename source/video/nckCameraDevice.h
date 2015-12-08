
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_CAMERA_DEVICE_H
#define NCK_CAMERA_DEVICE_H

#include "nckVideoConfig.h"
#include <string>
#include <vector>

_VIDEO_BEGIN

/**
* Video pixel format.
*/
enum Format{
	FORMAT_RGB_24,
	FORMAT_RGBA_32,
	FORMAT_YUV_420,
	FORMAT_YUV_422,
};

class Camera;

/**
* Abstract class for camera frame delivery callback.
*/
class CameraCallback{
public:
	virtual ~CameraCallback(){}
	virtual void Capture(Camera * cam,const void * outputData, size_t outputSize) = 0;
};

/**
* Abstract class for camera/webcamera capture device.
*/
class Camera{
public:
	virtual ~Camera(){}
	virtual int GetWidth()=0;
	virtual int GetHeight()=0;
	virtual std::string GetDeviceName()=0;
	virtual void StartCapture()=0;
	virtual void StopCapture()=0;
	void SetCallback(CameraCallback * callback);
	CameraCallback * GetCallback();
protected:
	CameraCallback * m_Callback;
};

Camera * OpenCamera(const std::string & devName, int width, int height, Format outFormat);

_VIDEO_END

#endif // NCK_CAMERA_DEVICE_H
