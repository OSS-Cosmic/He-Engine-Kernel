
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


// default
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
