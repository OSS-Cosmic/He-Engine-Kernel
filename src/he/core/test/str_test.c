#include "he/core/str.h"
#include "he/core/alloc.h"
#include "he/core/log.h"
#include "utest.h"

UTEST(str, str_caseless_compare)
{
    EXPECT_EQ(str_caseless_compare(c_to_str_span("test"), c_to_str_span("TEST")), 0);
    EXPECT_EQ(str_caseless_compare(c_to_str_span("testAA"), c_to_str_span("TEST")), 1);
}

UTEST(str, str_casless_equal)
{
    EXPECT_EQ(str_casless_equal(c_to_str_span("Hello world"), c_to_str_span("Hello world")), 1);
    EXPECT_EQ(str_casless_equal(c_to_str_span("Helloworld"), c_to_str_span("Hello world")), 0);
    EXPECT_EQ(str_casless_equal(c_to_str_span("Hello World"), c_to_str_span("Hello world")), 1);
    EXPECT_EQ(str_casless_equal(c_to_str_span("Hello"), c_to_str_span("Hello ")), 0);
}

UTEST(str, str_compare)
{
    EXPECT_EQ(str_compare(c_to_str_span("test"), c_to_str_span("TEST")), 32);
    EXPECT_EQ(str_compare(c_to_str_span("Test"), c_to_str_span("test")), -32);
}

UTEST(str, str_index_of)
{
    EXPECT_EQ(str_index_of(c_to_str_span("foo foo"), c_to_str_span("foo")), 0);
    EXPECT_EQ(str_index_of(c_to_str_span("banana"), c_to_str_span("ana")), 1);
    EXPECT_EQ(str_index_of(c_to_str_span("textbook"), c_to_str_span("book")), 4);
    EXPECT_EQ(str_index_of(c_to_str_span("abababcabababcbab"), c_to_str_span("ababc")), 2);
    EXPECT_EQ(str_index_of(c_to_str_span("This is a test string to see how Boyer-Moore handles longer searches"), c_to_str_span("searches")), 60);
    EXPECT_EQ(str_index_of(c_to_str_span("hello world"), c_to_str_span("hello")), 0);
    EXPECT_EQ(str_index_of(c_to_str_span("apple banana"), c_to_str_span("nana")), 8);
    EXPECT_EQ(str_index_of(c_to_str_span("goodbye"), c_to_str_span("bye")), 4);
    EXPECT_EQ(str_index_of(c_to_str_span("cat"), c_to_str_span("dog")), -1);
    EXPECT_EQ(str_index_of(c_to_str_span(""), c_to_str_span("abc")), -1);
    EXPECT_EQ(str_index_of(c_to_str_span("hello"), c_to_str_span("")), -1);
    EXPECT_EQ(str_index_of(c_to_str_span("abc"), c_to_str_span("abcd")), -1);
}

UTEST(str, str_read_double) 
{
    double result = 0;
    EXPECT_TRUE(str_read_double(c_to_str_span(".01"), &result));
    EXPECT_NEAR(result, .01f, .0001f);
    
    EXPECT_TRUE(str_read_double(c_to_str_span("5.01"), &result));
    EXPECT_NEAR(result, 5.01f, .0001f);
    
    EXPECT_TRUE(str_read_double(c_to_str_span("5.01"), &result));
    EXPECT_NEAR(result, 5.01f, .0001f);
    
    EXPECT_TRUE(str_read_double(c_to_str_span("-5.01"), &result));
    EXPECT_NEAR(result, -5.01f, .0001f);
}

UTEST(str, str_read_float)
{
    float result = 0;
    EXPECT_TRUE(str_read_float(c_to_str_span(".01"), &result));
    EXPECT_NEAR(result, .01f, .0001f);
    
    EXPECT_TRUE(str_read_float(c_to_str_span("5.01"), &result));
    EXPECT_NEAR(result, 5.01f, .0001f);
    
    EXPECT_TRUE(str_read_float(c_to_str_span("5.01"), &result));
    EXPECT_NEAR(result, 5.01f, .0001f);
    
    EXPECT_TRUE(str_read_float(c_to_str_span("-5.01"), &result));
    EXPECT_NEAR(result, -5.01f, .0001f);

}

