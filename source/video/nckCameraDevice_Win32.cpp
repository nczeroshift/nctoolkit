#include "nckCameraDevice_Win32.h"

#include "nckImage.h"
#include <memory>

#include "nckUtils.h"
#include "nckException.h"

_VIDEO_BEGIN

BOOL UnicodeToMByte(LPCWSTR unicodeStr, LPSTR multiByteStr, DWORD size)
{
	// Get the required size of the buffer that receives the multiByte string. 
	DWORD minSize;
	minSize = WideCharToMultiByte(CP_OEMCP, NULL, unicodeStr, -1, NULL, 0, NULL, FALSE);
	if (size < minSize)
	{
		return FALSE;
	}
	// Convert string from Unicode to multi-byte.
	WideCharToMultiByte(CP_OEMCP, NULL, unicodeStr, -1, multiByteStr, size, NULL, FALSE);
	return TRUE;
}
#define MF_CHECK(R) if (FAILED(R)) THROW_EXCEPTION("Unable to setup windows Media Foundation")

void MFInit(void) 
{
	static bool inited = false;
	if (inited) return;
	inited = true;

	HRESULT hr = MFStartup(MF_VERSION);
	MF_CHECK(hr);
	
	hr = MFTRegisterLocalByCLSID(__uuidof(CColorConvertDMO), MFT_CATEGORY_VIDEO_PROCESSOR, L"", MFT_ENUM_FLAG_SYNCMFT, 0, NULL, 0, NULL);
	MF_CHECK(hr);
}

CameraWin32::CameraWin32(int width, int height)
{
	MFInit();

	m_Width = width;
	m_Height = height;
	
	BytesPerPixel = 3;
	outStride = BytesPerPixel * width;
	outSize = outStride * height;

	m_Data = new uint8_t[m_Width*m_Height*BytesPerPixel];


	msAttr = NULL;
	MF_CHECK(MFCreateAttributes(&msAttr, 1));

	
	MF_CHECK(msAttr->SetGUID(
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
	));

	IMFActivate **ppDevices = NULL;
	UINT32  count = 0;
	MF_CHECK(MFEnumDeviceSources(msAttr, &ppDevices, &count));
	
	if(!count) THROW_EXCEPTION("No video devices found");

	UINT32 ind = 0;

	LPWSTR webcamFriendlyName;
	char * webCamName = new char[128];
	MF_CHECK(ppDevices[ind]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &webcamFriendlyName, NULL));
	
	UnicodeToMByte(webcamFriendlyName, (LPSTR)webCamName, 128);

	m_Name = std::string(webCamName);

	delete webCamName;

	CComPtr<IMFMediaSource> mSrc;
	CComPtr<IMFAttributes> srAttr;
	
	MF_CHECK(ppDevices[ind]->ActivateObject(__uuidof(IMFMediaSource), (void**)&mSrc));
	MF_CHECK(MFCreateAttributes(&srAttr, 10));
	MF_CHECK(srAttr->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, true));
	
	MF_CHECK(srAttr->SetUINT32(MF_SOURCE_READER_DISABLE_DXVA, false));
	MF_CHECK(srAttr->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, false));
	MF_CHECK(srAttr->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, true));
	MF_CHECK(MFCreateSourceReaderFromMediaSource(mSrc, srAttr, &videoFileSource));

	ppDevices[ind]->Release();
	
	IAMVideoProcAmp *pProcAmp = NULL;
	IAMCameraControl *pProcControl = NULL;

	/*if (SUCCEEDED(videoFileSource->GetServiceForStream((DWORD)MF_SOURCE_READER_MEDIASOURCE, GUID_NULL, IID_PPV_ARGS(&pProcAmp))))
	{
	long paramVal, paramFlag;
	HRESULT hr = pProcAmp->Get(VideoProcAmp_Gain, &paramVal, &paramFlag);
	long minVal, maxVal, stepVal;
	if (FAILED(hr))
	hr = pProcAmp->GetRange(VideoProcAmp_Gain, &minVal, &maxVal, &stepVal, &paramVal, &paramFlag);//Unable to get the property, trying to return default value
	pProcAmp->Release();

	}*/

	/*
	if (SUCCEEDED(videoFileSource->GetServiceForStream((DWORD)MF_SOURCE_READER_MEDIASOURCE, GUID_NULL, IID_PPV_ARGS(&pProcAmp))))
	{
		long paramVal, paramFlag;
		HRESULT hr = pProcAmp->Get(CameraControl_Exposure, &paramVal, &paramFlag);
		long minVal, maxVal, stepVal;
		if (FAILED(hr))
			hr = pProcAmp->GetRange(CameraControl_Exposure, &minVal, &maxVal, &stepVal, &paramVal, &paramFlag);//Unable to get the property, trying to return default value
		pProcAmp->Release();	
	}
	*/

	if (SUCCEEDED(videoFileSource->GetServiceForStream((DWORD)MF_SOURCE_READER_MEDIASOURCE, GUID_NULL, IID_PPV_ARGS(&pProcControl))))
	{
		long paramVal = 0;
		HRESULT hr = pProcControl->Set(CameraControl_Exposure, paramVal, VideoProcAmp_Flags_Manual);
		pProcControl->Release();
	}


	if (SUCCEEDED(videoFileSource->GetServiceForStream((DWORD)MF_SOURCE_READER_MEDIASOURCE, GUID_NULL, IID_PPV_ARGS(&pProcControl))))
	{
		long paramVal = (long)-8;
		HRESULT hr = pProcControl->Set(CameraControl_Exposure, paramVal, VideoProcAmp_Flags_Manual);
		pProcControl->Release();
	}

	/*
	if (SUCCEEDED(videoFileSource->GetServiceForStream((DWORD)MF_SOURCE_READER_MEDIASOURCE, GUID_NULL, IID_PPV_ARGS(&pProcAmp))))
	{
		float v = -7 / 100.0;
		long paramVal = (long)v;
		HRESULT hr = pProcAmp->Set(VideoProcAmp_Gain, paramVal, VideoProcAmp_Flags_Manual);
		pProcAmp->Release();
		//return SUCCEEDED(hr);
	}
	*/


	dwStreamIndex = 0;
	sampleTime = 0;
	
	MF_CHECK(MFCreateMediaType(&mediaTypeOut));

	MF_CHECK(mediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
	MF_CHECK(mediaTypeOut->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB24));

	

	MF_CHECK(mediaTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive));
	MF_CHECK(MFSetAttributeRatio(mediaTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1));
	MF_CHECK(MFSetAttributeSize(mediaTypeOut, MF_MT_FRAME_SIZE, width, height));
	MF_CHECK(mediaTypeOut->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, 1));
	MF_CHECK(mediaTypeOut->SetUINT32(MF_MT_SAMPLE_SIZE, outSize));
	MF_CHECK(mediaTypeOut->SetUINT32(MF_MT_DEFAULT_STRIDE, outStride));

	DWORD dwStreamBest = 0;
	DWORD tryStream = (DWORD)(dwStreamBest);

	MF_CHECK(videoFileSource->SetStreamSelection((DWORD)MF_SOURCE_READER_ALL_STREAMS, false));
	MF_CHECK(videoFileSource->SetStreamSelection(tryStream, true));
	MF_CHECK(videoFileSource->SetCurrentMediaType(tryStream, NULL, mediaTypeOut));
}

