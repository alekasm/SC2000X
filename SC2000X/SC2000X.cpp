/*
* Written by Aleksander Krimsky
* www.krimsky.net | www.alekasm.com
* Icon from iconarchive.org, accessed 29 July 2021
* https://iconarchive.com/show/standard-city-icons-by-aha-soft/city-icon.html
*/

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
#include "SC2KVersion.h"
#include "Patcher.h"
#include "AssemblyData.h"

int main()
{
  //Shared between installer and patcher
  std::filesystem::path exe_path;
  std::filesystem::path exe_parent_path;
  std::filesystem::path root_path;

  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  Logger::Initialize(hConsole);
  SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
  printf("Welcome to SC2000X - An Open-Source SimCity 2000(Win95) Patcher\n");
  SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
  printf("Current Version: 0.9 (DEV)\n");
  printf("Aleksander Krimsky - alekasm.com | krimsky.net\n\n");
  SetConsoleTextAttribute(hConsole, FOREGROUND_GRAY);
  printf("Current Features:\n");
  printf("- Performs installation/setup\n\n");
  //printf("- Fixes Save-As crashing\n\n");  

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
  {
    exe_path.clear();
    exe_parent_path.clear();
    root_path.clear();
    std::wstring input = Logger::Prompt(FOREGROUND_WHITE, L"Enter your SIMCITY.EXE file location: ");
    exe_path = std::filesystem::path(input);
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
  }

label_install_prompt:
  {
    std::wstring run_installer = Logger::Prompt(FOREGROUND_WHITE,
      L"Would you like to install the game? (required to run) Y/n: ");
    std::transform(run_installer.begin(), run_installer.end(), run_installer.begin(), ::tolower);
    if (!run_installer.empty() && run_installer.at(0) == L'n') goto label_patcher_prompt;
  }

label_install:
  {
    printf("\nInstalling SimCity 2000(Win95)...\n");
    if (!SC2KRegistry::SetLocalization()) goto label_start;
    if (!SC2KRegistry::SetPaths(root_path, exe_parent_path)) goto label_start;
    if (!SC2KRegistry::SetRegistration()) goto label_start;
    if (!SC2KRegistry::SetOptions()) goto label_start;
    if (!SC2KRegistry::SetSCURK()) goto label_start;
    if (!SC2KRegistry::SetVersion()) goto label_start;
    if (!SC2KRegistry::SetWindows()) goto label_start;
    printf("Finished installing SimCity 2000(Win95)\n");
  }

label_patcher_prompt:
  {
    std::wstring run_patcher = Logger::Prompt(FOREGROUND_WHITE,
      L"Would you like to patch the game? Y/n: ");
    std::transform(run_patcher.begin(), run_patcher.end(), run_patcher.begin(), ::tolower);
    if (!run_patcher.empty() && run_patcher.at(0) == L'n') goto label_finished;
  }

label_patcher:
  {
    std::string hash;
    bool hash_result = Hash::GenerateMD5(exe_path.wstring(), hash);
    if (!hash_result) goto label_start;
    printf("Hash=%s\n", hash.c_str());
    auto vinfo_it = SC2KVersion::VersionInfoMap.find(hash);
    if (vinfo_it == SC2KVersion::VersionInfoMap.end())
    {
      std::string warning = "SC2000X was unable to recognize the game with the hash above!\n";
      warning.append("Either the game is already modified or it does not exist in the ");
      warning.append("SC2000X list of recognized versions. Please submit a copy of your SIMCITY.exe for ");
      warning.append("evaluation.\n");
      Logger::PrintWarning(warning);
      goto label_start;
    }
    VersionInfo vinfo = vinfo_it->second;
    printf("Recognized Version: %s\n", vinfo.description.c_str());
    std::filesystem::path sc2000x_path(exe_parent_path);
    sc2000x_path.append("SC2000X.EXE");
    std::error_code copy_error;
    std::filesystem::copy(exe_path, sc2000x_path,
      std::filesystem::copy_options::overwrite_existing, copy_error);
    if (copy_error.value())
    {
      char buffer[512];
      snprintf(buffer, sizeof(buffer),
        "Failed to copy file from: %ls\nto: %ls\nError Code: %d\n%s\n",
        exe_path.wstring().c_str(),
        sc2000x_path.wstring().c_str(),
        copy_error.value(), 
        copy_error.message().c_str());
      Logger::PrintWarning(buffer);
      goto label_start;
    }
    std::vector<Instructions> instructions;
    AssemblyData::GenerateData(vinfo, instructions);
    //Okay, I got lazy and used std string here, I really don't care right now
    bool patch_result = Patcher::Patch(sc2000x_path.string(), instructions);
    if (!patch_result) goto label_patcher_prompt;
  }

label_finished:
  SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
  printf("\nSC2000X is now finished!\n");
  SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
  _getch();
}