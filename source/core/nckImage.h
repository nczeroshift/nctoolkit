
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_IMAGE_H
#define NCK_IMAGE_H

#include "nckCoreConfig.h"
#include "nckDataIO.h"
#include <string>

_CORE_BEGIN

enum PixelFormat{
	PIXEL_FORMAT_ALPHA_8B,
	PIXEL_FORMAT_RGB_8B,
	PIXEL_FORMAT_RGBA_8B,
};

enum ImageType{
	IMAGE_TYPE_TGA,
	IMAGE_TYPE_PNG,
	IMAGE_TYPE_JPEG,
	IMAGE_TYPE_BMP,
	IMAGE_TYPE_TBIN,
	IMAGE_TYPE_RAW,
};

/**
 * Image loading & Saving class
 */
class Image
{
public:
	Image();
	~Image();

	static Image * Create(int width, int height, PixelFormat format);

	/**
	 * Load image from path.
	 * @throw Exception on image loading failure. 
	 */
	static Image * Load(const std::string & filename);
	static Image * Load(ImageType type, DataReader * reader);

	static Image * GenerateCircle(int size, bool expand, PixelFormat format);
	static Image * GenerateCheck(int size, PixelFormat format);

	/**
	 * Get image type from filename extension.
	 * @return Image file type
	 * @throws Exception if extension is unsupported.
	 */
	static ImageType GetImageType(const std::string & filename);

	int GetWidth();
	int GetHeight();

	PixelFormat GetFormat();
	ImageType GetType();

	unsigned char * GetData();

	/**
	 * Save image data to file.
	 */
	void Save(const std::string & filename,ImageType type, float quality = 100.0);

	/**
	 * Save image to memory.
	 * @param type Output image type, BMP and JPEG supported.
	 * @param quality Compression quality, JPEG only.
	 * @param output Reference to pointer where the image file will be written.
	 * @return Image file size.
	 */
	int Save(ImageType type, float quality, unsigned char ** output);  
private:
	int width;
	int height;
	PixelFormat format;
	ImageType type;
	unsigned char * data;
};

bool ImageCopy(int srcWidth,int srcHeight,PixelFormat srcFormat,unsigned char * src,
	 int dstWidth,int dstHeight,PixelFormat dstFormat, unsigned char * dst);

_CORE_END

#endif // #ifndef NCK_IMAGE_H
