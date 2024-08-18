// Copyright (c) 2024 Steffen Illhardt,
// Licensed under the MIT license ( https://opensource.org/license/mit/ ).

#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "wtswidth.h"

static inline void help()
{
  static const char msg[] =
    "* 'wtswidth' - Windows Terminal string width *\n"
    "Count the number of columns required to represent a string in the Windows Terminal.\n"
    "The return value is the width as the number of character cells the string occupies.\n"
    "In case the wrong syntax is used, this message is displayed and -1 is returned.\n\n"
    "Usage:\n"
    "wtswidth \"string to measure\"\n\n";

  fwrite(msg, 1, sizeof(msg) - 1, stderr);
}

#if defined(_WIN32) || defined(__CYGWIN__)
// On Windows, the command line is always available as wide string anyway. So we just use that.
#  include <windows.h>
int main(void)
{
  int argc = 0;
  wchar_t **const argv = CommandLineToArgvW(GetCommandLineW(), &argc);
  if (argc != 2)
  {
    LocalFree(argv);
    help();
    return -1;
  }

  const int totalWidth = wts16width((const char16_t *)argv[1], wcslen(argv[1]));
  LocalFree(argv);
  return totalWidth;
}
#else
// On *nix we assume to get UTF-8 as this is virtually standard.
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    help();
    return -1;
  }

  return wts8width(argv[1], strlen(argv[1]));
}
#endif
