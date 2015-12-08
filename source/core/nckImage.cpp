
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckImage.h"
#include "nckUtils.h"
#include "nckException.h"
#include "nckPoint.h"
#include "nckDataIO.h"
#include "../nckConfig.h"

#if defined(__APPLE__) 
#include "macosx/nckWindow_macosx.h"
#endif

#include <math.h>

static int SaveTBIN(int width,int height, Core::PixelFormat format, unsigned char * data, unsigned char ** output){
	// BinTexture
	int hSize = sizeof(int) + // id 
				sizeof(int) * 2 + // width height
				sizeof(int) + // compression
				sizeof(int); // type

	int iSize = 0;
	switch(format){
	case Core::PIXEL_FORMAT_RGB_8B:
		iSize = width * height * 3;
		break;
	case Core::PIXEL_FORMAT_RGBA_8B:
		iSize = width * height * 4;
		break;
	case Core::PIXEL_FORMAT_ALPHA_8B:
		iSize = width * height;
		break;
	}
             
	*output = new unsigned char[hSize+iSize];
	(*output)[0] = 'B';
	(*output)[1] = 'T';
	(*output)[2] = 'E';
	(*output)[3] = 'X';
	((int*)*output)[1] = width;
	((int*)*output)[2] = height;
	((int*)*output)[3] = 0;
	((int*)*output)[4] = (int)format;
    
	memcpy(*output+hSize,data,iSize);

	return hSize+iSize;
}


static void LoadTBIN(Core::DataReader * reader, int * width, int * height, Core::PixelFormat * format, unsigned char ** data)
{
	char id[4];
	if(reader->Read(id,sizeof(int))!= sizeof(int))
		THROW_EXCEPTION("Unable to read id");

	if(id[0] != 'B' || id[1] != 'T' || id[2] != 'E' || id[3] != 'X')
		THROW_EXCEPTION("Incorrect bin texture id");

	if(reader->Read(width,sizeof(int))!= sizeof(int))
		THROW_EXCEPTION("Unable to read width");

	if(reader->Read(height,sizeof(int))!= sizeof(int))
		THROW_EXCEPTION("Unable to read height");

	int tmpCompression;
	if(reader->Read(&tmpCompression,sizeof(int))!= sizeof(int))
		THROW_EXCEPTION("Unable to read compression");

	int tmpFormat;
	if(reader->Read(&tmpFormat,sizeof(int))!= sizeof(int))
		THROW_EXCEPTION("Unable to read format");

	if(tmpFormat != Core::PIXEL_FORMAT_RGB_8B && 
		tmpFormat != Core::PIXEL_FORMAT_RGBA_8B && 
			tmpFormat != Core::PIXEL_FORMAT_ALPHA_8B)
				THROW_EXCEPTION("Invalid pixel format");

	int iSize = 0;
	switch(tmpFormat){
	case Core::PIXEL_FORMAT_RGB_8B:
		iSize = *width * *height * 3;
		*format = Core::PIXEL_FORMAT_RGB_8B;
		break;
	case Core::PIXEL_FORMAT_RGBA_8B:
		iSize = *width * *height * 4;
		*format = Core::PIXEL_FORMAT_RGBA_8B;
		break;
	case Core::PIXEL_FORMAT_ALPHA_8B:
		iSize = *width * *height;
		*format = Core::PIXEL_FORMAT_ALPHA_8B;
		break;
	}

	*data = new unsigned char[iSize];
	if(reader->Read(*data,iSize)!=iSize){
		SafeArrayDelete(*data);
		THROW_EXCEPTION("Unable to read image data");
	}
}

/**
* PNG read/save support funtions
*/
#if defined NCK_PNG

#include <png.h>

static void CustomPNGReadData(png_structp pngPtr, png_bytep data, png_size_t length){
	png_voidp a = png_get_io_ptr(pngPtr);
	((Core::DataReader*)a)->Read((char*)data, (int)length);
}

