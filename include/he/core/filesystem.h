#ifndef HE_FILESYSTEM_H_INCLUDED
#define HE_FILESYSTEM_H_INCLUDED

#include "he/core/types.h"
#include "he/config.h"
#include "he/core/str.h"

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

struct he_file {
  uint8_t mode;
#ifdef HE_TARGET_UNIX
  int fd;
#endif
};

bool fs_is_open(struct he_file* file);
bool fs_open(const char *path, enum HeFileMode_e mode, struct he_file *pOut);
bool fs_close(struct he_file *file);
ssize_t fs_seek_start_of_file(struct he_file *file, ssize_t offset);
ssize_t fs_seek_cur_pos(struct he_file *file, ssize_t offset);
ssize_t fs_get_seek_position(struct he_file *file);
ssize_t fs_get_size(struct he_file *file);
bool fs_flush_file(struct he_file *file);
bool fs_is_at_end(struct he_file *file);

ssize_t fs_append_to_file(struct he_file* file, const void* buf, size_t len);
static inline ssize_t fs_append_str_span_to_file(struct he_file* file, struct he_str_span sp) {
  return fs_append_to_file(file, sp.buf, sp.len);
}

#endif

