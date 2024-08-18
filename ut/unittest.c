#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "../wtswidth.h"

// Some explanations about what certain strings contain. First column is UTF-8 bytes in HEX.
//
// 6 code points in हिन्दी:
// E0A4B9 | U+0939 | Devanagari Letter Ha
// E0A4BF | U+093F | Devanagari Vowel Sign I
// E0A4A8 | U+0928 | Devanagari Letter Na
// E0A58D | U+094D | Devanagari Sign Virama
// E0A4A6 | U+0926 | Devanagari Letter Da
// E0A580 | U+0940 | Devanagari Vowel Sign Ii
//
// 5 code points in 🙋🏻‍♂️:
// F09F998B | U+1F64B | Happy Person Raising One Hand
// F09F8FBB | U+1F3FB | Emoji Modifier Fitzpatrick Type-1-2 (a.k.a. Light Skin Tone)
// E2808D   | U+200D  | Zero Width Joiner (ZWJ)
// E29982   | U+2642  | Male Sign
// EFB88F   | U+FE0F  | Variation Selector-16
//
// 2 code points in ❤️:
// E29DA4 | U+2764 | Heavy Black Heart
// EFB88F | U+FE0F | Variation Selector-16
//
#define S1 "espa\u00F1ol" //                                                 "español" (Spanish) in Latin
#define S2 "\u0441\u043B\u0430\u0432\u044F\u043D\u0441\u043A\u0438\u0439" // "славянский" (Slavic) in Cyrillic
#define S3 "\u5B98\u8BDD" //                                                 "官话" (Mandarin) in simpl. Chinese
#define S4 "\u0939\u093F\u0928\u094D\u0926\u0940" //                         "हिन्दी" (Hindi) in Devanagari
#define S5 "\U0001F648\U0001F604" //                                         "🙈😄" Emojis simple
#define S6 "\U0001F64B\U0001F3FB\u200D\u2642\uFE0F" //                       "🙋🏻‍♂️" Emoji with skin and gender information
#define S7 "\u2705" //                                                       "✅" Dingbat simple
#define S8 "\u2764\uFE0F" //                                                 "❤️" Dingbat with variation

// Determine the length (number of code units) of a string of char16_t. Terminating null not counted.
// Function wcslen() only works on Windows for this purpose as the size of a wchar_t is bigger than 16 bits everywhere else.
static inline size_t c16slen(const char16_t *const str)
{
  const char16_t *it = str;
  while (*it)
    ++it;

  return (size_t)(it - str);
}

// Test all 4 functions declared in wtswidth.h.
// The same string is passed to both parameters u8str and u16str each, once UTF-8 encoded and once UTF-16 encoded, respectively.
static inline int unittest(const char *const u8str, const char16_t *const u16str)
{
  // ~~~ UTF-8 ~~~
  const size_t u8len = strlen(u8str);
  // wts8clusterlen()
  size_t rem8len = u8len;
  int u8sum = 0;
  for (const char *it8 = u8str; *it8;)
  {
    int width;
    const int len = wts8clusterlen(it8, rem8len, &width);
    it8 += len;
    u8sum += width;
    rem8len -= (size_t)len;
  }
  // wts8width()
  const int u8width = wts8width(u8str, u8len);

  // ~~~ UTF-16 ~~~
  const size_t u16len = c16slen(u16str);
  // wts16clusterlen()
  size_t rem16len = u16len;
  int u16sum = 0;
  for (const char16_t *it16 = u16str; *it16;)
  {
    int width;
    const int len = wts16clusterlen(it16, rem16len, &width);
    it16 += len;
    u16sum += width;
    rem16len -= (size_t)len;
  }
  // wts16width()
  const int u16width = wts16width(u16str, u16len);

  // ~~~ result ~~~
  // check whether all calculated display widths are equal
  const int ret = (u8sum == u8width && u8sum == u16sum && u8sum == u16width) ? u8sum : -1;
  // visual check of how the calculated width matches the displayed width in the terminal
  printf("|%s|\n"
         " 1234567890\n"
         "width: %d\n\n",
         u8str,
         ret);

  return ret;
}

#if defined(_WIN32) || defined(__CYGWIN__)
#  include <windows.h>
#  define TERM_OUT_UTF8                                   \
    const UINT oldConsoleOutputCp = GetConsoleOutputCP(); \
    SetConsoleOutputCP(CP_UTF8) // make the output device render UTF-8 strings
#  define TERM_OUT_RESET SetConsoleOutputCP(oldConsoleOutputCp)
#else
#  define TERM_OUT_UTF8 // NOP, assuming that terminals expect UTF-8 on UNIX - like platforms anyway
#  define TERM_OUT_RESET
#endif

// concatenate a string literal with a prefix to generate UTF-8 or UTF-16 encoded literals
#define MAKELITERAL(prefix_, str_) prefix_##str_
#define UNITTEST(str_) unittest((const char *)MAKELITERAL(u8, str_), MAKELITERAL(u, str_))

int main(void)
{
  TERM_OUT_UTF8;

  const int ret = (UNITTEST(S1) == -1 ||
                   UNITTEST(S2) == -1 ||
                   UNITTEST(S3) == -1 ||
                   UNITTEST(S4) == -1 ||
                   UNITTEST(S5) == -1 ||
                   UNITTEST(S6) == -1 ||
                   UNITTEST(S7) == -1 ||
                   UNITTEST(S8) == -1) ?
                    1 :
                    0;

  puts(ret == 1 ? "*** latest test failed ***" : "*** tests passed ***");

  TERM_OUT_RESET;

  return ret;
}
