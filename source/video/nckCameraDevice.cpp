
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckCameraDevice.h"
#include "nckUtils.h"
#include "nckThread.h"
#include "nckException.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(NCK_LINUX)
    #include <sys/time.h>
    #include <sys/mman.h>
    #include <sys/ioctl.h>
    #include <asm/types.h>
    #include <linux/videodev2.h>
    #include <getopt.h>
    #include <unistd.h>
#endif

#include "nckColorSpace.h"

#include <list>

enum IOMethod{
	IO_METHOD_READ      = 1,
	IO_METHOD_USERPTR   = 2,
	IO_METHOD_MMAP      = 4
};

_VIDEO_BEGIN

class Buffer {
public:
	Buffer(){
		length = 0;
		data = NULL;
	}

	Buffer(size_t size){
		data = new uint8_t[size];
		length = size;
	}

	~Buffer(){
		SafeDelete(data);
	}

	uint8_t * data;
	size_t length;
};


void Camera::SetCallback(CameraCallback * callback){
	m_Callback = callback;
}

CameraCallback * Camera::GetCallback(){
	return m_Callback;
}

#if defined(NCK_WEBCAM_V2L)

#define CLEAR(x) memset (&(x), 0, sizeof (x))
static int xioctl(int fd,int request,void *arg)
{
	int r;
	do r = ioctl (fd, request, arg);
	while (-1 == r && EINTR == errno);
	return r;
}

class V4L2Cam : public Camera, public virtual Core::Threadable{
public:
	int frameWidth;
	int frameHeight;
	Format outFormat;
	Format inFormat;
	std::string devicePath;
	int deviceId;
	int supportedMethods;
	int fd;
	std::list<Buffer*> buffers;
	std::string deviceName;
	uint8_t * outputFrame;

	void StartCapture(){
		Start();
	}

	void StopCapture(){
		TearDownAndJoin();
	}

	std::string GetDeviceName(){
		return deviceName;
	}

	void Run(){
		if(AllocBuffers())
		{
			StartStreaming();

			while(!IsTearingDown()){
				int ret = ReadFrame();
				if(ret == 1){
					if(m_Callback)
						m_Callback->Capture(this,outputFrame,frameWidth*frameHeight*3);
				}else if(ret == 2){
					usleep(2000);
				}
				else if(ret == 0){
					break;
				}
			}

			StopStreaming();
		}
		FreeBuffers();
	}


	V4L2Cam(const std::string & devPath, int width, int height,Format format) : Threadable(){
		fd = 0;
		frameWidth = width;
		frameHeight = height;

		outFormat = format;
		inFormat = FORMAT_YUV_422;
		deviceId = 0;
		devicePath = devPath;
		supportedMethods = 0;
		OpenDevice();
		outputFrame = new uint8_t[frameWidth*frameHeight*3];
	}

	virtual ~V4L2Cam(){
		CloseDevice();

		while(buffers.size()>0){
			delete buffers.front();
			buffers.pop_front();
		}

		SafeArrayDelete(outputFrame);
	}


	int GetWidth(){
		return frameWidth;
	}

	int GetHeight(){
		return frameHeight;
	}

	int ReadFrame(){
		struct v4l2_buffer buf;

		CLEAR (buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;

		if (xioctl (fd, VIDIOC_DQBUF, &buf) == -1) {
			switch (errno) {
				case EAGAIN:
					return 2;
				case EIO:
				default:
					return 0;
			}
		}

		if(buf.index >= buffers.size())
			return 0;

		size_t index=0;
		for(std::list<Buffer*>::iterator i = buffers.begin();i!=buffers.end();i++)
			if(buf.index == index++){
				ColorSpace::ConvertImageYUV422ToRGB24((*i)->data, (*i)->length,frameWidth,frameHeight,(void*)outputFrame);
				break;
			}

		if (xioctl (fd, VIDIOC_QBUF, &buf) == -1)
			return 0;

		return 1;
	}

	/**
	 * Open device for capture.
	 */
	int OpenDevice(){

		struct stat st;

		if(stat (devicePath.c_str(), &st) == -1){
			fprintf(stderr,"Cannot identify device");
			return 0;
		}

		if(!S_ISCHR (st.st_mode)){
			fprintf(stderr,"Filename isn't a device");
			return 0;
		}

		fd = open (devicePath.c_str(), O_RDWR | O_NONBLOCK, 0);

		if (fd == -1){
			fprintf(stderr,"Unable open device");
			return 0;
		}

		struct v4l2_capability cap;
		struct v4l2_cropcap cropcap;
		struct v4l2_crop crop;
		struct v4l2_format fmt;
		unsigned int min;

		if (xioctl (fd, VIDIOC_QUERYCAP, &cap) == -1){
			fprintf(stderr,"Unable to enumerate device capabilities");
			return 0;
		}

		deviceName = std::string((const char*)cap.card);

		if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)){
			fprintf(stderr,"Device doesn't support capture");
			return 0;
		}

		if (cap.capabilities & V4L2_CAP_READWRITE) {
			supportedMethods |= IO_METHOD_READ;
		}

		if (cap.capabilities & V4L2_CAP_STREAMING) {
			supportedMethods |= IO_METHOD_MMAP | IO_METHOD_USERPTR;
		}