UTEST(str, qLastIndexOf)
{
    EXPECT_EQ(str_last_index_of(c_to_str_span("foo foo"), c_to_str_span(" ")), 3);
    EXPECT_EQ(str_last_index_of(c_to_str_span("foo foo"), c_to_str_span("foo")), 4);
    EXPECT_EQ(str_last_index_of(c_to_str_span("banana"), c_to_str_span("ana")), 3);
    EXPECT_EQ(str_last_index_of(c_to_str_span("textbook"), c_to_str_span("book")), 4);
    EXPECT_EQ(str_last_index_of(c_to_str_span("abababcabababcbab"), c_to_str_span("ababc")), 9);
    EXPECT_EQ(str_last_index_of(c_to_str_span("This is a test string to see how Boyer-Moore handles longer searches"), c_to_str_span("searches")), 60);
    EXPECT_EQ(str_last_index_of(c_to_str_span("hello world"), c_to_str_span("hello")), 0);
    EXPECT_EQ(str_last_index_of(c_to_str_span("apple banana"), c_to_str_span("nana")), 8);
    EXPECT_EQ(str_last_index_of(c_to_str_span("goodbye"), c_to_str_span("bye")), 4);
    EXPECT_EQ(str_last_index_of(c_to_str_span("cat"), c_to_str_span("dog")), -1);
    EXPECT_EQ(str_last_index_of(c_to_str_span(""), c_to_str_span("abc")), -1);
    EXPECT_EQ(str_last_index_of(c_to_str_span("hello"), c_to_str_span("")), -1);
    EXPECT_EQ(str_last_index_of(c_to_str_span("abc"), c_to_str_span("abcd")), -1);
}

UTEST(str, qEq)
{
    EXPECT_EQ(str_equal(c_to_str_span("Hello world"), c_to_str_span("Hello world")), 1);
    EXPECT_EQ(str_equal(c_to_str_span("Helloworld"), c_to_str_span("Hello world")), 0);
    EXPECT_EQ(str_equal(c_to_str_span("Hello World"), c_to_str_span("Hello world")), 0);
    EXPECT_EQ(str_equal(c_to_str_span("Hello"), c_to_str_span("Hello ")), 0);
}

UTEST(str, qDuplicate)
{
  struct he_str s1 = {0};
  EXPECT_EQ(str_assign(&default_alloc, &s1, c_to_str_span("Hello World")), true);
  struct he_str s2 = str_dup(&default_alloc, &s1);
  EXPECT_NE(s1.buf, s2.buf);
  str_free(&default_alloc, &s1);
  str_free(&default_alloc, &s2);
}

UTEST(str, qIterateRev)
{
  struct he_str_span buf = c_to_str_span("one two three four five");
  struct str_split_iterable iterable = {buf, c_to_str_span(" "), buf.len};
  struct he_str_span s = {0};
  s = str_split_rev_iter(&iterable);
  EXPECT_EQ(str_empty(s), 0);
  EXPECT_EQ(str_equal(c_to_str_span("five"), s), 1);
  s = str_split_rev_iter(&iterable);
  EXPECT_EQ(str_empty(s), 0);
  EXPECT_EQ(str_equal(c_to_str_span("four"), s), 1);
  s = str_split_rev_iter(&iterable);
  EXPECT_EQ(str_empty(s), 0);
  EXPECT_EQ(str_equal(c_to_str_span("three"), s), 1);
  s = str_split_rev_iter(&iterable);
  EXPECT_EQ(str_empty(s), 0);
  EXPECT_EQ(str_equal(c_to_str_span("two"), s), 1);
  s = str_split_rev_iter(&iterable);
  EXPECT_EQ(str_empty(s), 0);
  EXPECT_EQ(str_equal(c_to_str_span("one"), s), 1);
  s = str_split_rev_iter(&iterable);
  EXPECT_EQ(str_empty(s), 1);
}

UTEST(str, str_read_ull)
{
    unsigned long long res = 0;
    EXPECT_EQ(str_read_ull(c_to_str_span("123456"), &res), 1);
    EXPECT_EQ(res, 123456);
}

UTEST(str, str_read_ll)
{
    long long res = 0;
    EXPECT_EQ(str_read_ll(c_to_str_span("123456"), &res), 1);
    EXPECT_EQ(res, 123456);
}