static int SavePNG(int width,int height,Core::PixelFormat format,unsigned char * data,unsigned char ** output){
	THROW_EXCEPTION("PNG save not supported yet");
}

static void LoadPNG(Core::DataReader * reader, int * width, int * height, Core::PixelFormat * format, unsigned char ** data)
{
	char header[8];

	if(reader->Read(header,sizeof(char)*8)!=sizeof(char)*8)
		THROW_EXCEPTION("Unable to read file signature");

	if(png_sig_cmp((png_byte*)header, 0, 8) != 0)
		THROW_EXCEPTION("PNG file signature not found");

	png_struct *pngStruct= png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

	png_info *pngInfo = png_create_info_struct(pngStruct);
	if(!pngInfo)
	{
		png_destroy_read_struct(&pngStruct, 0, 0);
		THROW_EXCEPTION("PNG invalid file structure");
	}

	if(setjmp(png_jmpbuf(pngStruct)) != 0)
	{
		png_destroy_read_struct(&pngStruct, &pngInfo, 0);
		THROW_EXCEPTION("PNG setjmp failure");
	}

	png_set_read_fn(pngStruct,(png_voidp)reader, CustomPNGReadData);

	png_set_sig_bytes(pngStruct, 8);
	png_read_png(pngStruct, pngInfo, 0, 0);

	int imageWidth = png_get_image_width(pngStruct, pngInfo);
	int imageHeight = png_get_image_height(pngStruct, pngInfo);
	int bpp = png_get_bit_depth(pngStruct, pngInfo);
	int cType = png_get_color_type(pngStruct, pngInfo);

	int bytesPerPixel;

	if((cType == PNG_COLOR_TYPE_RGB) && (bpp == 8))
		bytesPerPixel = 3;
	else if((cType == PNG_COLOR_TYPE_RGB_ALPHA) && (bpp == 8))
		bytesPerPixel = 4;
	else if(cType == PNG_COLOR_TYPE_PALETTE)
		bytesPerPixel = 1;
	else{
		png_destroy_read_struct(&pngStruct, &pngInfo, NULL);
		THROW_EXCEPTION("PNG unsupported pixel format");
	}

	*height = imageHeight;
	*width = imageWidth;

	if(bytesPerPixel==3)
		*format = Core::PIXEL_FORMAT_RGB_8B;
	else if(bytesPerPixel==4)
		*format = Core::PIXEL_FORMAT_RGBA_8B;

	png_byte **imageData = png_get_rows(pngStruct, pngInfo);

	*data = new unsigned char [imageHeight * imageWidth * bytesPerPixel];

	for (int i = 0; i < imageHeight; i++)
	{
		memcpy(&(*data)[i * imageWidth * bytesPerPixel],
				imageData[i], imageWidth * bytesPerPixel);
	}

	png_destroy_read_struct(&pngStruct, &pngInfo, NULL);
}

#endif

/**
* BMP read/save support funtions
*/
#if defined NCK_BMP

void LoadBMP(Core::DataReader * reader, int * width, int * height, Core::PixelFormat * format, unsigned char ** data){
	THROW_EXCEPTION("BMP load not supported yet");
}

