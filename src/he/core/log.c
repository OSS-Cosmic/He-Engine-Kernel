#include "he/core/log.h"
#include "he/core/filesystem.h"
#include "he/core/thread.h"
#include "he/core/alloc.h"
#include "he/core/str.h"

#include "stb_ds.h"

#include <signal.h>

#define HE_LOGGER_MAX_BUFFER 1024

//#define MAX_THREAD_NAME_LENGTH 31
//#define LOG_PREAMBLE_SIZE  (56 + MAX_THREAD_NAME_LENGTH + FILENAME_NAME_LENGTH_LOG)
//#define LOG_LEVEL_SIZE     6
//#define LOG_MESSAGE_OFFSET (LOG_PREAMBLE_SIZE + LOG_LEVEL_SIZE)

struct log_callback {
  void *user_data;
  log_callback_fn callback;
  log_close_fn close;
  log_flush_fn flush;
  uint32_t level;
};

struct {
  bool ready;
  struct log_callback *cb;
  struct he_mutex mutex;
  uint32_t log_level;
} logger;

static THREAD_LOCAL struct {
  HE_STACK_ALLOCATOR_BASE
  uint8_t log_buffer[2048];
} logger_buffer = {&DECLARE_DEFAULT_HE_ALLOCATOR};

static void __addInitialLogFile(const char* appName)
{
    // Add new file with executable name

    const char*  extension = ".log";
    const size_t extensionLength = strlen(extension);

    char exeFileName[HE_FS_MAX_PATH] = { 0 };
    strcpy(exeFileName, appName);

    // Minimum length check
    if (exeFileName[0] == 0 || exeFileName[1] == 0)
    {
        strncpy(exeFileName, "Log", 4);
    }
    strncat(exeFileName, extension, extensionLength + 1);

    he_add_log_file(exeFileName, HE_FS_WRITE_ALLOW_READ, eALL);
}


void he_init_log(const char *appName, enum HeLogLevel_e level) {
  he_init_mutex(&DefaultHeMutexDesc, &logger.mutex);
  logger.log_level = level;
  logger.ready = true;
}

void he_exit_log(void) {
  he_release_mutex(&logger.mutex);
}

struct default_log_cb {
  struct he_file file;
};

static void defaultCallback(void* user_data, struct he_str_span* message)
{
    struct default_log_cb* fh = (struct default_log_cb*)user_data;
    fs_append_to_file(&fh->file, message->buf, message->len);  
    fs_flush_file(&fh->file);
}

// Close callback
static void defaultClose(void* user_data)
{
    struct default_log_cb* fh = (struct default_log_cb*)user_data;
    fs_close(&fh->file);
    he_free(fh);
}

// Flush callback
static void defaultFlush(void* user_data)
{
    struct default_log_cb* fh = (struct default_log_cb*)user_data;
    fs_flush_file(&fh->file);
}


void he_add_log_file(const char *filename, enum HeFileMode_e file_mode,
                  enum HeLogLevel_e log_level) {
  struct he_str log_path = {0};
  str_cat_fmt(&log_path, "./%s.log", filename);
  struct he_file file;
  if (fs_open(log_path.buf, HE_FS_WRITE, &file)) {
    struct default_log_cb *cb = he_malloc(sizeof(struct default_log_cb));
    cb->file = file;
    he_add_log_callback(log_level, cb, defaultCallback, defaultClose,
                        defaultFlush);
  } else {
    
  }
  str_free(&log_path);
}

void he_add_log_callback(uint32_t log_level, void *user_data,
                         log_callback_fn callback, log_close_fn close,
                         log_flush_fn flush) {

  he_acquire_mutex(&logger.mutex);
  struct log_callback cb;
  cb.user_data = user_data;
  cb.flush = flush;
  cb.callback = callback;
  cb.close = close;
  cb.level = log_level;
  arrpush(logger.cb, cb);
  he_release_mutex(&logger.mutex);
}

