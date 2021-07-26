#include "Logger.h"
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

HANDLE Logger::GetConsole()
{
  return hConsole;
}