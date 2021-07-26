#pragma once
#include <Windows.h>
#include <string>

#define FOREGROUND_GRAY FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define FOREGROUND_WHITE FOREGROUND_GRAY | FOREGROUND_INTENSITY

class Logger
{
public:
  static void Initialize(HANDLE);
  static void PrintWarning(const std::string&);
  static HANDLE GetConsole();
private:
  static HANDLE hConsole;
};