void he_write_log_va_list(uint32_t level, const char *filename, int line_number,
                        const char *message, va_list args) {
   const size_t prefix_len = 6;
   struct {
     uint32_t first;
     const char *second;
   } prefixes[] = {{eWARNING, "WARN| "},
                   {eINFO, "INFO| "},
                   {eDEBUG, " DBG| "},
                   {eERROR, " ERR| "}};
    
    struct he_allocator alloc = create_inplace_stack_alloc(&log_allocator);
    reset_stack_allocator(&alloc);
    struct he_str log_buffer = {&alloc};
    {

      time_t t = time(NULL);
      struct tm time_info;
#if defined(_WINDOWS) || defined(XBOX)
      localtime_s(&time_info, &t);
#elif defined(ORBIS) || defined(PROSPERO)
      localtime_s(&t, &time_info);
#elif defined(NX64)
      t = getTimeSinceStart();
      localtime_r(&t, &time_info);
#else
      localtime_r(&t, &time_info);
#endif
      str_cat_printf(&log_buffer, "%04d-%02d-%02d %02d:%02d:%02d ",
          1900 + time_info.tm_year, 1 + time_info.tm_mon, time_info.tm_mday,
          time_info.tm_hour, time_info.tm_min, time_info.tm_sec); 
    }

    const size_t preable_offset = log_buffer.len; 
    for (size_t i = 0; i < prefix_len; i++)
      str_append_char(&log_buffer, ' ');

    str_cat_vprintf(&log_buffer, message, args);
    str_append_char(&log_buffer, '\n');

    // Log for each flag
    for (uint32_t i = 0; i < HE_ARRAY_COUNT(prefixes); ++i) {
      if ((prefixes[i].first & level) == 0) {
        continue;
      }
      strncpy(log_buffer.buf + preable_offset, prefixes[i].second, prefix_len);
      struct he_str_span log_span = str_to_str_span(log_buffer);
      printf("%.*s", (int)log_buffer.len, log_span.buf);
      
      he_acquire_mutex(&logger.mutex);
      for (size_t k = 0; k < arrlen(logger.cb); k++) {
        if ((logger.cb[k].level & prefixes[i].first)) {
          logger.cb[k].callback(logger.cb[k].user_data, &log_span);
        }
      }
      he_release_mutex(&logger.mutex);
    }
    str_free(&log_buffer);
}
void he_write_log(uint32_t level, const char *filename, int line_number,
                const char *message, ...) {
    va_list args;
    va_start(args, message);
    he_write_log_va_list(level, filename, line_number, message, args);
    va_end(args);
}

void he_write_raw_log(uint32_t level, bool error, const char *message, ...) {
  reset_stack_allocator(logger_buffer);
  struct stack_allocator_user user = {&logger_buffer,
                                      sizeof(logger_buffer.log_buffer)};
  struct he_allocator alloc = fixed_stack_allocator(&user);

  struct he_str log_buffer = {};

  va_list args;
  va_start(args, message);
  str_cat_vprintf(&alloc, &log_buffer, message, args);
  va_end(args);
  struct he_str_span log_span = str_to_str_span(log_buffer);
  printf("%.*s", (int)log_span.len, log_span.buf);
  he_acquire_mutex(&logger.mutex);
  for (size_t k = 0; k < arrlen(logger.cb); k++) {
    if ((logger.cb[k].level & level)) {
      logger.cb[k].callback(logger.cb[k].user_data, &log_span);
    }
  }
  he_release_mutex(&logger.mutex);
  str_free(&alloc, &log_buffer);
}

//+V576, function:_FailedAssert, format_arg:4, ellipsis_arg:5
void __he_failed_assert(const char *file, int line, const char *statement,
                        const char *msg, ...) {
  reset_stack_allocator(logger_buffer);
  struct stack_allocator_user user = {&logger_buffer, sizeof(logger_buffer.log_buffer) };
  struct he_allocator alloc = fixed_stack_allocator(&user);

  struct he_str log_buffer = {};

  if (msg && msg[0]) {
    va_list args;
    va_start(args, msg);
    str_cat_vprintf(&alloc, &log_buffer, msg, args);
    va_end(args);
  }   
  if (logger.ready) {
    he_write_log(eERROR, file, line, "Assert failed: %s\nAssert message: %.*s",
                  statement, (int)log_buffer.len, log_buffer.buf);
  } else {
    printf("%.*s", (int)log_buffer.len, log_buffer.buf);
  }

#ifdef HE_ASSERT_INTERACTIVE
  #ifdef HE_TARGET_UNIX
  raise(SIGTRAP);
  #endif
#endif
}