UTEST(str, str_split_iter__whitespace)
{
    struct str_split_iterable  iterable = { c_to_str_span("one  two"), c_to_str_span(" "), 0 };
    struct he_str_span           s = { 0 };

    s = str_split_iter(&iterable);
    EXPECT_EQ(str_empty(s), 0);
    EXPECT_EQ(str_equal(c_to_str_span("one"), s), 1);

    s = str_split_iter(&iterable);
    EXPECT_EQ(str_empty(s), 1);

    s = str_split_iter(&iterable);
    EXPECT_EQ(str_equal(c_to_str_span("two"), s), 1);

    s = str_split_iter(&iterable);
    EXPECT_EQ(str_empty(s), 1);
}

UTEST(str, iterate_count)
{
    {
        struct str_split_iterable  iterable = { c_to_str_span("one two three four five"), c_to_str_span(" "), 0 };
        struct he_str_span slices[] = {
            c_to_str_span("one"), c_to_str_span("two"), c_to_str_span("three"), c_to_str_span("four"), c_to_str_span("five"),
        };
        size_t i = 0;
        while (iterable.cursor < iterable.buffer.len)
        {
            struct he_str_span it = str_split_iter(&iterable);
            EXPECT_EQ(str_equal(slices[i++], it), 1);
        }
    }
    {
        struct str_split_iterable  iterable = { c_to_str_span("one   two"), c_to_str_span(" "), 0 };
        struct he_str_span slices[] = {
            c_to_str_span("one"),
            c_to_str_span(""),
            c_to_str_span(""),
            c_to_str_span("two"),
        };
        size_t i = 0;
        while (iterable.cursor < iterable.buffer.len)
        {
            struct he_str_span it = str_split_iter(&iterable);
            EXPECT_EQ(str_equal(slices[i++], it), 1);
        }
    }
}

UTEST(str, qIterate)
{
    struct str_split_iterable  iterable = { c_to_str_span("one two three four five"), c_to_str_span(" "),  0 };

    struct he_str_span s = { 0 };
    s = str_split_iter(&iterable);
    EXPECT_EQ(str_empty(s), 0);
    EXPECT_EQ(str_equal(c_to_str_span("one"), s), 1);
    s = str_split_iter(&iterable);
    EXPECT_EQ(str_empty(s), 0);
    EXPECT_EQ(str_equal(c_to_str_span("two"), s), 1);
    s = str_split_iter(&iterable);
    EXPECT_EQ(str_empty(s), 0);
    EXPECT_EQ(str_equal(c_to_str_span("three"), s), 1);
    s = str_split_iter(&iterable);
    EXPECT_EQ(str_empty(s), 0);
    EXPECT_EQ(str_equal(c_to_str_span("four"), s), 1);
    s = str_split_iter(&iterable);
    EXPECT_EQ(str_empty(s), 0);
    EXPECT_EQ(str_equal(c_to_str_span("five"), s), 1);
    s = str_split_iter(&iterable);
    EXPECT_EQ(str_empty(s), 1);
}

UTEST(str, str_fmt_ll)
{
    char   test_buffer[STR_LLSTR_SIZE];
    size_t len = str_fmt_ll((struct he_str_span){  test_buffer, STR_LLSTR_SIZE }, 12481);
    EXPECT_EQ(len, 5);
    EXPECT_EQ(str_equal(c_to_str_span("12481"), (struct he_str_span){ test_buffer, len }), true);
}


UTEST(str, str_cat_join)
{
    struct he_str buf = { 0 };
    struct he_str_span slices[] = {
        c_to_str_span("one"),
        c_to_str_span("two"),
        c_to_str_span("three"),
        c_to_str_span("four"),
    };
    EXPECT_EQ(str_cat_join(&default_alloc, &buf, slices, 4, c_to_str_span(" ")), true);
    EXPECT_EQ(str_equal(str_to_str_span(buf), c_to_str_span("one two three four")), true);
    str_free(&default_alloc,&buf);
}

UTEST(str, str_cat_join_c)
{
    struct he_str buf = { 0 };
    const char* slices[] = {
        "one",
        "two",
        "three",
        "four",
    };
    EXPECT_EQ(str_cat_join_c(&default_alloc, &buf, slices, 4, c_to_str_span(" ")), true);
    EXPECT_EQ(str_equal(str_to_str_span(buf), c_to_str_span("one two three four")), true);
    str_free(&default_alloc, &buf);
}