int SaveBMP(unsigned int width, unsigned int height,unsigned char *data, unsigned char ** output)
{
	const int filesize = 54 + 3*width*height; 
	const int padding = (4-(width*3)%4)%4;
	const int retFileSize = 54 + 3*width*height + padding * height;

	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	unsigned char bmppad[3] = {0,0,0};

	bmpfileheader[ 2] = (unsigned char)(filesize);
	bmpfileheader[ 3] = (unsigned char)(filesize>>8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(width);
	bmpinfoheader[ 5] = (unsigned char)(width>>8);
	bmpinfoheader[ 6] = (unsigned char)(width>>16);
	bmpinfoheader[ 7] = (unsigned char)(width>>24);
	bmpinfoheader[ 8] = (unsigned char)(height);
	bmpinfoheader[ 9] = (unsigned char)(height>>8);
	bmpinfoheader[10] = (unsigned char)(height>>16);
	bmpinfoheader[11] = (unsigned char)(height>>24);

	if(*output == NULL)
		*output = new unsigned char[retFileSize];

	memcpy(*output,bmpfileheader,14);
	memcpy(*output+14,bmpinfoheader,40);

	for(int i=height-1,p = 54; i >= 0; i--)
	{
		memcpy(*output+p,data+(width*(height-i-1)*3),3*width);
		p += 3*width;

		if(padding){
			memcpy(*output+p,bmppad,padding);
			p += padding;
		}
	}

	return retFileSize;
}
#endif

/**
* JPG read/save support funtions
*/
#if defined NCK_JPG

#include <jpeglib.h>

void jpeg_init_buffer(jpeg_compress_struct* cinfo) {}
boolean jpeg_empty_buffer(jpeg_compress_struct* cinfo){return TRUE;}
void jpeg_term_buffer(jpeg_compress_struct* cinfo) {}

/**
* Export raw RGB image as a JPG file on memory.
* @param output Reference where the JPG binary is stored.
* @return JPG file binary size.
*/
int SaveJPG(unsigned int width, unsigned int height,unsigned char *data,float quality,unsigned char ** output)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr       jerr;
	struct jpeg_destination_mgr dmgr;

	const int max_buffer_size = width * height *3;

	if(*output == NULL)
		*output = new unsigned char[max_buffer_size];

	// here is the magic 
	dmgr.init_destination    = jpeg_init_buffer;
	dmgr.empty_output_buffer = jpeg_empty_buffer;
	dmgr.term_destination    = jpeg_term_buffer;
	dmgr.next_output_byte    = *output;
	dmgr.free_in_buffer      = max_buffer_size;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	// make sure we tell it about our manager
	cinfo.dest = &dmgr;

	cinfo.image_width      = width;
	cinfo.image_height     = height;
	cinfo.input_components = 3;
	cinfo.in_color_space   = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality (&cinfo, 50, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	JSAMPROW row_pointer;

	// main code to write jpeg data 
	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer = (JSAMPROW) &data[max_buffer_size - cinfo.next_scanline * width * 3];
		jpeg_write_scanlines(&cinfo, &row_pointer, 1);
	}
	jpeg_finish_compress(&cinfo);

	jpeg_destroy_compress(&cinfo);

	return (int)(cinfo.dest->next_output_byte - *output);
}

#define JPEG_BUFFER_SIZE (8 << 10)
	
typedef struct {
    struct jpeg_source_mgr pub;
	Core::DataReader * reader;
	char buffer[JPEG_BUFFER_SIZE];
} CustomJPEGReader;

static void init_sourceFunc(j_decompress_ptr cinfo) {
    ((CustomJPEGReader*)cinfo->src)->pub.bytes_in_buffer = 0;
}

static boolean fill_input_bufferFunc(j_decompress_ptr cinfo) {
    CustomJPEGReader * src = (CustomJPEGReader*)cinfo->src;
	src->pub.bytes_in_buffer = src->reader->Read(src->buffer,JPEG_BUFFER_SIZE);
    src->pub.next_input_byte = (const unsigned char*)src->buffer;
    return TRUE;
}

static void skip_input_dataFunc(j_decompress_ptr cinfo, long num_bytes) {
    CustomJPEGReader * src = (CustomJPEGReader*)cinfo->src;
    if (num_bytes > 0) {

		src->reader->Seek(num_bytes,Core::SEEK_OFFSET_CURRENT);

        if ((size_t)num_bytes > src->pub.bytes_in_buffer) 
			src->pub.bytes_in_buffer = 0;
        else {
            src->pub.next_input_byte += num_bytes;
            src->pub.bytes_in_buffer -= num_bytes;
        }
    }
}

static void term_sourceFunc(j_decompress_ptr cinfo) {}

