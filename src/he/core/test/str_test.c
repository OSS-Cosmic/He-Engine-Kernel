#include "../qstr.h"
#include "utest.h"

UTEST(qstr, qCaselessCompare)
{
    EXPECT_EQ(qStrCaselessCompare(CToStrRef("test"), CToStrRef("TEST")), 0);
    EXPECT_EQ(qStrCaselessCompare(CToStrRef("testAA"), CToStrRef("TEST")), 1);
}

UTEST(qstr, qCompare)
{
    EXPECT_EQ(qStrCompare(CToStrRef("test"), CToStrRef("TEST")), 32);
    EXPECT_EQ(qStrCompare(CToStrRef("Test"), CToStrRef("test")), -32);
}

UTEST(qstr, qIndexOf)
{
    EXPECT_EQ(qStrIndexOf(CToStrRef("foo foo"), CToStrRef("foo")), 0);
    EXPECT_EQ(qStrIndexOf(CToStrRef("banana"), CToStrRef("ana")), 1);
    EXPECT_EQ(qStrIndexOf(CToStrRef("textbook"), CToStrRef("book")), 4);
    EXPECT_EQ(qStrIndexOf(CToStrRef("abababcabababcbab"), CToStrRef("ababc")), 2);
    EXPECT_EQ(qStrIndexOf(CToStrRef("This is a test string to see how Boyer-Moore handles longer searches"), CToStrRef("searches")), 60);
    EXPECT_EQ(qStrIndexOf(CToStrRef("hello world"), CToStrRef("hello")), 0);
    EXPECT_EQ(qStrIndexOf(CToStrRef("apple banana"), CToStrRef("nana")), 8);
    EXPECT_EQ(qStrIndexOf(CToStrRef("goodbye"), CToStrRef("bye")), 4);
    EXPECT_EQ(qStrIndexOf(CToStrRef("cat"), CToStrRef("dog")), -1);
    EXPECT_EQ(qStrIndexOf(CToStrRef(""), CToStrRef("abc")), -1);
    EXPECT_EQ(qStrIndexOf(CToStrRef("hello"), CToStrRef("")), -1);
    EXPECT_EQ(qStrIndexOf(CToStrRef("abc"), CToStrRef("abcd")), -1);
}

UTEST(qstr, qStrReadDouble) 
{
    double result = 0;
    EXPECT_TRUE(qStrReadDouble(CToStrRef(".01"), &result));
    EXPECT_NEAR(result, .01f, .0001f);
    
    EXPECT_TRUE(qStrReadDouble(CToStrRef("5.01"), &result));
    EXPECT_NEAR(result, 5.01f, .0001f);
    
    EXPECT_TRUE(qStrReadDouble(CToStrRef("5.01"), &result));
    EXPECT_NEAR(result, 5.01f, .0001f);
    
    EXPECT_TRUE(qStrReadDouble(CToStrRef("-5.01"), &result));
    EXPECT_NEAR(result, -5.01f, .0001f);
}

UTEST(qstr, qStrReadFloat)
{
    float result = 0;
    EXPECT_TRUE(qStrReadFloat(CToStrRef(".01"), &result));
    EXPECT_NEAR(result, .01f, .0001f);
    
    EXPECT_TRUE(qStrReadFloat(CToStrRef("5.01"), &result));
    EXPECT_NEAR(result, 5.01f, .0001f);
    
    EXPECT_TRUE(qStrReadFloat(CToStrRef("5.01"), &result));
    EXPECT_NEAR(result, 5.01f, .0001f);
    
    EXPECT_TRUE(qStrReadFloat(CToStrRef("-5.01"), &result));
    EXPECT_NEAR(result, -5.01f, .0001f);

}