UTEST(str, str_append_slice)
{
    struct he_str buf = { 0 };
    str_assign(&default_alloc ,&buf, c_to_str_span("Hello"));
    EXPECT_EQ(str_append_slice(&default_alloc,&buf, c_to_str_span(" world")), true);
    EXPECT_EQ(str_equal(str_to_str_span(buf), c_to_str_span("Hello world")), true);
    str_free(&default_alloc,&buf);
}

UTEST(str, str_cat_printf)
{
    struct he_str buf = { 0 };
    EXPECT_EQ(str_cat_printf(&default_alloc, &buf, "Hello %s", "world"), true);
    EXPECT_EQ(str_equal(str_to_str_span(buf), c_to_str_span("Hello world")), true);

    str_set_len(&default_alloc,&buf, 0);
    EXPECT_EQ(str_cat_printf(&default_alloc,&buf, "%d", 123), true);
    EXPECT_EQ(str_equal(str_to_str_span(buf), c_to_str_span("123")), true);

    EXPECT_EQ(str_cat_printf(&default_alloc,&buf, " %lu", 156), true);
    EXPECT_EQ(str_equal(str_to_str_span(buf), c_to_str_span("123 156")), true);

    str_set_len(&default_alloc,&buf, 0);
    EXPECT_EQ(str_cat_printf(&default_alloc, &buf, "a%cb", 0), true);
    EXPECT_EQ(str_equal(str_to_str_span(buf), (struct he_str_span){ (char*)"a\0" "b",(size_t)3 }), 1);
    str_free(&default_alloc,&buf);
}

UTEST(str, str_cat_fmt) {
  struct he_str s = {0};
  {
    str_cat_fmt(&default_alloc, &s, "%S\n", c_to_str_span("Hello World"));
    EXPECT_EQ(str_equal(str_to_str_span(s), c_to_str_span("Hello World\n")),
              true);
    str_clear(&default_alloc, &s);
  }
  {
    str_cat_fmt(&default_alloc, &s, "%i\n", 125);
    EXPECT_EQ(str_equal(str_to_str_span(s), c_to_str_span("125\n")), true);
    str_clear(&default_alloc, &s);
  }
  {
    str_cat_fmt(&default_alloc, &s, "%i\n", -125);
    EXPECT_EQ(str_equal(str_to_str_span(s), c_to_str_span("-125\n")), true);
    str_clear(&default_alloc, &s);
  }
  str_free(&default_alloc, &s);
}

UTEST(str, str_update_len)
{
    struct he_str buf = { 0 };
    str_assign(&default_alloc,&buf, c_to_str_span("Hello World"));
    buf.buf[5] = '\0';
    str_update_len(&buf);

    EXPECT_EQ(str_equal(str_to_str_span(buf), c_to_str_span("Hello")), true);
    EXPECT_EQ(buf.len, 5);

    str_free(&default_alloc,&buf);
}

UTEST(str, str_assign)
{
    struct he_str buf = { 0 };
    {
        str_assign(&default_alloc, &buf, c_to_str_span("Hello World"));
        EXPECT_EQ(str_equal(str_to_str_span(buf), c_to_str_span("Hello World")), true);
    }
    {
        str_assign(&default_alloc, &buf, str_trim(c_to_str_span("   Hello World   ")));
        EXPECT_EQ(str_equal(str_to_str_span(buf), c_to_str_span("Hello World")), true);
    }
    str_free(&default_alloc, &buf);
}

UTEST(str, str_r_trim)
{
    EXPECT_EQ(str_equal(str_r_trim(c_to_str_span("Hello world  ")), c_to_str_span("Hello world")), true);
    EXPECT_EQ(str_equal(str_r_trim(c_to_str_span("  Hello world  ")), c_to_str_span("  Hello world")), true);
    EXPECT_EQ(str_equal(str_r_trim(c_to_str_span("  ")), c_to_str_span("")), true);
    EXPECT_EQ(str_equal(str_r_trim(c_to_str_span(" ")), c_to_str_span("")), true);
    EXPECT_EQ(str_equal(str_r_trim(c_to_str_span("\n")), c_to_str_span("")), true);
    EXPECT_EQ(str_equal(str_r_trim(c_to_str_span("\t")), c_to_str_span("")), true);
}

