#ifndef HE_FILESYSTEM_H_INCLUDED
#define HE_FILESYSTEM_H_INCLUDED

#include "he/core/types.h"
#include "he/config.h"

enum HeFileMode_e {
  // Get read access for file. Error if file not exist.
  HE_FS_READ = 1 << 0,

  // Get write access for file. File is created if not exist.
  HE_FS_WRITE = 1 << 1,

  // Set initial seek position to the end of file.
  HE_FS_APPEND = 1 << 2,

  // Read access for other processes.
  // Note: flag is required for Windows&Xbox.
  //       On other platforms read access is always available.
  HE_FS_ALLOW_READ = 1 << 4,

  // RW mode
  HE_FS_READ_WRITE = HE_FS_READ | HE_FS_WRITE,

  // W mode and set position to the end
  HE_FS_WRITE_APPEND = HE_FS_WRITE | HE_FS_APPEND,

  // R mode and set position to the end
  HE_FS_READ_APPEND = HE_FS_READ | HE_FS_APPEND,

  // RW mode and set position to the end
  HE_FS_READ_WRITE_APPEND = HE_FS_READ | HE_FS_APPEND,

  // -- mode and -- and also read access for other processes.
  HE_FS_WRITE_ALLOW_READ = HE_FS_WRITE | HE_FS_ALLOW_READ,
  HE_FS_READ_WRITE_ALLOW_READ = HE_FS_READ_WRITE | HE_FS_ALLOW_READ,
  HE_FS_WRITE_APPEND_ALLOW_READ = HE_FS_WRITE_APPEND | HE_FS_ALLOW_READ,
  HE_FS_READ_WRITE_APPEND_ALLOW_READ = HE_FS_READ_WRITE_APPEND | HE_FS_ALLOW_READ,
};


struct HeFileStream {
  union {
#ifdef HE_TARGET_UNIX 
    struct  {
      int fd;
    } unixFS;
#endif
  };
};

struct HeFilesystem {
  bool (*open)(struct HeFilesystem *pIO, const char *path,
               enum HeFileMode_e mode, struct HeFileStream *pOut);

  /// Closes and invalidates the file stream.
  bool (*Close)(FileStream *pFile);

  /// Returns the number of bytes read.
  size_t (*Read)(FileStream *pFile, void *outputBuffer,
                 size_t bufferSizeInBytes);

  /// Reads at most `bufferSizeInBytes` bytes from sourceBuffer and writes them
  /// into the file. Returns the number of bytes written.
  size_t (*Write)(FileStream *pFile, const void *sourceBuffer,
                  size_t byteCount);

  /// Seeks to the specified position in the file, using `baseOffset` as the
  /// reference offset.
  bool (*Seek)(FileStream *pFile, SeekBaseOffset baseOffset,
               ssize_t seekOffset);

  /// Gets the current seek position in the file.
  ssize_t (*GetSeekPosition)(FileStream *pFile);

  /// Gets the current size of the file. Returns -1 if the size is unknown or
  /// unavailable.
  ssize_t (*GetFileSize)(FileStream *pFile);

  /// Flushes all writes to the file stream to the underlying subsystem.
  bool (*Flush)(FileStream *pFile);

  /// Returns whether the current seek position is at the end of the file
  /// stream.
  bool (*IsAtEnd)(FileStream *pFile);

  const char *(*GetResourceMount)(ResourceMount mount);

  // Acquire unique file identifier.
  // Only Archive FS supports it currently.
  bool (*GetFileUid)(IFileSystem *pIO, ResourceDirectory rd, const char *name,
                     uint64_t *outUid);

  // Open file using unique identifier. Use GetFileUid to get uid.
  bool (*OpenByUid)(IFileSystem *pIO, uint64_t uid, FileMode fm,
                    FileStream *pOut);

  // Creates virtual address space of file.
  // When memory mapping is done, file can be accessed just like an array.
  // This is more efficient than using "FILE" stream.
  // Not all platforms are supported.
  // Use fsStreamWrapMemoryMap for strong cross-platform compatibility.
  // This function does read-only memory map.
  bool (*MemoryMap)(FileStream *fs, size_t *outSize, void const **outData);

  void* userData;
};


bool heOpen(const char* path, enum HeFileMode_e mode, struct HeFileStream* pOut);
bool heClose(const char* path, enum HeFileMode_e mode, struct HeFileStream* pOut);


#endif

