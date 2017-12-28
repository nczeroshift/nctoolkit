
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckWavStream.h"
#include "nckUtils.h"
#include "nckException.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <sstream>

_AUDIO_BEGIN

struct WavHeader {
    char ChunkID[4];
    uint32_t ChunkSize;
    char Format[4];

    char Subchunk1ID[4];
    uint32_t Subchunk1Size; // 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
    uint16_t AudioFormat;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;       // SampleRate * NumChannels * BitsPerSample/8
    uint16_t BlockAlign;     // NumChannels * BitsPerSample/8
    uint16_t BitsPerSample;  // 8, 16, etc bits per sample

    uint16_t ExtraParamSize;
    uint8_t * ExtraParams;

    char Subchunk2ID[4];
    uint32_t Subchunk2Size;// NumSamples * NumChannels * BitsPerSample/8

    WavHeader() {
        ChunkID[0] = 'R';
        ChunkID[1] = 'I';
        ChunkID[2] = 'F';
        ChunkID[3] = 'F';

        Format[0] = 'W';
        Format[1] = 'A';
        Format[2] = 'V';
        Format[3] = 'E';

        Subchunk1ID[0] = 'f';
        Subchunk1ID[1] = 'm';
        Subchunk1ID[2] = 't';
        Subchunk1ID[3] = ' ';

        ChunkSize = 0;

        Subchunk1Size = 16;
        AudioFormat = 1;   // PCM Raw
        NumChannels = 0;
        SampleRate = 0;
        ByteRate = 0;
        BlockAlign = 0;
        BitsPerSample = 16;

        ExtraParamSize = 0;
        ExtraParams = NULL;

        Subchunk2ID[0] = 'd';
        Subchunk2ID[1] = 'a';
        Subchunk2ID[2] = 't';
        Subchunk2ID[3] = 'a';
        Subchunk2Size = 0;
    }

    ~WavHeader() {
        SafeArrayDelete(ExtraParams);
    }
    
    void Write(Core::DataWriter * reader) {
        reader->Write((uint8_t*)ChunkID, 4);
        reader->Write((uint8_t*)&ChunkSize, 4);
        reader->Write((uint8_t*)Format, 4);
        reader->Write((uint8_t*)Subchunk1ID, 4);
        reader->Write((uint8_t*)&Subchunk1Size, 4);
        reader->Write((uint8_t*)&AudioFormat, 2);
        reader->Write((uint8_t*)&NumChannels, 2);
        reader->Write((uint8_t*)&SampleRate, 4);
        reader->Write((uint8_t*)&ByteRate, 4);
        reader->Write((uint8_t*)&BlockAlign, 2);
        reader->Write((uint8_t*)&BitsPerSample, 2);
        if (AudioFormat != 1) {
            reader->Write((uint8_t*)&ExtraParamSize, 2);
            reader->Write((uint8_t*)ExtraParams, ExtraParamSize);
        }
        reader->Write((uint8_t*)Subchunk2ID, 4);
        reader->Write((uint8_t*)&Subchunk2Size, 4);
    }

    void Read(Core::DataReader * reader) {
        reader->Read(ChunkID, 4);
        reader->Read(&ChunkSize, 4);
        reader->Read(Format, 4);

        reader->Read(Subchunk1ID, 4);
        reader->Read(&Subchunk1Size, 4);
        reader->Read(&AudioFormat, 2);

        reader->Read(&NumChannels, 2);
        reader->Read(&SampleRate, 4);
        reader->Read(&ByteRate, 4);
        reader->Read(&BlockAlign, 2);
        reader->Read(&BitsPerSample, 2);

        if (AudioFormat != 1) {
            reader->Read(&ExtraParamSize, 2);

            if (ExtraParamSize > 0)
            {
                ExtraParams = new uint8_t[ExtraParamSize];
                reader->Read(ExtraParams, ExtraParamSize);
            }
        }

        reader->Read(Subchunk2ID, 4);
        reader->Read(&Subchunk2Size, 4);
    }
};

