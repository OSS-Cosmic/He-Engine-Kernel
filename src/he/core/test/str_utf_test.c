
#include "he/core/str_utf.h"
#include "he/core/alloc.h"
#include "he/core/log.h"
#include "utest.h"


UTEST(qstr_utf, qstrSliceToUtf16CodePoint) {
  unsigned char leftPointingMagnify[] = {0x3D,0xD8,0x0D,0xDD};
  struct he_str_utf_result res;
  {
    struct he_str_utf_iterable iter = {
      .buffer = (struct he_str_span) {
        .buf = (char*)leftPointingMagnify,
        .len = 2,
      },
      .cursor = 0
    };
    res = str_utf16_next_code_point(&iter);
    EXPECT_EQ((bool)res.invalid, true);
    EXPECT_EQ((bool)res.finished, true);
  }
  {
    struct he_str_utf_iterable iter = {
      .buffer = (struct he_str_span) {
        .buf = (char*)leftPointingMagnify,
        .len = sizeof(leftPointingMagnify),
      },
      .cursor = 0
    };
    res = str_utf16_next_code_point(&iter);
    EXPECT_EQ(res.codePoint, 0x1F50D);
    EXPECT_EQ((bool)res.finished, true);
  }
  {
    struct he_str_utf_iterable iter = {
      .buffer = (struct he_str_span) {
        .buf = NULL, 
        .len = 0,
      },
      .cursor = 0
    };
    res = str_utf16_next_code_point(&iter);
    EXPECT_EQ((bool)res.invalid, true);
    EXPECT_EQ((bool)res.finished, true);
  }
  {
    char badInput[] = {0};
    struct he_str_utf_iterable iter = {
        .buffer = (struct he_str_span){
          .buf = badInput,
          .len = 0, 
        },
        .cursor = 0};
    res = str_utf16_next_code_point(&iter);
    EXPECT_EQ((bool)res.invalid, true);
    EXPECT_EQ((bool)res.finished, true);
  }

}

//UTEST(bstr, bstrSliceToUtf8CodePoint) {
//  char smilyCat[] = {0xF0, 0x9F, 0x98, 0xBC};
//  struct bstr_utf_iterable_s iter = {
//    .buffer = (struct bstr_const_slice_s) {
//      .len = sizeof(smilyCat),
//      .buf = smilyCat
//    },
//    .cursor = 0
//  };
//
//  struct bstr_utf_result_s res = bstrUtf8NextCodePoint(&iter);
//  EXPECT_EQ(res.codePoint, 0x0001f63c);
//  
//  char charU[] = {'U'};
//  EXPECT_EQ(bstrSliceToUtf8CodePoint((struct bstr_const_slice_s) {
//    .len = sizeof(charU),
//    .buf = charU 
//  }, 0), 'U');
//
//  char ringOperator[] = {0xe2, 0x88, 0x98};
//  EXPECT_EQ(bstrSliceToUtf8CodePoint((struct bstr_const_slice_s) {
//    .len = sizeof(ringOperator),
//    .buf = ringOperator 
//  }, 0), 0x2218);
// 
//  // this has an extra byte 
//  char badRingOperator[] = {0xe2, 0x88, 0x98, 0x1};
//  EXPECT_EQ(bstrSliceToUtf8CodePoint((struct bstr_const_slice_s) {
//    .len = sizeof(badRingOperator),
//    .buf = badRingOperator 
//  }, 1), 1);
//}

UTEST(qstr_utf, qstrUtf8CodePointIter) {
  {
    unsigned char smilyCat[] = {0xF0, 0x9F, 0x98, 0xBC};
    struct he_str_utf_iterable iter = {
      .buffer = (struct he_str_span) {
        .buf = (char*)smilyCat,
        .len = sizeof(smilyCat),
      },
      .cursor = 0
    };
    struct he_str_utf_result res = str_utf8_next_code_point(&iter);
    EXPECT_EQ(res.codePoint, 0x0001f63c);
    EXPECT_EQ((bool)res.finished, true);
    EXPECT_EQ((bool)res.invalid, false);
  }
{
  // Ḽơᶉëᶆ
  unsigned char buffer[] = {0xE1, 0xB8, 0xBC, 0xC6, 0xA1, 0xE1, 0xB6, 0x89,0xC3, 0xAB,0xE1, 0xB6,0x86 };
  struct he_str_utf_iterable iterable = {
    .buffer = {
      .buf = (const char*)buffer,
      .len = sizeof(buffer)
    },
    .cursor = 0,
  };
  struct he_str_utf_result s = {0};
  s = str_utf8_next_code_point(&iterable);
  EXPECT_EQ(s.codePoint, 0x00001E3C); // 0xE1, 0xB8, 0xBC
  EXPECT_EQ((bool)s.finished, false);
  EXPECT_EQ((bool)s.invalid, false);
  s = str_utf8_next_code_point(&iterable);
  EXPECT_EQ(s.codePoint,0x1a1); // 0xBC, 0xC6
  EXPECT_EQ((bool)s.finished, false);
  EXPECT_EQ((bool)s.invalid, false);
  s = str_utf8_next_code_point(&iterable);
  EXPECT_EQ(s.codePoint,0x1D89); // 0xE1, 0xB6, 0x89
  EXPECT_EQ((bool)s.finished, false);
  EXPECT_EQ((bool)s.invalid, false);
  s = str_utf8_next_code_point(&iterable);
  EXPECT_EQ(s.codePoint,0x00EB); // 0xC3 0xAB
  EXPECT_EQ((bool)s.finished, false);
  EXPECT_EQ((bool)s.invalid, false);
  s = str_utf8_next_code_point(&iterable);
  EXPECT_EQ(s.codePoint,0x1D86); //0xE1 0xB6 0x86
  EXPECT_EQ((bool)s.finished, true);
  EXPECT_EQ((bool)s.invalid, false);
  s = str_utf8_next_code_point(&iterable);
  EXPECT_EQ((bool)s.finished, true);
  EXPECT_EQ((bool)s.invalid, true);
  }
}

UTEST_STATE();
int main(int argc, const char *const argv[]) {
  he_init_alloc();
  he_init_log("str_utf_test",eALL);
  int res = utest_main(argc, argv);
  he_exit_log();
  he_exit_alloc();
  return res;
}