/**
* Load JPG file from data reader stream.
*/
void LoadJPG(Core::DataReader * reader, int * width, int * height, Core::PixelFormat * format, unsigned char ** data)
{
	CustomJPEGReader * jpegReader = new CustomJPEGReader();

	jpegReader->reader = reader;
	jpegReader->pub.init_source = init_sourceFunc;
    jpegReader->pub.fill_input_buffer = fill_input_bufferFunc;
    jpegReader->pub.skip_input_data = skip_input_dataFunc;
    jpegReader->pub.resync_to_restart = jpeg_resync_to_restart;
    jpegReader->pub.term_source = term_sourceFunc;
    jpegReader->pub.next_input_byte = NULL;
    jpegReader->pub.bytes_in_buffer = 0;

	jpeg_decompress_struct cinfo;
	
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);

	cinfo.src = (struct jpeg_source_mgr*)jpegReader;
	jpeg_read_header(&cinfo, TRUE);

	jpeg_start_decompress(&cinfo);

	switch(cinfo.num_components){
	case 3:
		*format = Core::PIXEL_FORMAT_RGB_8B;
		break;
	case 4:
		*format = Core:: PIXEL_FORMAT_RGBA_8B;
		break;
	default:
		SafeDelete(jpegReader);
		THROW_EXCEPTION("JPEG invalid number of components");
	}

	*width = cinfo.output_width;
	*height = cinfo.output_height;

	(*data) = new unsigned char[(*width) * (*height) * cinfo.num_components];
	unsigned char *curr_scanline = (*data);

	while (cinfo.output_scanline < cinfo.output_height){
		jpeg_read_scanlines(&cinfo, &curr_scanline, 1);
		curr_scanline += (*width) * cinfo.num_components;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	SafeDelete(jpegReader);
}
#endif

/**
* TGA read/save support funtions
*/
#if defined NCK_TGA

static int SaveTGA(int width,int height,Core::PixelFormat format,unsigned char * data,unsigned char ** output){
	THROW_EXCEPTION("TGA load not supported yet");
}

void LoadTGA(Core::DataReader * reader, int * width, int * height, Core::PixelFormat * format, unsigned char ** data)
{
	short sWidth,sHeight;
	reader->Seek(12,Core::SEEK_OFFSET_BEGIN);
	reader->Read(&sWidth,sizeof(short));
	*width = sWidth;

	reader->Seek(14,Core::SEEK_OFFSET_BEGIN);
	reader->Read(&sHeight,sizeof(short));
	*height = sHeight;

	unsigned char depth;
	reader->Seek(16,Core::SEEK_OFFSET_BEGIN);
	reader->Read(&depth,sizeof(char));

	int cCount = 3;

	if(depth == 24){
		cCount = 3;
		*format = Core::PIXEL_FORMAT_RGB_8B;
	}
	else if (depth == 32){
		cCount = 4;
		*format = Core::PIXEL_FORMAT_RGBA_8B;
	}
	else{
		THROW_EXCEPTION("Invalid TGA color depth");
	}

	unsigned int data_size = (*width)*(*height)*cCount;
	*data = new unsigned char[data_size];

	reader->Seek(18,Core::SEEK_OFFSET_BEGIN);
	reader->Read(data,data_size*sizeof(char));

	// Swap channels.
	for (unsigned int i = 0; i < data_size; i+= cCount)
	{
		unsigned char aux = (*data)[i];
		data[i] = data[i + 2];
		(*data)[i + 2] = aux;
	}
}
#endif

_CORE_BEGIN


Image::Image(){
	data = NULL;
	width = 0;
	height = 0;
	format = PIXEL_FORMAT_RGBA_8B;
	type = IMAGE_TYPE_RAW;
}

Image::~Image() {
	SafeArrayDelete(data);
}

Image * Image::Create(int width,int height,PixelFormat format){
	Image * img = new Image();
	img->width = width;
	img->height = height;
	img->format = format;
	if(format == PIXEL_FORMAT_RGB_8B)
		img->data = new unsigned char[3*img->width*img->height];
	else if(format == PIXEL_FORMAT_ALPHA_8B)
		img->data = new unsigned char[img->width*img->height];
	else if(format == PIXEL_FORMAT_RGBA_8B)
		img->data = new unsigned char[4*img->width*img->height];
	return img;
}

