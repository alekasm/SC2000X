#include "Logger.h"
#include <iostream>
HANDLE Logger::hConsole = NULL;

void Logger::Initialize(HANDLE handle)
{
  hConsole = handle;
}

void Logger::PrintWarning(const std::string& warning)
{
  SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
  printf("\nWARNING:\n%s\n", warning.c_str());
  SetConsoleTextAttribute(hConsole, FOREGROUND_GRAY);
}

std::wstring Logger::Prompt(WORD wAttributes, const wchar_t* string)
{
  std::wstring input;
  SetConsoleTextAttribute(hConsole, wAttributes);
  printf("%ls", string);
  SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
  std::getline(std::wcin, input);
  SetConsoleTextAttribute(hConsole, FOREGROUND_GRAY);
  return input;
}

HANDLE Logger::GetConsole()
{
  return hConsole;
}