		/*v4l2_frmivalenum frameProps;
		CLEAR(frameProps);
		xioctl(fd,VIDIOC_ENUM_FRAMEINTERVALS,&frameProps);
		printf("type %d\n",frameProps.type);
		printf("max %d %d\n",frameProps.stepwise.max.numerator,frameProps.stepwise.max.denominator);
		printf("min %d %d\n",frameProps.stepwise.min.numerator,frameProps.stepwise.min.denominator);*/

		CLEAR (cropcap);

		cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (xioctl (fd, VIDIOC_CROPCAP, &cropcap)==0) {
			crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			crop.c = cropcap.defrect;

			if (xioctl (fd, VIDIOC_S_CROP, &crop)==-1) {
			}
		}

		CLEAR (fmt);

		fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmt.fmt.pix.width       = frameWidth;
		fmt.fmt.pix.height      = frameHeight;

		switch(inFormat)
		{
		case FORMAT_RGB_24:
			fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
			break;
		case FORMAT_RGBA_32:
			fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32;
			break;
		case FORMAT_YUV_420:
			fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
			break;
		case FORMAT_YUV_422:
			fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
			break;
		}

		fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

		if (xioctl (fd, VIDIOC_S_FMT, &fmt) == -1){
			fprintf(stderr,"Unable to set device format");
			return 0;
		}

		min = fmt.fmt.pix.width * 2;
		if (fmt.fmt.pix.bytesperline < min)
			fmt.fmt.pix.bytesperline = min;
		min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
		if (fmt.fmt.pix.sizeimage < min)
			fmt.fmt.pix.sizeimage = min;

		return 1;
	}

	/**
	 * Allocate Buffers
	 */
	int AllocBuffers()
	{
		// Initialize Buffers
		struct v4l2_requestbuffers req;

		CLEAR (req);

		req.count               = 4;
		req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory              = V4L2_MEMORY_MMAP;

		if (xioctl (fd, VIDIOC_REQBUFS, &req) == -1){
			fprintf(stderr,"Unable request mmap buffers");
			return 0;
		}

		if (req.count < 2){
			fprintf(stderr,"Number of buffers bellow required");
			return 0;
		}

		for (size_t i = 0; i < req.count; ++i)
		{
			struct v4l2_buffer buf;

			CLEAR (buf);

			buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory      = V4L2_MEMORY_MMAP;
			buf.index       = i;

			if (xioctl (fd, VIDIOC_QUERYBUF, &buf) == -1){
				fprintf(stderr,"Unable to query mmap buffer %d",i);
				return 0;
			}

			Buffer * buff = new Buffer();
			buff->length = buf.length;
			buff->data = (uint8_t*)mmap (NULL, // start anywhere
							buf.length,
							PROT_READ | PROT_WRITE, // required
							MAP_SHARED, // recommended
							fd,buf.m.offset);

			if (MAP_FAILED == buff->data){
				fprintf(stderr,"Unable to create mmap buffer %d",i);
				return 0;
			}

			buffers.push_back(buff);
		}

		// Start Streaming
		for (size_t i = 0; i < buffers.size(); ++i) {
			struct v4l2_buffer buf;

			CLEAR (buf);

			buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory      = V4L2_MEMORY_MMAP;
			buf.index       = i;

			if (xioctl (fd, VIDIOC_QBUF, &buf) == -1){
				fprintf(stderr,"Unable to enqueue mmap buffer %d",i);
				return 0;
			}
		}

		return 1;
	}

	/**
	 * Start streaming from device.
	 */
	int StartStreaming(){
		enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (xioctl (fd, VIDIOC_STREAMON, &type) == -1){
			fprintf(stderr,"Unable to open streamming");
			return 0;
		}

		return 1;
	}

	/**
	 * Stop streaming from device.
	 */
	int StopStreaming(){
		// Stop Streaming
		enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (xioctl (fd, VIDIOC_STREAMOFF, &type) == -1){
			fprintf(stderr,"Unable to stop stream");
			return 0;
		}

		return 1;
	}

	/**
	 * Free Buffers.
	 */
	int FreeBuffers(){

		int c = 0;
		for(std::list<Buffer*>::iterator i = buffers.begin();i!=buffers.end();i++,c++){
			if( munmap((*i)->data,(*i)->length) == -1)
			{
				fprintf(stderr,"Unable to free buffer %d",c);
			}
			(*i)->data = NULL;
			(*i)->length = 0;
		}

		return 1;
	}

	/**
	 * Close device.
	 */
	int CloseDevice(void)
	{
		// Close device
		if (close (fd)==-1){
			fprintf(stderr,"Unable to close device");
			return 0;
		}

		fd = -1;
		return 1;
	}
};

#endif // #if defined(NCK_WEBCAM_V2L)

Camera * OpenCamera(const std::string & devName, int width, int height, Format outFormat)
{
#if defined(NCK_WEBCAM_V2L)
	return new V4L2Cam(devName,width,height,outFormat);
#else
	THROW_EXCEPTION("Camera device not yet implemented on this platform");
#endif
	return NULL;
}

_VIDEO_END


