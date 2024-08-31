// Copyright (c) 2024 Steffen Illhardt,
// Licensed under the MIT license ( https://opensource.org/license/mit/ ).

#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "wtswidth.h"

static inline void help()
{
  static const char msg[] =
    "* 'wtswidth' - Windows Terminal string width, version 1.1. *\n"
    "Count the number of columns required to represent strings in the Windows Terminal.\n"
    "Write the widths as list of numbers of character cells the passed strings occupy.\n"
    "In case the wrong syntax is used, this message is displayed and 1 is returned.\n\n"
    "Usage:\n"
    "wtswidth s1 [s2 [... sN]]\n"
    "  s1..sN  Strings to be measured.\n\n";

  fwrite(msg, 1, sizeof(msg) - 1, stderr);
}

#if defined(_WIN32) || defined(__CYGWIN__)
// On Windows, the command line is always available as wide string anyway. So we just use that.
#  include <windows.h>
int main(void)
{
  int argc = 0;
  wchar_t **const argv = CommandLineToArgvW(GetCommandLineW(), &argc);
  if (argc < 2)
  {
    LocalFree(argv);
    help();
    return 1;
  }

  for (int i = 1; i < argc; ++i)
    printf_s("%d\n", wts16width((const char16_t *)argv[i], wcslen(argv[i])));

  LocalFree(argv);
  return 0;
}
#else
// On *nix we assume to get UTF-8 as this is virtually standard.
int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    help();
    return 1;
  }

  for (int i = 1; i < argc; ++i)
    printf("%d\n", wts8width(argv[i], strlen(argv[i])));

  return 0;
}
#endif