UTEST(qstr, qLastIndexOf)
{
    EXPECT_EQ(qStrLastIndexOf(CToStrRef("foo foo"), CToStrRef(" ")), 3);
    EXPECT_EQ(qStrLastIndexOf(CToStrRef("foo foo"), CToStrRef("foo")), 4);
    EXPECT_EQ(qStrLastIndexOf(CToStrRef("banana"), CToStrRef("ana")), 3);
    EXPECT_EQ(qStrLastIndexOf(CToStrRef("textbook"), CToStrRef("book")), 4);
    EXPECT_EQ(qStrLastIndexOf(CToStrRef("abababcabababcbab"), CToStrRef("ababc")), 9);
    EXPECT_EQ(qStrLastIndexOf(CToStrRef("This is a test string to see how Boyer-Moore handles longer searches"), CToStrRef("searches")), 60);
    EXPECT_EQ(qStrLastIndexOf(CToStrRef("hello world"), CToStrRef("hello")), 0);
    EXPECT_EQ(qStrLastIndexOf(CToStrRef("apple banana"), CToStrRef("nana")), 8);
    EXPECT_EQ(qStrLastIndexOf(CToStrRef("goodbye"), CToStrRef("bye")), 4);
    EXPECT_EQ(qStrLastIndexOf(CToStrRef("cat"), CToStrRef("dog")), -1);
    EXPECT_EQ(qStrLastIndexOf(CToStrRef(""), CToStrRef("abc")), -1);
    EXPECT_EQ(qStrLastIndexOf(CToStrRef("hello"), CToStrRef("")), -1);
    EXPECT_EQ(qStrLastIndexOf(CToStrRef("abc"), CToStrRef("abcd")), -1);
}

UTEST(qstr, qEq)
{
    EXPECT_EQ(qStrEqual(CToStrRef("Hello world"), CToStrRef("Hello world")), 1);
    EXPECT_EQ(qStrEqual(CToStrRef("Helloworld"), CToStrRef("Hello world")), 0);
    EXPECT_EQ(qStrEqual(CToStrRef("Hello World"), CToStrRef("Hello world")), 0);
    EXPECT_EQ(qStrEqual(CToStrRef("Hello"), CToStrRef("Hello ")), 0);
}

UTEST(qstr, qDuplicate)
{
    struct Str s1 = { 0 };
    EXPECT_EQ(qStrAssign(&s1, CToStrRef("Hello World")), true);
    struct Str s2 = qStrDup(&s1);
    EXPECT_NE(s1.buf, s2.buf);
    qStrFree(&s1);
    qStrFree(&s2);
}

UTEST(qstr, qIterateRev)
{
    struct StrSpan           buf = CToStrRef("one two three four five");
    struct qStrSplitIterable iterable = { buf, CToStrRef(" "), buf.len };
    struct StrSpan           s = { 0 };
    s = qStrSplitRevIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 0);
    EXPECT_EQ(qStrEqual(CToStrRef("five"), s), 1);
    s = qStrSplitRevIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 0);
    EXPECT_EQ(qStrEqual(CToStrRef("four"), s), 1);
    s = qStrSplitRevIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 0);
    EXPECT_EQ(qStrEqual(CToStrRef("three"), s), 1);
    s = qStrSplitRevIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 0);
    EXPECT_EQ(qStrEqual(CToStrRef("two"), s), 1);
    s = qStrSplitRevIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 0);
    EXPECT_EQ(qStrEqual(CToStrRef("one"), s), 1);
    s = qStrSplitRevIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 1);
}

UTEST(qstr, qReadull)
{
    unsigned long long res = 0;
    EXPECT_EQ(qStrReadull(CToStrRef("123456"), &res), 1);
    EXPECT_EQ(res, 123456);
}

UTEST(qstr, qReadu32)
{
    long long res = 0;
    EXPECT_EQ(qStrReadll(CToStrRef("123456"), &res), 1);
    EXPECT_EQ(res, 123456);
}

UTEST(qstr, qIterateWhiteSpace)
{
    struct qStrSplitIterable iterable = { CToStrRef("one  two"), CToStrRef(" "), 0 };
    struct StrSpan           s = { 0 };

    s = qStrSplitIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 0);
    EXPECT_EQ(qStrEqual(CToStrRef("one"), s), 1);

    s = qStrSplitIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 1);

    s = qStrSplitIter(&iterable);
    EXPECT_EQ(qStrEqual(CToStrRef("two"), s), 1);

    s = qStrSplitIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 1);
}

