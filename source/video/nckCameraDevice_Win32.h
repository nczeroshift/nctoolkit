
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_CAMERA_DEVICE_WIN32_H
#define NCK_CAMERA_DEVICE_WIN32_H

#include "nckCameraDevice.h"
#include "nckThread.h"

#include <atlbase.h>
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include <Wmcodecdsp.h>
_VIDEO_BEGIN

class CameraWin32 : public Camera, public virtual Core::Threadable {
public:
	CameraWin32(int width, int height);
	~CameraWin32();

	int GetWidth() {return m_Width;};
	int GetHeight() {return m_Height;};
	std::string GetDeviceName();
	void StartCapture();
	void StopCapture();

	void Run();
	
	int m_Width;
	int m_Height;

	UINT32 outSize;
	UINT32 outStride;
	UINT32 BytesPerPixel;
	std::string m_Name;
	CComPtr<IMFSourceReader> videoFileSource;
	CComPtr<IMFAttributes> msAttr;
	CComPtr<IMFMediaType> mediaTypeOut;
	uint8_t * m_Data;
	DWORD streamIndex, flags;
	DWORD dwStreamIndex;
	LONGLONG  sampleTime;
};

_VIDEO_END

#endif // NCK_CAMERA_DEVICE_H
