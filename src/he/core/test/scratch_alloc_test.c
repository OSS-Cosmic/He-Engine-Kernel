#include "he/core/scratch_alloc.h"
#include "he/core/log.h"

#include "utest.h"

UTEST(alloc, scratchAlloc_1) {
  struct he_scratch_allocator testAlloc = {0};
  struct he_scrach_alloc_desc desc = {0};
  desc.blockSize = 1024;
  desc.alignment = 16;
  init_scratch_allocator(&testAlloc, &desc);
  scratch_alloc(&testAlloc, 128);
  scratch_alloc(&testAlloc, 128);
  scratch_alloc(&testAlloc, 128);
  scratch_alloc(&testAlloc, 128);
  scratch_alloc(&testAlloc, 128);
  scratch_alloc(&testAlloc, 128);
  scratch_alloc(&testAlloc, 128);
  scratch_alloc(&testAlloc, 128);
  free_scratch_allocator(&testAlloc);
}

UTEST(alloc, scratchAlloc_2) {
  struct he_scratch_allocator test_alloc = {0};
  struct he_scrach_alloc_desc desc = {0};
  desc.blockSize = 1024;
  desc.alignment = 16;

  init_scratch_allocator(&test_alloc, &desc);
  scratch_alloc(&test_alloc, 128);
  scratch_alloc(&test_alloc, 128);
  scratch_alloc(&test_alloc, 128);
  void *data = scratch_alloc(&test_alloc, 2048);
  EXPECT_EQ(((size_t)data) % 16, 0);
  free_scratch_allocator(&test_alloc);
}

UTEST_STATE();
int main(int argc, const char *const argv[]) {
  he_init_alloc();
  he_init_log("scratch_alloc",eALL);
  int res = utest_main(argc, argv);
  
  he_exit_log();
  he_exit_alloc();
  return res;
}
