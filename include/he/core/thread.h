#include "he/core/types.h"
#include "he/config.h"

struct HeThread {

};

struct he_mutex {
#ifdef HE_TARGET_WINDOWS 
    CRITICAL_SECTION handle;
#else
    pthread_mutex_t handle;
    uint32_t        spinCount;
#endif

};
   
struct he_mutex_desc {
  uint32_t spinCount;
};
static struct he_mutex_desc DefaultHeMutexDesc = {
  .spinCount = HE_MUTEX_DEFAULT_SPIN_COUNT 
};
bool he_init_mutex(struct he_mutex_desc *desc, struct he_mutex *pMutex);
void he_destroy_mutex(struct he_mutex* pMutex);

void he_acquire_mutex(struct he_mutex* pMutex);
bool he_try_acquire_mutex(struct he_mutex* pMutex);
void he_release_mutex(struct he_mutex* pMutex);