UTEST(qstr, qIterateCount)
{
    {
        struct qStrSplitIterable iterable = { CToStrRef("one two three four five"), CToStrRef(" "), 0 };
        struct StrSpan slices[] = {
            CToStrRef("one"), CToStrRef("two"), CToStrRef("three"), CToStrRef("four"), CToStrRef("five"),
        };
        size_t i = 0;
        while (iterable.cursor < iterable.buffer.len)
        {
            struct StrSpan it = qStrSplitIter(&iterable);
            EXPECT_EQ(qStrEqual(slices[i++], it), 1);
        }
    }
    {
        struct qStrSplitIterable iterable = { CToStrRef("one   two"), CToStrRef(" "), 0 };
        struct StrSpan slices[] = {
            CToStrRef("one"),
            CToStrRef(""),
            CToStrRef(""),
            CToStrRef("two"),
        };
        size_t i = 0;
        while (iterable.cursor < iterable.buffer.len)
        {
            struct StrSpan it = qStrSplitIter(&iterable);
            EXPECT_EQ(qStrEqual(slices[i++], it), 1);
        }
    }
}

UTEST(qstr, qIterate)
{
    struct qStrSplitIterable iterable = { CToStrRef("one two three four five"), CToStrRef(" "),  0 };

    struct StrSpan s = { 0 };
    s = qStrSplitIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 0);
    EXPECT_EQ(qStrEqual(CToStrRef("one"), s), 1);
    s = qStrSplitIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 0);
    EXPECT_EQ(qStrEqual(CToStrRef("two"), s), 1);
    s = qStrSplitIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 0);
    EXPECT_EQ(qStrEqual(CToStrRef("three"), s), 1);
    s = qStrSplitIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 0);
    EXPECT_EQ(qStrEqual(CToStrRef("four"), s), 1);
    s = qStrSplitIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 0);
    EXPECT_EQ(qStrEqual(CToStrRef("five"), s), 1);
    s = qStrSplitIter(&iterable);
    EXPECT_EQ(qStrEmpty(s), 1);
}

UTEST(qstr, qfmtWriteLongLong)
{
    char   test_buffer[STR_LLSTR_SIZE];
    size_t len = qstrfmtll((struct StrSpan){  test_buffer, STR_LLSTR_SIZE }, 12481);
    EXPECT_EQ(len, 5);
    EXPECT_EQ(qStrEqual(CToStrRef("12481"), (struct StrSpan){ test_buffer, len }), true);
}

UTEST(qstr, qCaselessEq)
{
    EXPECT_EQ(qStrCaselessEqual(CToStrRef("Hello world"), CToStrRef("Hello world")), 1);
    EXPECT_EQ(qStrCaselessEqual(CToStrRef("Helloworld"), CToStrRef("Hello world")), 0);
    EXPECT_EQ(qStrCaselessEqual(CToStrRef("Hello World"), CToStrRef("Hello world")), 1);
    EXPECT_EQ(qStrCaselessEqual(CToStrRef("Hello"), CToStrRef("Hello ")), 0);
}

UTEST(qstr, qCatJoin)
{
    struct Str     buf = { 0 };
    struct StrSpan slices[] = {
        CToStrRef("one"),
        CToStrRef("two"),
        CToStrRef("three"),
        CToStrRef("four"),
    };
    EXPECT_EQ(qstrcatjoin(&buf, slices, 4, CToStrRef(" ")), true);
    EXPECT_EQ(qStrEqual(ToStrRef(buf), CToStrRef("one two three four")), true);
    qStrFree(&buf);
}

UTEST(qstr, qCatJoinCstr)
{
    struct Str buf = { 0 };
    const char* slices[] = {
        "one",
        "two",
        "three",
        "four",
    };
    EXPECT_EQ(qstrcatjoinCStr(&buf, slices, 4, CToStrRef(" ")), true);
    EXPECT_EQ(qStrEqual(ToStrRef(buf), CToStrRef("one two three four")), true);
    qStrFree(&buf);
}

