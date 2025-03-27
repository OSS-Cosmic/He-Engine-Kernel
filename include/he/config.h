
// configurations
#define RI_MAX_SWAPCHAIN_IMAGES 8
#define RI_NUMBER_FRAMES_FLIGHT 3


#define HE_MUTEX_DEFAULT_SPIN_COUNT 1500

// alloc
#ifdef NDEBUG
#else
  // track allocatos for debug 
  #define HE_ALLOC_TRACKED 1 
#endif

#if (defined(_WINDOWS_) || defined(_WINDOWS_H))
  #define HE_TARGET_WINDOWS
#elif defined(__APPLE__)
  #define HE_TARGET_APPLE
#else
  #define HE_TARGET_UNIX
#endif

#ifdef NDEBUG
#define HE_RELEASE 
#else
#define HE_DEBUG 
#endif

#define HE_ASSERT_INTERACTIVE 

// default
//#define HE_ALLOC_BACKTRACE
//#define HE_ALLOC_ALWAYS_LOG
//#define HE_ALLOC_ASAN

#define HE_ALLOC_HASH_BITS (12u)
#define HE_ALLOC_RANDOM_WIPE (false)
#define HE_ALLOC_ALWAYS_VALIDATE_ALL (false)
#define HE_ALLOC_ALWAYS_LOG_ALL (false)
#define HE_ALLOC_ALWAYS_WIPE_ALL (true)
#define HE_ALLOC_CLEANUP_LOG_ON_FIRST_RUN (true)
#define HE_ALLOC_PADDING_SIZE (4)
// stress
//#define HE_ALLOC_HASH_BITS (12u)
//#define HE_ALLOC_RANDOM_WIPE (true)
//#define HE_ALLOC_ALWAYS_VALIDATE_ALL (true)
//#define HE_ALLOC_ALWAYS_LOG_ALL (true)
//#define HE_ALLOC_ALWAYS_WIPE_ALL (true)
//#define HE_ALLOC_CLEANUP_LOG_ON_FIRST_RUN (true)
//#define HE_ALLOC_PADDING_SIZE (1024)

#define HE_FS_MAX_PATH (512)

