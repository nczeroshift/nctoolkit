
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_DATAIO_H_
#define NCK_DATAIO_H_

#include "nckCoreConfig.h"
#include <stdint.h>
#include <string>

_CORE_BEGIN

/**
 * Get absolute path for toolkit data folder.
 */
std::string GetDataFolder();

/**
 * Retreive file extension part from filename.
 * @return String with file extension, empty otherwise.
 */
std::string FindExtension(const std::string & filename);

/**
 * Resolve file system path from URL scheme. 
 */
std::string ResolveFilename(const std::string & filename);

/* Get file last modified date */
int64_t GetFileLastModified(const std::string & filename);

/**
 * File seek mode.
 */
enum SeekOffsetMode{
	SEEK_OFFSET_BEGIN,
	SEEK_OFFSET_END,
	SEEK_OFFSET_CURRENT,
};

/**
 * Interface data reader class.
 */
class DataReader{
public:
	virtual ~DataReader(){};

	/**
	 * Read data from stream.
	 * @return Number of bytes read, 0 on failure.
	 */
	virtual size_t Read(void * data, size_t size) = 0;

	/**
	 * Read string from binary stream.
	 * @param str Reference to output string.
	 * @return True if the string was correctly read.
	 */
	virtual bool Read(std::string * str) = 0;

	/**
	 * Tell current position in the stream.
	 * @return Returns -1 if the end of the stream is reached.
	 */
	virtual int64_t Tell() = 0;

	/**
	 * Seek to position in the stream.
	 * @return True if seek was successfull.
	 */
	virtual bool Seek(int64_t pos, SeekOffsetMode mode) = 0;
};

/**
 * Data reader using a file stream.
 */
class FileReader: public virtual DataReader{
public:
    /**
     * Open a file reader from path.
     * @return NULL if file not found or open failure.
	 */
	static FileReader * Open(const std::string & filename);
 
	/**
	 * Get file size on disk from path.
	 * @return Returns file size, 
	 */
	static int64_t Size(const std::string & filename);

	/**
	 * Check if file exists on path.
	 */
	static bool Exists(const std::string & filename);

	virtual ~FileReader();
	size_t Read(void * data, size_t size);
	bool Read(std::string * str);
	int64_t Tell();
	bool Seek(int64_t pos,SeekOffsetMode mode);

private:
	FILE * fHandle;
};

/**
 * Class for data reading from a byte array.
 */
class MemoryReader: public virtual DataReader{
public:
	/**
	 * Constructor
	 * @param data Byte array pointer
	 * @param size Byte array size
	 * @para copy If true, copy all data and released it on destructor.
	 */
	MemoryReader(uint8_t *data, int64_t size, bool copy);

	virtual ~MemoryReader();
	int64_t GetSize();
	size_t Read(void * data, size_t size);
	bool Read(std::string * str);
	int64_t Tell();
	bool Seek(int64_t pos,SeekOffsetMode mode);

protected:
	bool mCopy;
	int64_t mStart;
	int64_t mSize;
	uint8_t * mData;
};

/**
 * TAR files class reader.
 */
class TarReader{
public:
	class Entry : public MemoryReader{
	public:
		virtual ~Entry();

		/// Get entry filename title.
		std::string GetName();

	protected:
		friend class TarReader;

		Entry(const std::string & name,uint8_t *data,size_t size);

		std::string mName;
	};

	TarReader(DataReader * reader);
	~TarReader();

	/**
	 * Returns the next archive file inside the Tar file.
	 */
	Entry * Next();

private:
	DataReader * fReader;
};

/**
 * Interface class for data writer.
 */
class DataWriter{
public:
	virtual ~DataWriter(){}

	/**
	 * Write data to stream.
	 * @eturn Number of bytes writen to stream, 0 on failure.
	 */
	virtual size_t Write(uint8_t * data, size_t size) = 0;

	/// Return current write position.
	virtual int64_t Tell() = 0;

	/// Change writing position.
	virtual bool Seek(int64_t pos, SeekOffsetMode mode) = 0;
};

_CORE_END

#endif // #ifndef NCK_DATAIO_H_