WavStream::WavStream(Core::DataReader * srcReader){
	srcReader->Seek(0,Core::SEEK_OFFSET_END);
	fLength = srcReader->Tell();

	srcReader->Seek(0,Core::SEEK_OFFSET_BEGIN);
	reader = srcReader;

    header = NULL;
}

void WavStream::Init() {
    header = new WavHeader();
    header->Read(reader);
    headerOffset = reader->Tell();
}

WavStream::~WavStream(){
    SafeDelete(header);
	SafeDelete(reader);
}

int WavStream::GetSampleRate(){
	return header->SampleRate;
}

int WavStream::GetChannelsCount(){
	return header->NumChannels;
}

Format WavStream::GetFormat(){
	return FORMAT_S16;
}

int WavStream::Read(int dataSize, uint8_t * dataBuffer)
{
    int ret = reader->Read(dataBuffer, dataSize);

    int64_t pos = reader->Tell();
    int64_t samplesCount = (pos - headerOffset) / (header->NumChannels * (header->BitsPerSample / 8));
	currentTime = 1e6 * samplesCount / (double)header->SampleRate;

    return ret;
}

WavStream * WavStream::Load(const std::string & filename)
{
	Core::DataReader * f = Core::DataReader::Open(filename);

	if(!f)
		THROW_EXCEPTION("Unable to open the WAV file");

    WavStream * stream = new WavStream(f);

	try{
		stream->Init();
	}catch(const Core::Exception & ex){
		THROW_EXCEPTION_STACK("Error reading WAV data from file",ex);
		SafeDelete(f);
	}

	return stream;
}

WavStream * WavStream::Load(Core::DataReader * reader){
    WavStream * stream = new WavStream(reader);
    try{
		stream->Init();
	}catch(const Core::Exception & ex){
		THROW_EXCEPTION_STACK("Error reading WAV data from data reader",ex);
		SafeDelete(stream);
		return NULL;
	}
	return stream;
}

int64_t WavStream::GetDuration(){
    int64_t samplesCount = header->Subchunk2Size / (header->NumChannels * (header->BitsPerSample / 8));
    return 1e6 * samplesCount / (double)header->SampleRate;
}

int64_t WavStream::GetTime() {
	return currentTime;
}

void WavStream::SetTime(int64_t position) {
    int64_t sampleId = header->SampleRate * (position / 1e6);
    int64_t sampleOffset = sampleId *  (header->NumChannels * (header->BitsPerSample / 8));
    reader->Seek(sampleOffset + headerOffset, Core::SEEK_OFFSET_BEGIN);
}

WavWriter::WavWriter(Core::DataWriter * writer) {
    this->writer = writer;
    header = new WavHeader();
    header->NumChannels = 2;
    header->SampleRate = 44100;
    headerOffset = 0;
    length = 0;
}

WavWriter::~WavWriter() {
    SafeDelete(this->header);
}

void WavWriter::SetSampleRate(int sampleRate) {
    header->SampleRate = sampleRate;
}

void WavWriter::SetChannelsCount(int count) {
    header->NumChannels = count;
}

void WavWriter::SetFormat(Format format) {
    header->AudioFormat = 1;
    if (format == FORMAT_S16) {
        header->BitsPerSample = 16;
    }
}

void WavWriter::Write(uint8_t * data, int size) {
    if (headerOffset == 0) {
        header->Write(writer);
        headerOffset = writer->Tell();
    }
    length += writer->Write(data, size);
}

void WavWriter::Finish() {
    header->Subchunk2Size = length;
    header->ByteRate = header->SampleRate * header->NumChannels * header->BitsPerSample / 8;
    header->ChunkSize = 36 + header->Subchunk2Size;
    header->BlockAlign = header->NumChannels * header->BitsPerSample / 8;
    writer->Seek(0, Core::SEEK_OFFSET_BEGIN);
    header->Write(writer);
}

_AUDIO_END

