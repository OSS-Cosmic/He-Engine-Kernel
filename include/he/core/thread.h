#include "he/core/types.h"
#include "he/config.h"

struct HeThread {

};

struct HeMutex {
#ifdef HE_TARGET_WINDOWS 
    CRITICAL_SECTION handle;
#else
    pthread_mutex_t handle;
    uint32_t        spinCount;
#endif

};
   
struct HeMutexDesc {
  uint32_t spinCount;
};
static struct HeMutexDesc DefaultHeMutexDesc = {
  .spinCount = HE_MUTEX_DEFAULT_SPIN_COUNT 
};
bool heInitMutex(struct HeMutexDesc *desc, struct HeMutex *pMutex);
void heDestroyMutex(struct HeMutex* pMutex);

void heAcquireMutex(struct HeMutex* pMutex);
bool heTryAcquireMutex(struct HeMutex* pMutex);
void heReleaseMutex(struct HeMutex* pMutex);

