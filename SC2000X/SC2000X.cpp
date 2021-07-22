#include <iostream>
#include <cstdio>
#include <string>
#include <conio.h>
#include <fstream>
#include <windows.h>
#include <winreg.h>
#include <filesystem>
#include <wincon.h>


#include "Registry.h"
#include "Hash.h"

//Warnings = Red, Prompts = White, Debug = Gray
#define FOREGROUND_GRAY FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define FOREGROUND_WHITE FOREGROUND_GRAY | FOREGROUND_INTENSITY

void print_warning(HANDLE hConsole, const std::string warning)
{
  SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
  printf("\nWARNING:\n%s\n", warning.c_str());
  SetConsoleTextAttribute(hConsole, FOREGROUND_GRAY);
}

int main()
{
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
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
    print_warning(hConsole, warning);
  }

label_start:
  SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
  printf("Enter your SIMCITY.EXE file location: ");
  std::wstring input;
  std::getline(std::wcin, input);
  SetConsoleTextAttribute(hConsole, FOREGROUND_GRAY);
  std::filesystem::path exe_path(input);
  std::filesystem::path exe_parent_path;
  std::filesystem::path exe_sc2k_path;
  if (!exe_path.has_extension())
    exe_path.append(L"SimCity.exe");
  try
  {
    exe_path = std::filesystem::canonical(exe_path);
    exe_parent_path = exe_path.parent_path();
    exe_sc2k_path = std::filesystem::path(exe_parent_path);
    exe_sc2k_path.append(L"SC2K");
    exe_sc2k_path = std::filesystem::canonical(exe_sc2k_path);
  }
  catch (const std::exception& e)
  {
    printf("%s\n", e.what());
    goto label_start;
  }

  printf("Canonical Path=%ls\n", exe_path.wstring().c_str());
  printf("Parent Path=%ls\n", exe_parent_path.wstring().c_str());
  printf("SC2K Path = % ls\n", exe_sc2k_path.wstring().c_str());

label_md5:
  std::string hash;
  bool hash_result = Hash::GenerateMD5(exe_path.wstring(), hash);
  if (hash_result)
    printf("md5sum=%s\n", hash.c_str());
  else
    goto label_start;

  printf("\nInstalling SimCity 2000 (WIN95)...\n");

label_install_reg_localize:
  RegistryKey rkey_localize;
  rkey_localize.hKey = HKEY_CURRENT_USER;
  rkey_localize.SubKey = L"Software\\Maxis\\SimCity 2000\\Localize";

  RegistryValue rval_localize_language;
  rval_localize_language.dwType = REG_SZ;
  rval_localize_language.ValueName = L"Language";
  rval_localize_language.Data = L"USA";

  const RegistryValue rvals_localize[] = { rval_localize_language };
  if (!Registry::SetValues(rkey_localize, rvals_localize, 1))
  {
    printf("Try restarting this program as an administrator!\n");
    goto label_start;
  }

label_install_reg_paths:
  RegistryKey rkey_paths;
  rkey_paths.hKey = HKEY_CURRENT_USER;
  rkey_paths.SubKey = L"Software\\Maxis\\SimCity 2000\\Paths";  
 
  const size_t path_size = 9;
  RegistryValue rval_paths[path_size];
  const std::wstring subdir_paths[path_size] = {
    L"Cities", L"Data", L"Goodies", L"Graphics", L"Home",
    L"Music", L"SaveGame", L"Scenarios", L"TileSets" };
  bool missing_filepath = false;
  for (size_t i = 0; i < path_size; ++i)
  {
    std::filesystem::path subdir_path(exe_sc2k_path);
    subdir_path.append(subdir_paths[i]);
    try
    {
      subdir_path = std::filesystem::canonical(subdir_path);
    }
    catch (const std::exception& e)
    {
      printf("Directory isn't valid, still adding as a path: %ls\n",
        subdir_path.c_str());
      missing_filepath = true;
    }
    rval_paths[i] = RegistryValue(REG_SZ, subdir_paths[i], subdir_path);
  }

  if (!Registry::SetValues(rkey_paths, rval_paths, path_size))
    goto label_start;

  if (missing_filepath)
  {
    std::string warning = "You were missing a directory listed above! The installation and patch may\n";
    warning.append("be successful, but the game may fail to run or have miscellaneous issues.\n");
    print_warning(hConsole, warning);
  } 

  SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
  printf("\nFinished! SimCity 2000 (Win95) is now installed and patched.\n");
  SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
  _getch();
}