UTEST(str, str_l_trim)
{
    EXPECT_EQ(str_equal(str_l_trim(c_to_str_span("Hello world  ")), c_to_str_span("Hello world  ")), true);
    EXPECT_EQ(str_equal(str_l_trim(c_to_str_span("  ")), c_to_str_span("")), true);
    EXPECT_EQ(str_equal(str_l_trim(c_to_str_span(" ")), c_to_str_span("")), true);
    EXPECT_EQ(str_equal(str_l_trim(c_to_str_span("\n")), c_to_str_span("")), true);
    EXPECT_EQ(str_equal(str_l_trim(c_to_str_span("\t")), c_to_str_span("")), true);
}

UTEST(str, str_trim)
{
    {
      struct he_str buf = {0};
      str_assign(&default_alloc, &buf, c_to_str_span("  Hello World "));
      str_assign(&default_alloc, &buf, str_trim(str_to_str_span(buf)));
      EXPECT_EQ(str_equal(str_to_str_span(buf), c_to_str_span("Hello World")), true);
      str_free(&default_alloc, &buf);
    }

    EXPECT_EQ(str_equal(str_trim(c_to_str_span("Hello world  ")), c_to_str_span("Hello world")), true);
    EXPECT_EQ(str_equal(str_trim(c_to_str_span("  \t Hello world  ")), c_to_str_span("Hello world")), true);
    EXPECT_EQ(str_equal(str_trim(c_to_str_span("  ")), c_to_str_span("")), true);
    EXPECT_EQ(str_equal(str_trim(c_to_str_span(" ")), c_to_str_span("")), true);
    EXPECT_EQ(str_equal(str_trim(c_to_str_span("\n")), c_to_str_span("")), true);
    EXPECT_EQ(str_equal(str_trim(c_to_str_span("\t")), c_to_str_span("")), true);
}

UTEST(str, str_sscanf)
{
    {
        uint32_t a = 0;
        uint32_t b = 0;
        int      read = str_sscanf(c_to_str_span("10.132"), "%u.%u", &a, &b);
        EXPECT_EQ(read, 2);
        EXPECT_EQ(a, 10);
        EXPECT_EQ(b, 132);
    }
}

// UTEST(str, qSliceToUtf16CodePoint) {
//   unsigned char leftPointingMagnify[] = {0x3D,0xD8,0x0D,0xDD};
//
//   struct tf_utf_result_s res;
//   {
//     struct str_split_iterable  iter = {
//       .buffer = (struct he_str_span) {
//         .buf = (const char*)leftPointingMagnify,
//         .len = 2,
//       },
//       .cursor = 0
//     };
//     res = tfUtf16NextCodePoint(&iter);
//     EXPECT_EQ((bool)res.invalid, true);
//     EXPECT_EQ((bool)res.finished, true);
//   }
//   {
//     struct str_split_iterable  iter = {
//       .buffer = (struct he_str_span) {
//         .buf = (const char*)leftPointingMagnify,
//         .len = sizeof(leftPointingMagnify),
//       },
//       .cursor = 0
//     };
//     res = tfUtf16NextCodePoint(&iter);
//     EXPECT_EQ(res.codePoint, 0x1F50D);

//     EXPECT_EQ((bool)res.finished, true);
//   }
//   {
//     struct str_split_iterable  iter = (struct str_split_iterable ){
//       .buffer = (struct he_str_span) {
//         .buf = NULL,
//         .len = 0,
//       },
//       .cursor = 0
//     };
//     res = tfUtf16NextCodePoint(&iter);
//     EXPECT_EQ((bool)res.invalid, true);
//     EXPECT_EQ((bool)res.finished, true);
//   }
//   {
//     char badInput[] = {0};
//     struct str_split_iterable  iter = (struct str_split_iterable ){
//         .buffer = (struct he_str_span){
//           .buf = badInput,
//           .len = 0,
//         },
//         .cursor = 0};
//     res = tfUtf16NextCodePoint(&iter);
//     EXPECT_EQ((bool)res.invalid, true);
//     EXPECT_EQ((bool)res.finished, true);
//   }
//
// }

