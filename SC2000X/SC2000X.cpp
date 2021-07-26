#include <iostream>
#include <cstdio>
#include <string>
#include <conio.h>
#include <fstream>
#include <windows.h>
#include <winreg.h>
#include <filesystem>
#include <wincon.h>

#include "Logger.h"
#include "Registry.h"
#include "Hash.h"
#include "SC2KRegistry.h"

//Warnings = Red, Prompts = White, Debug = Gray


std::filesystem::path GetFilesystemPath(const std::wstring& path)
{
  std::filesystem::path fs_path(path);
  try
  {
    fs_path = std::filesystem::canonical(fs_path);
  }
  catch (const std::exception&)
  {
    printf("Unable to validate that the following path exists: %ls\n", fs_path.c_str());
  }
  return fs_path;
}

int main()
{
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  Logger::Initialize(hConsole);
  SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
  printf("Welcome to SC2000X - An Open-Source Win95 SimCity 2000 Patcher\n");
  SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
  printf("Current Version: 0.1\n");
  printf("Aleksander Krimsky - alekasm.com | krimsky.net\n\n");
  SetConsoleTextAttribute(hConsole, FOREGROUND_GRAY);
  printf("Current Features:\n");
  printf("- Performs installation/setup\n");
  printf("- Fixes Save-As crashing\n\n"); 
 

  HANDLE hToken = NULL;
  if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
  {
    TOKEN_ELEVATION elevation;
    DWORD dwSize;
    if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
    {
      if (elevation.TokenIsElevated)
      {
        goto label_start;
      }
    }
  }

  {
    std::string warning = "It appears you are running SC2000X without elevated permissions!\n\n";
    warning.append("If you run into errors, try any of the following:\n");
    warning.append("1. Right-click -> \"Run-as-Administrator\"\n");
    warning.append("2. Move the game files to a user directory (ie Desktop)\n");
    warning.append("3. Ensure the game is not located on read-only/removable storage ie usb/cd/iso/zip\n");
    Logger::PrintWarning(warning);
  }

label_start:
  SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
  printf("Enter your SIMCITY.EXE file location: ");
  std::wstring input;
  std::getline(std::wcin, input);
  SetConsoleTextAttribute(hConsole, FOREGROUND_GRAY);
  std::filesystem::path exe_path(input);
  std::filesystem::path exe_parent_path;
  std::filesystem::path root_path;

  if (!exe_path.has_extension())
    exe_path.append(L"SimCity.exe");
  try
  {
    exe_path = std::filesystem::canonical(exe_path);
    exe_parent_path = exe_path.parent_path();
    root_path = exe_parent_path.parent_path();
  }
  catch (const std::exception& e)
  {
    printf("%s\n", e.what());
    goto label_start;
  }

  printf("Canonical Path=%ls\n", exe_path.wstring().c_str());
  printf("Parent Path=%ls\n", exe_parent_path.wstring().c_str());
  printf("Root Path=%ls\n", root_path.wstring().c_str());

  std::string hash;
  bool hash_result = Hash::GenerateMD5(exe_path.wstring(), hash);
  if (hash_result)
    printf("md5sum=%s\n", hash.c_str());
  else
    goto label_start;

  printf("\nInstalling SimCity 2000 (WIN95)...\n");

  if (!SC2KRegistry::SetLocalization()) goto label_start;
  if (!SC2KRegistry::SetPaths(root_path, exe_parent_path)) goto label_start;
  if (!SC2KRegistry::SetRegistration()) goto label_start;

  SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
  printf("\nFinished! SimCity 2000 (Win95) is now installed and patched.\n");
  SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
  _getch();
}