Image * Image::Load(const std::string & filename)
{
	Image * img = new Image();
	DataReader * reader = NULL;
	try{			
		ImageType type = Core::Image::GetImageType(filename);

		reader = FileReader::Open(filename);

        if(reader == NULL)
            THROW_EXCEPTION("Unable to open image file");
        
		switch(type)
		{
#ifdef NCK_PNG
			case IMAGE_TYPE_PNG:
				LoadPNG(reader,&(img->width),&(img->height),&(img->format),&(img->data));
				break;
#endif
#ifdef NCK_JPG
			case IMAGE_TYPE_JPEG:
				LoadJPG(reader,&(img->width),&(img->height),&(img->format),&(img->data));
				break;
#endif
#ifdef NCK_TGA
			case IMAGE_TYPE_TGA:
				LoadTGA(reader,&(img->width),&(img->height),&(img->format),&(img->data));
				break;
#endif
#ifdef NCK_BMP
			case IMAGE_TYPE_BMP:
				LoadBMP(reader,&(img->width),&(img->height),&(img->format),&(img->data));
				break;
#endif
			case IMAGE_TYPE_TBIN:
				LoadTBIN(reader,&(img->width),&(img->height),&(img->format),&(img->data));
				break;

			default:
				break;
		}
	}
	catch(const Core::Exception & ex)
	{
		SafeDelete(img);
		SafeDelete(reader);
		THROW_EXCEPTION_STACK("Unable to load image",ex);
	}
    
    SafeDelete(reader);

	return img;
}

Image * Image::Load(ImageType type,DataReader * reader){
	Image * img = new Image();
	try{
		switch(type){
#ifdef NCK_PNG
		case IMAGE_TYPE_PNG:
			LoadPNG(reader,&(img->width),&(img->height),&(img->format),&(img->data));
			break;
#endif
#ifdef NCK_JPG
		case IMAGE_TYPE_JPEG:
			LoadJPG(reader,&(img->width),&(img->height),&(img->format),&(img->data));
			break;
#endif
#ifdef NCK_TGA
		case IMAGE_TYPE_TGA:
			LoadTGA(reader,&(img->width),&(img->height),&(img->format),&(img->data));
			break;
#endif
#ifdef NCK_BMP
		case IMAGE_TYPE_BMP:
			LoadBMP(reader,&(img->width),&(img->height),&(img->format),&(img->data));
			break;
#endif
		case IMAGE_TYPE_TBIN:
			LoadTBIN(reader,&(img->width),&(img->height),&(img->format),&(img->data));
			break;
		default:
			break;
		}

	}catch(const Core::Exception & ex){
		SafeDelete(img);
		THROW_EXCEPTION_STACK("Unable to load image",ex);
	}

	return img;
}

ImageType Image::GetImageType(const std::string & filename){
	std::string extension = FindExtension(filename);
	if(extension == "tga")
		return IMAGE_TYPE_TGA;
	else if(extension == "jpg")
		return IMAGE_TYPE_JPEG;
	else if(extension == "png")
		return IMAGE_TYPE_PNG;
	else if(extension == "bmp")
		return IMAGE_TYPE_BMP;
	else if(extension == "tbin")
		return IMAGE_TYPE_TBIN;
	else 
		THROW_EXCEPTION("File type not supported with extension \""+extension+"\"");
}

int Image::GetWidth(){
	return width;
}

int Image::GetHeight(){
	return height;
}


ImageType Image::GetType(){
	return type;
}

unsigned char * Image::GetData(){
	return data;
}

PixelFormat Image::GetFormat(){
	return format;
}