// UTEST(str, qSliceToUtf8CodePoint) {
//   char smilyCat[] = {0xF0, 0x9F, 0x98, 0xBC};
//   struct str_split_iterable  iter = {
//     .buffer = (struct he_str_span) {
//       .len = sizeof(smilyCat),
//       .buf = smilyCat
//     },
//     .cursor = 0
//   };
//
//   struct tf_utf_result_s res = tfUtf8NextCodePoint(&iter);
//   EXPECT_EQ(res.codePoint, 0x0001f63c);
//
//   char charU[] = {'U'};
//   EXPECT_EQ(tfSliceToUtf8CodePoint((struct he_str_span) {
//     .len = sizeof(charU),
//     .buf = charU
//   }, 0), 'U');
//
//   char ringOperator[] = {0xe2, 0x88, 0x98};
//   EXPECT_EQ(tfSliceToUtf8CodePoint((struct he_str_span) {
//     .len = sizeof(ringOperator),
//     .buf = ringOperator
//   }, 0), 0x2218);
//
//   // this has an extra byte
//   char badRingOperator[] = {0xe2, 0x88, 0x98, 0x1};
//   EXPECT_EQ(tfSliceToUtf8CodePoint((struct he_str_span) {
//     .len = sizeof(badRingOperator),
//     .buf = badRingOperator
//   }, 1), 1);
// }

// UTEST(str, qUtf8CodePointIter) {
//   {
//     unsigned char smilyCat[] = {0xF0, 0x9F, 0x98, 0xBC};
//     struct str_split_iterable  iter = {
//       .buffer = (struct he_str_span) {
//         .buf = (char*)smilyCat,
//         .len = sizeof(smilyCat),
//       },
//       .cursor = 0
//     };
//     struct tf_utf_result_s res = tfUtf8NextCodePoint(&iter);
//     EXPECT_EQ(res.codePoint, 0x0001f63c);
//     EXPECT_EQ((bool)res.finished, true);
//     EXPECT_EQ((bool)res.invalid, false);
//   }
// {
//   // Ḽơᶉëᶆ
//   unsigned char buffer[] = {0xE1, 0xB8, 0xBC, 0xC6, 0xA1, 0xE1, 0xB6, 0x89,0xC3, 0xAB,0xE1, 0xB6,0x86 };
//   struct str_split_iterable  iterable = {
//     .buffer = {
//       .buf = (const char*)buffer,
//       .len = sizeof(buffer)
//     },
//     .cursor = 0,
//   };
//   struct tf_utf_result_s s = {0};
//   s = tfUtf8NextCodePoint(&iterable);
//   EXPECT_EQ(s.codePoint, 0x00001E3C); // 0xE1, 0xB8, 0xBC
//   EXPECT_EQ((bool)s.finished, false);
//   EXPECT_EQ((bool)s.invalid, false);
//   s = tfUtf8NextCodePoint(&iterable);
//   EXPECT_EQ(s.codePoint,0x1a1); // 0xBC, 0xC6
//   EXPECT_EQ((bool)s.finished, false);
//   EXPECT_EQ((bool)s.invalid, false);
//   s = tfUtf8NextCodePoint(&iterable);
//   EXPECT_EQ(s.codePoint,0x1D89); // 0xE1, 0xB6, 0x89
//   EXPECT_EQ((bool)s.finished, false);
//   EXPECT_EQ((bool)s.invalid, false);
//   s = tfUtf8NextCodePoint(&iterable);
//   EXPECT_EQ(s.codePoint,0x00EB); // 0xC3 0xAB
//   EXPECT_EQ((bool)s.finished, false);
//   EXPECT_EQ((bool)s.invalid, false);
//   s = tfUtf8NextCodePoint(&iterable);
//   EXPECT_EQ(s.codePoint,0x1D86); //0xE1 0xB6 0x86
//   EXPECT_EQ((bool)s.finished, true);
//   EXPECT_EQ((bool)s.invalid, false);
//   s = tfUtf8NextCodePoint(&iterable);
//   EXPECT_EQ((bool)s.finished, true);
//   EXPECT_EQ((bool)s.invalid, true);
//   }
// }

UTEST_STATE();
int main(int argc, const char *const argv[]) {
  he_init_alloc();
  he_init_log("str_test",eALL);
  int res = utest_main(argc, argv);
  he_exit_log();
  he_exit_alloc();
  return res;
}

