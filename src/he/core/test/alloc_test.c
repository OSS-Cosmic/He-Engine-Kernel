#include "he/core/alloc.h"
#include "utest.h"

UTEST(he_malloc, malloc) {
  void *a = he_malloc(1024);
  void *b = he_malloc(1024);
  void *c = he_malloc(1024);
  void *d = he_malloc(1024);

  he_free(a);
  he_free(b);
  he_free(c);
  he_free(d);
}

UTEST_STATE();
int main(int argc, const char *const argv[]) {
  he_init_alloc();
  he_init_log("str_test",eALL);
  int res = utest_main(argc, argv);
  he_exit_log();
  he_exit_alloc();
  return res;
}