UTEST(qstr, appendSlice)
{
    struct Str buf = { 0 };
    qStrAssign(&buf, CToStrRef("Hello"));
    EXPECT_EQ(qStrAppendSlice(&buf, CToStrRef(" world")), true);
    EXPECT_EQ(qStrEqual(ToStrRef(buf), CToStrRef("Hello world")), true);
    qStrFree(&buf);
}

UTEST(qstr, qcatprintf)
{
    struct Str buf = { 0 };
    EXPECT_EQ(qstrcatprintf(&buf, "Hello %s", "world"), true);
    EXPECT_EQ(qStrEqual(ToStrRef(buf), CToStrRef("Hello world")), true);

    qStrSetLen(&buf, 0);
    EXPECT_EQ(qstrcatprintf(&buf, "%d", 123), true);
    EXPECT_EQ(qStrEqual(ToStrRef(buf), CToStrRef("123")), true);

    EXPECT_EQ(qstrcatprintf(&buf, " %lu", 156), true);
    EXPECT_EQ(qStrEqual(ToStrRef(buf), CToStrRef("123 156")), true);

    qStrSetLen(&buf, 0);
    EXPECT_EQ(qstrcatprintf(&buf, "a%cb", 0), true);
    EXPECT_EQ(qStrEqual(ToStrRef(buf), (struct StrSpan){ (char*)"a\0" "b",(size_t)3 }), 1);
    qStrFree(&buf);
}

UTEST(qstr, qcatfmt)
{
    struct Str s = { 0 };
    {
        qstrcatfmt(&s, "%S\n", CToStrRef("Hello World"));
        EXPECT_EQ(qStrEqual(ToStrRef(s), CToStrRef("Hello World\n")), true);
        qStrClear(&s);
    }
    {
        qstrcatfmt(&s, "%i\n", 125);
        EXPECT_EQ(qStrEqual(ToStrRef(s), CToStrRef("125\n")), true);
        qStrClear(&s);
    }
    {
        qstrcatfmt(&s, "%i\n", -125);
        EXPECT_EQ(qStrEqual(ToStrRef(s), CToStrRef("-125\n")), true);
        qStrClear(&s);
    }
    qStrFree(&s);
}

UTEST(qstr, updateLen)
{
    struct Str buf = { 0 };
    qStrAssign(&buf, CToStrRef("Hello World"));
    buf.buf[5] = '\0';
    qStrUpdateLen(&buf);

    EXPECT_EQ(qStrEqual(ToStrRef(buf), CToStrRef("Hello")), true);
    EXPECT_EQ(buf.len, 5);

    qStrFree(&buf);
}

UTEST(qstr, qStrAssign)
{
    struct Str buf = { 0 };
    {
        qStrAssign(&buf, CToStrRef("Hello World"));
        EXPECT_EQ(qStrEqual(ToStrRef(buf), CToStrRef("Hello World")), true);
    }
    {
        qStrAssign(&buf, qStrTrim(CToStrRef("   Hello World   ")));
        EXPECT_EQ(qStrEqual(ToStrRef(buf), CToStrRef("Hello World")), true);
    }
    qStrFree(&buf);
}

UTEST(qstr, q_rtrim)
{
    EXPECT_EQ(qStrEqual(qStrRTrim(CToStrRef("Hello world  ")), CToStrRef("Hello world")), true);
    EXPECT_EQ(qStrEqual(qStrRTrim(CToStrRef("  Hello world  ")), CToStrRef("  Hello world")), true);
    EXPECT_EQ(qStrEqual(qStrRTrim(CToStrRef("  ")), CToStrRef("")), true);
    EXPECT_EQ(qStrEqual(qStrRTrim(CToStrRef(" ")), CToStrRef("")), true);
    EXPECT_EQ(qStrEqual(qStrRTrim(CToStrRef("\n")), CToStrRef("")), true);
    EXPECT_EQ(qStrEqual(qStrRTrim(CToStrRef("\t")), CToStrRef("")), true);
}