CameraWin32::~CameraWin32() {
	
}

std::string CameraWin32::GetDeviceName() {
	return m_Name;
}

void CameraWin32::StartCapture() {
	Start();
}

void CameraWin32::StopCapture() {
	TearDownAndJoin();
}

void CameraWin32::Run() 
{
	while (!IsTearingDown()) 
	{
		CComPtr<IMFSample> videoSample = NULL;

		HRESULT hr = videoFileSource->ReadSample(
			dwStreamIndex, // Stream index.
			0,             // Flags.
			&streamIndex,  // Receives the actual stream index.
			&flags,        // Receives status flags.
			&sampleTime,  // Receives the time stamp.
			&videoSample   // Receives the sample or NULL.
		);

		if (videoSample != NULL) {

			DWORD bcnt;
			if (videoSample && SUCCEEDED(videoSample->GetBufferCount(&bcnt)) && bcnt > 0)
			{
				CComPtr<IMFMediaBuffer> buf = NULL;
				if (SUCCEEDED(videoSample->GetBufferByIndex(0, &buf)))
				{
					DWORD maxsize, cursize;
					BYTE* ptr = NULL;
					if (SUCCEEDED(buf->Lock(&ptr, &maxsize, &cursize))) {
						memcpy(m_Data, ptr, cursize);
						m_Callback->Capture(this, m_Data, cursize);
						buf->Unlock();
					}

				}
			}
		}

		Core::Thread::Wait(1);
	}	
}


/*while (true) {
CComPtr<IMFSample> videoSample = NULL;

hr = videoFileSource->ReadSample(
dwStreamIndex, // Stream index.
0,             // Flags.
&streamIndex,  // Receives the actual stream index.
&flags,        // Receives status flags.
&sampleTime,  // Receives the time stamp.
&videoSample   // Receives the sample or NULL.
);

if (videoSample != NULL) {

DWORD bcnt;
if (videoSample && SUCCEEDED(videoSample->GetBufferCount(&bcnt)) && bcnt > 0)
{
CComPtr<IMFMediaBuffer> buf = NULL;
if (SUCCEEDED(videoSample->GetBufferByIndex(0, &buf)))
{
DWORD maxsize, cursize;
BYTE* ptr = NULL;
if (SUCCEEDED(buf->Lock(&ptr, &maxsize, &cursize))) {
//FILE * f = fopen("C:\\tmp\\out.raw", "wb");
Core::Image * img = Core::Image::Create(width, height, Core::PIXEL_FORMAT_RGB_8B);

memcpy(img->GetData(), ptr, cursize);
img->Save("C:\\tmp\\out.bmp", Core::IMAGE_TYPE_BMP, 100);
//fwrite(data, 1, cursize, f);
//fclose(f);
buf->Unlock();
}

}
}
}
Core::Thread::Wait(1);
}*/

_VIDEO_END