#include "he/core/filesystem.h"
#include "he/core/types.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

bool fs_open(const char *path, enum he_file_mode mode, struct he_file *fs) {
#ifdef HE_TARGET_UNIX
  memset(fs, 0, sizeof *fs);

  int oflags = 0;

  // 666
  mode_t omode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IROTH;

  if (mode & HE_FS_WRITE) {
    oflags |= O_CREAT;

    if (mode & HE_FS_APPEND) {
      oflags |= O_APPEND;
    } else {
      oflags |= O_TRUNC;
    }

    if (mode & HE_FS_READ) {
      oflags |= O_RDWR;
    } else {
      oflags |= O_WRONLY;
    }
  } else {
    oflags |= O_RDONLY;
  }
  fs->mode = mode;
  int fd = open(path, oflags, omode);

  // Might fail to open the file for read+write if file doesn't exist
  if (fd < 0) {
    // LOGF(eERROR, "Error opening file '%s': %s", filePath, strerror(errno));
    return false;
  }
#endif
return false;
}
bool fs_close(struct he_file *file) {
#ifdef HE_TARGET_UNIX
  return file->fd < 0 || close(file->fd) == 0;
#endif
  return false;
}

ssize_t fs_seek_start_of_file(struct he_file *file, ssize_t offset) {
#ifdef HE_TARGET_UNIX
  return lseek(file->fd, offset, SEEK_SET);
#endif
  return -1;
}

ssize_t fs_seek_cur_pos(struct he_file *file, ssize_t offset) {
#ifdef HE_TARGET_UNIX
  return lseek(file->fd, offset, SEEK_CUR);
#endif
  return -1;
}
bool fs_is_open(struct he_file* file) {
#ifdef HE_TARGET_UNIX
  return file && file->fd > 0;
#endif
  return -1;

}


ssize_t fs_get_seek_position(struct he_file *file) {
#ifdef HE_TARGET_UNIX
  return lseek(file->fd, 0, SEEK_CUR);
#endif
  return -1;
}
ssize_t fs_get_size(struct he_file *file) {
#ifdef HE_TARGET_UNIX
  struct stat finfo;
  if (fstat(file->fd, &finfo) == 0) {
    return finfo.st_size;
  }
#endif
  return -1;
}

bool fs_flush_file(struct he_file *file) {
  if (!(file->mode & HE_FS_WRITE))
    return true;
#ifdef HE_TARGET_UNIX
  if (!fsync(file->fd))
    return true;
#endif
  return false;
}

ssize_t  fs_append_to_file(struct he_file* file, const void* buf, size_t len) {
#ifdef HE_TARGET_UNIX
  return write(file->fd, buf, len);
#endif
  return -1;
}

bool fs_is_at_end(struct he_file *file) {
  return fs_get_seek_position(file) >= fs_get_size(file);
}