UTEST(qstr, q_ltrim)
{
    EXPECT_EQ(qStrEqual(qStrLTrim(CToStrRef("Hello world  ")), CToStrRef("Hello world  ")), true);
    EXPECT_EQ(qStrEqual(qStrLTrim(CToStrRef("  ")), CToStrRef("")), true);
    EXPECT_EQ(qStrEqual(qStrLTrim(CToStrRef(" ")), CToStrRef("")), true);
    EXPECT_EQ(qStrEqual(qStrLTrim(CToStrRef("\n")), CToStrRef("")), true);
    EXPECT_EQ(qStrEqual(qStrLTrim(CToStrRef("\t")), CToStrRef("")), true);
}

UTEST(qstr, qStrTrim)
{
    {
        struct Str buf = { 0 };
        qStrAssign(&buf, CToStrRef("  Hello World "));
        qStrAssign(&buf, qStrTrim(ToStrRef(buf)));
        EXPECT_EQ(qStrEqual(ToStrRef(buf), CToStrRef("Hello World")), true);
        qStrFree(&buf);
    }

    EXPECT_EQ(qStrEqual(qStrTrim(CToStrRef("Hello world  ")), CToStrRef("Hello world")), true);
    EXPECT_EQ(qStrEqual(qStrTrim(CToStrRef("  \t Hello world  ")), CToStrRef("Hello world")), true);
    EXPECT_EQ(qStrEqual(qStrTrim(CToStrRef("  ")), CToStrRef("")), true);
    EXPECT_EQ(qStrEqual(qStrTrim(CToStrRef(" ")), CToStrRef("")), true);
    EXPECT_EQ(qStrEqual(qStrTrim(CToStrRef("\n")), CToStrRef("")), true);
    EXPECT_EQ(qStrEqual(qStrTrim(CToStrRef("\t")), CToStrRef("")), true);
}

UTEST(qstr, qsscanf)
{
    {
        uint32_t a = 0;
        uint32_t b = 0;
        int      read = qstrsscanf(CToStrRef("10.132"), "%u.%u", &a, &b);
        EXPECT_EQ(read, 2);
        EXPECT_EQ(a, 10);
        EXPECT_EQ(b, 132);
    }
}

// UTEST(qstr, qSliceToUtf16CodePoint) {
//   unsigned char leftPointingMagnify[] = {0x3D,0xD8,0x0D,0xDD};
//
//   struct tf_utf_result_s res;
//   {
//     struct qStrSplitIterable iter = {
//       .buffer = (struct StrSpan) {
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
//     struct qStrSplitIterable iter = {
//       .buffer = (struct StrSpan) {
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
//     struct qStrSplitIterable iter = (struct qStrSplitIterable){
//       .buffer = (struct StrSpan) {
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
//     struct qStrSplitIterable iter = (struct qStrSplitIterable){
//         .buffer = (struct StrSpan){
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

// UTEST(qstr, qSliceToUtf8CodePoint) {
//   char smilyCat[] = {0xF0, 0x9F, 0x98, 0xBC};
//   struct qStrSplitIterable iter = {
//     .buffer = (struct StrSpan) {
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
//   EXPECT_EQ(tfSliceToUtf8CodePoint((struct StrSpan) {
//     .len = sizeof(charU),
//     .buf = charU
//   }, 0), 'U');
//
//   char ringOperator[] = {0xe2, 0x88, 0x98};
//   EXPECT_EQ(tfSliceToUtf8CodePoint((struct StrSpan) {
//     .len = sizeof(ringOperator),
//     .buf = ringOperator
//   }, 0), 0x2218);
//
//   // this has an extra byte
//   char badRingOperator[] = {0xe2, 0x88, 0x98, 0x1};
//   EXPECT_EQ(tfSliceToUtf8CodePoint((struct StrSpan) {
//     .len = sizeof(badRingOperator),
//     .buf = badRingOperator
//   }, 1), 1);
// }

// UTEST(qstr, qUtf8CodePointIter) {
//   {
//     unsigned char smilyCat[] = {0xF0, 0x9F, 0x98, 0xBC};
//     struct qStrSplitIterable iter = {
//       .buffer = (struct StrSpan) {
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
//   struct qStrSplitIterable iterable = {
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

UTEST_MAIN();

