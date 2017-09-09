
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckOggStream.h"
#include "nckUtils.h"
#include "nckException.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <sstream>

_AUDIO_BEGIN

#ifdef NCK_OGG

OggStream::OggStream(Core::DataReader * srcReader){
	srcReader->Seek(0,Core::SEEK_OFFSET_END);
	fLength = srcReader->Tell();

	srcReader->Seek(0,Core::SEEK_OFFSET_BEGIN);
	reader = srcReader;

	ogg_sync_init(&oy);
	vorbis_info_init(&vi);
	vorbis_comment_init(&vc);

	memset(&vb,0,sizeof(vorbis_block));
	memset(&vd,0,sizeof(vorbis_dsp_state));
	memset(&os,0,sizeof(ogg_stream_state));

	currentTime = 0;
    lastGranulate = -1;
}

void OggStream::Init()
{
	char * buffer = ogg_sync_buffer(&oy,4096);
	int bytes = reader->Read(buffer,4096);

	ogg_sync_wrote(&oy,bytes);

	if(ogg_sync_pageout(&oy,&og)!=1 && bytes<4096){
		THROW_EXCEPTION("Input does not appear to be an Ogg bitstream");
	}

	ogg_stream_init(&os,ogg_page_serialno(&og));

	if(ogg_stream_pagein(&os,&og)<0){
		THROW_EXCEPTION("Error reading first page of Ogg bitstream data");
	}

	if(ogg_stream_packetout(&os,&op)!=1){
		THROW_EXCEPTION("Error reading initial header packet");
	}

	if(vorbis_synthesis_headerin(&vi,&vc,&op)<0){
		THROW_EXCEPTION("This Ogg bitstream does not contain Vorbis audio data");
	}

	int i=0;
	while(i<2){
		while(i<2){
			int result=ogg_sync_pageout(&oy,&og);
			if(result==0)break;
			if(result==1){
				ogg_stream_pagein(&os,&og);
				while(i<2){
					result=ogg_stream_packetout(&os,&op);
					if(result==0)break;
					if(result<0){
						THROW_EXCEPTION("Corrupt secondary header");
						return;
					}
					result=vorbis_synthesis_headerin(&vi,&vc,&op);
					if(result<0){
						THROW_EXCEPTION("Corrupt secondary header");
					}
					i++;
				}
			}
		}

		buffer = ogg_sync_buffer(&oy,4096);
		bytes = reader->Read(buffer,4096);

		if(bytes==0 && i<2){
			THROW_EXCEPTION("End of file before finding all Vorbis headers");
		}

		ogg_sync_wrote(&oy,bytes);
	}

	if(vorbis_synthesis_init(&vd,&vi)==0)
		vorbis_block_init(&vd,&vb);
}

OggStream::~OggStream(){
	SafeDelete(reader);

	vorbis_block_clear(&vb);
	vorbis_dsp_clear(&vd);

	ogg_stream_clear(&os);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);

	ogg_sync_clear(&oy);
}

int OggStream::GetSampleRate(){
	return vi.rate;
}

int OggStream::GetChannelsCount(){
	return vi.channels;
}

Format OggStream::GetFormat(){
	return FORMAT_S16;
}

int OggStream::Read(int dataSize, uint8_t * dataBuffer)
{
	bool decode = true;
	int convsize = dataSize / vi.channels;
	int retSize = 0;
	int8_t * convbuffer = (int8_t*)dataBuffer;

	char * buffer = NULL;
	int bytes = 0;

	while(decode)
	{
		while(decode)
		{
			int result = ogg_sync_pageout(&oy,&og);

			if(result==0) break; // Need more data
			if(result<0){
				printf("OggStream::Read, Corrupt or missing data in ogg bitstream, continuing...\n");
			}
			else{
				ogg_stream_pagein(&os,&og);
				while(1)
				{
					result=ogg_stream_packetout(&os,&op);

					if(result==0)break; // Need more data
					if(result<0){
						// Corrupt data
					}
					else{
						float **pcm;
						int samples;

						if(vorbis_synthesis(&vb,&op)==0)
							vorbis_synthesis_blockin(&vd,&vb);

						while((samples=vorbis_synthesis_pcmout(&vd,&pcm))>0)
						{
                            int j;
							//int clipflag=0;
							int bout=(samples<convsize?samples:convsize);

							// convert floats to 16 bit signed ints (host order) and interleave
							for(int i=0;i<vi.channels;i++)
							{
								ogg_int16_t *ptr=(int16_t*)convbuffer+i;
								float  *mono=pcm[i];
								for(j=0;j<bout;j++)
								{
									int val=floor(mono[j]*32767.f+.5f);

									/// might as well guard against clipping
									if(val>32767){
										val=32767;
									}
									if(val<-32768){
										val=-32768;
									}
									*ptr=val;
									ptr+=vi.channels;
								}
							}

							convbuffer += 2*vi.channels*bout;
							retSize += 2*vi.channels*bout;

							if(retSize>=dataSize)
								decode = false;

							vorbis_synthesis_read(&vd,bout);
						}
					}
				}

				if(ogg_page_eos(&og)){
                    updateGranulate(retSize);
					return retSize;
				}
			}
		}

		buffer = ogg_sync_buffer(&oy,4096);
		bytes = reader->Read(buffer,4096);
		ogg_sync_wrote(&oy,bytes);

		if(bytes==0){
            updateGranulate(retSize);
			return retSize;
		}
	}

    updateGranulate(retSize);

	return retSize;
}

void OggStream::updateGranulate(int dataSize) {
    currentTime = vorbis_granule_time(&vd, vd.granulepos) * 1e6;
}

OggStream * OggStream::Load(const std::string & filename)
{
	Core::DataReader * f = Core::DataReader::Open(filename);

	if(!f)
		THROW_EXCEPTION("Unable to open the ogg file");

	OggStream * stream = new OggStream(f);

	try{
		stream->Init();
	}catch(const Core::Exception & ex){
		THROW_EXCEPTION_STACK("Error reading OGG data from file",ex);
		SafeDelete(f);
	}

	return stream;
}

OggStream * OggStream::Load(Core::DataReader * reader){
	OggStream * stream = new OggStream(reader);
    try{
		stream->Init();
	}catch(const Core::Exception & ex){
		THROW_EXCEPTION_STACK("Error reading OGG data from data reader",ex);
		SafeDelete(stream);
		return NULL;
	}
	return stream;
}

int64_t OggStream::GetDuration(){
	return 8 * (1e6 * fLength) / vi.bitrate_nominal;
}

int64_t OggStream::GetTime() {
	return currentTime;
}

void OggStream::SetTime(int64_t position) {
    int64_t bytes = (fLength * position) / GetDuration();
	reader->Seek(bytes, Core::SEEK_OFFSET_BEGIN);
    ogg_stream_reset(&os);
	vorbis_synthesis_restart(&vd);   
}

#endif

_AUDIO_END