void Image::Save(const std::string & filename,ImageType type, float quality){
	unsigned char * output = NULL;

	try{
		int fsize = Save(type,quality,&output);

		FILE * fOut = fopen(ResolveFilename(filename).c_str(),"wb");
		
		if(fOut){
			fwrite(output,fsize,sizeof(char),fOut);
			fclose(fOut);
		}
		else 
			THROW_EXCEPTION("Unable to open file to write image");

		SafeDelete(output);
	}
	catch(Core::Exception & e){
		SafeDelete(output);
		THROW_EXCEPTION_STACK("Unable to save image to file",e);
	}
}

int Image::Save(ImageType type, float quality, unsigned char ** output)
{
	switch(type){
#ifdef NCK_PNG
		case IMAGE_TYPE_PNG:
			return SavePNG(width,height,format,data,output);
			break;
#endif
#ifdef NCK_JPG
		case IMAGE_TYPE_JPEG:
			return SaveJPG(width,height,data,quality,output);
			break;
#endif
#ifdef NCK_TGA
		case IMAGE_TYPE_TGA:
			return SaveTGA(width,height,format,data,output);
			break;
#endif
#ifdef NCK_BMP
		case IMAGE_TYPE_BMP:
			return SaveBMP(width,height,data,output);
			break;
#endif
		case IMAGE_TYPE_TBIN:
			return SaveTBIN(width,height,format,data,output);
			break;

        default:
			break;
    }
    
	return 0;
}

Image * Image::GenerateCircle(int size,bool expand,PixelFormat format){

	if(format == PIXEL_FORMAT_RGB_8B)
		return NULL;

	Core::Image * temp = Core::Image::Create(size,size,format);

	if(expand)
		size+=2;

	float center[2] = {size*0.5f,size*0.5f};

	float scaleOffset = size/(float)(size-1);

	float powSize = size*size*0.25f;

	for(int y = 0;y<temp->GetHeight();y++)
	{
		int byteOffset = y* temp->GetWidth()*4;
		for(int x = 0;x<temp->GetWidth();x++)
		{
			int samples = 0;
			int total = 0;
			for(float i = -0.5;i<=0.5;i+=0.25)
				for(float j = -0.5;j<=0.5;j+=0.25)
				{
					float offset[2] = {x*scaleOffset+i-center[0],
							y*scaleOffset+j-center[1]};
					if(expand){
						offset[0]+=1;
						offset[1]+=1;
					}
					if((offset[0]*offset[0]+offset[1]*offset[1])<powSize)
					{
						samples++;
					}

					total++;
				}

			float alpha = (float)samples/(float)total;

			if(format == PIXEL_FORMAT_ALPHA_8B)
			{
				temp->GetData()[byteOffset+x] = (unsigned char)(255*alpha);
			}
			else if(format == PIXEL_FORMAT_RGBA_8B)
			{
				temp->GetData()[byteOffset+x*4] = 255;
				temp->GetData()[byteOffset+x*4+1] = 255;
				temp->GetData()[byteOffset+x*4+2] = 255;
				temp->GetData()[byteOffset+x*4+3] = (unsigned char)(255*alpha);
			}
		}
	}
	return temp;
}

Image * Image::GenerateCheck(int size,PixelFormat format)
{
	if(format == PIXEL_FORMAT_RGB_8B)
			return NULL;

	Core::Image * temp = Core::Image::Create(size,size,format);

	float scaleOffset = size/(float)(size-1);

	float lineWidth = size/7.0f;

	Core::Point posA = Core::Point(lineWidth,size/4.0f+lineWidth);
	Core::Point posB = Core::Point(size/2.0f - lineWidth*0.5f,size/2.0f+lineWidth);
	Core::Point posC = Core::Point(size-lineWidth*0.5f,lineWidth);

	for(int y = 0;y<temp->GetHeight();y++)
	{
		int byteOffset = y* temp->GetWidth()*4;
		for(int x = 0;x<temp->GetWidth();x++)
		{
			int samples = 0;
			int total = 0;
			for(float i = -0.5;i<=0.5;i+=0.25)
				for(float j = -0.5;j<=0.5;j+=0.25)
				{
					float offset[2] = {x*scaleOffset+i,
							y*scaleOffset+j};


					float distA = 0;
					Point::DistanceToLine(posA,posB,offset[0],offset[1],&distA);

					float distB = 0;
					Point::DistanceToLine(posB,posC,offset[0],offset[1],&distB);

					if(distA <= lineWidth*0.5 || distB<=lineWidth*0.5)
						samples++;

					total++;
				}

			float alpha = (float)samples/(float)total;

			if(format == PIXEL_FORMAT_ALPHA_8B)
			{
				temp->GetData()[byteOffset+x] = (unsigned char)(255*alpha);
			}
			else if(format == PIXEL_FORMAT_RGBA_8B){
				temp->GetData()[byteOffset+x*4] = 255;
				temp->GetData()[byteOffset+x*4+1] = 255;
				temp->GetData()[byteOffset+x*4+2] = 255;
				temp->GetData()[byteOffset+x*4+3] = (unsigned char)(255*alpha);
			}
		}
	}
	return temp;
}


bool ImageCopy(int srcWidth,int srcHeight,PixelFormat srcFormat,unsigned char * src,
	 int dstWidth,int dstHeight,PixelFormat dstFormat, unsigned char * dst)
{
	//int srcStride = 3;
	//int dstStride = 4;

	if(srcFormat == PIXEL_FORMAT_RGBA_8B)
	{
		if(dstFormat == PIXEL_FORMAT_RGBA_8B)
		{
			int widthCopy = dstWidth - srcWidth;
			if(widthCopy<0)	widthCopy = dstWidth;
			widthCopy *= 4;
			for(int y = 0;y<srcHeight;y++){
				unsigned char * srcAdress = src + y * srcWidth * 4;
				unsigned char * dstAdress = dst + y * dstWidth * 4;
				memcpy(dstAdress,srcAdress,widthCopy);
			}
			return true;
		}
		else if(dstFormat == PIXEL_FORMAT_RGB_8B){
			for(int y = 0;y<srcHeight;y++){
				unsigned char * srcAdress = src + y * srcWidth * 4;
				unsigned char * dstAdress = dst + y * dstWidth * 3;
				for(int x = 0;x<dstWidth;x++){
					unsigned char * xSrcAdress = srcAdress + x * 4;
					unsigned char * xDstAdress = dstAdress + x * 3;
					xDstAdress[0] = xSrcAdress[0];
					xDstAdress[1] = xSrcAdress[1];
					xDstAdress[2] = xSrcAdress[2];
				}
			}
			return true;
		}
	}
	else if(srcFormat ==PIXEL_FORMAT_RGB_8B)
	{
		if(dstFormat == PIXEL_FORMAT_RGBA_8B)
		{
			int widthCopy = dstWidth - srcWidth;
			if(widthCopy<0)	widthCopy = dstWidth;
			widthCopy *= 3;
			for(int y = 0;y<srcHeight;y++){
				unsigned char * srcAdress = src + y * srcWidth * 3;
				unsigned char * dstAdress = dst + y * dstWidth * 3;
				memcpy(dstAdress,srcAdress,widthCopy);
			}
			return true;
		}
		else if(dstFormat == PIXEL_FORMAT_RGBA_8B){
			for(int y = 0;y<srcHeight;y++){
				unsigned char * srcAdress = src + y * srcWidth * 3;
				unsigned char * dstAdress = dst + y * dstWidth * 4;
				for(int x = 0;x<dstWidth;x++){
					unsigned char * xSrcAdress = srcAdress + x * 3;
					unsigned char * xDstAdress = dstAdress + x * 4;
					xDstAdress[0] = xSrcAdress[0];
					xDstAdress[1] = xSrcAdress[1];
					xDstAdress[2] = xSrcAdress[2];
					xDstAdress[3] = 0xFF;
				}
			}
			return true;
		}
	}

	return false;
}

_CORE_END
