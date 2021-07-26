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
#include "SC2KRegistry.h"

//Warnings = Red, Prompts = White, Debug = Gray
#define FOREGROUND_GRAY FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define FOREGROUND_WHITE FOREGROUND_GRAY | FOREGROUND_INTENSITY

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
  //printf("SC2K Path = % ls\n", exe_sc2k_path.wstring().c_str());

label_md5:
  std::string hash;
  bool hash_result = Hash::GenerateMD5(exe_path.wstring(), hash);
  if (hash_result)
    printf("md5sum=%s\n", hash.c_str());
  else
    goto label_start;

  printf("\nInstalling SimCity 2000 (WIN95)...\n");

label_install_registry:

  //---------- Localize ----------
  {
    RegistryKey rkey;
    rkey.hKey = HKEY_CURRENT_USER;
    rkey.SubKey = L"Software\\Maxis\\SimCity 2000\\Localize";
    const RegistryEntry rvals_localize[] = { RegistryEntry(L"Language", RegistryValue_SZ(L"USA"))};
    if (!Registry::SetValues(rkey, rvals_localize, 1))
    { //Assume that if it fails on the first registy edit, the others won't
      printf("Try restarting this program as an administrator!\n");
      goto label_start; 
    }
  }

  //---------- Paths ----------
  {

    std::unordered_map<std::wstring, std::wstring> KeyDirectoryMap;
    auto it = SC2KRegistry::RequiredSubDirectories.begin();
    for (; it != SC2KRegistry::RequiredSubDirectories.end(); ++it)
    {
      std::filesystem::path find_path(root_path);
      find_path.append(it->first);
      try
      {
        find_path = std::filesystem::canonical(find_path);
      }
      catch (const std::exception&)
      {
        if (it->second.Required)
        {
          char buffer[256];
          snprintf(buffer, sizeof(buffer),
            "The required path does not exist: %ls\n",
            find_path.wstring().c_str());
          print_warning(hConsole, std::string(buffer));
          goto label_start;
        }
        else
        {
          char buffer[256];
          snprintf(buffer, sizeof(buffer),
            "The optional path does not exist: %ls\nAlthough not required, some game features may not be present.",
            find_path.wstring().c_str());
          print_warning(hConsole, std::string(buffer));
        }
      }
      //Adds presumed paths
      for (const std::wstring& Key : it->second.KeyValues)
      {
        KeyDirectoryMap[Key] = find_path.wstring();
      }
    }

    RegistryKey rkey;
    rkey.hKey = HKEY_CURRENT_USER;
    rkey.SubKey = L"Software\\Maxis\\SimCity 2000\\Paths";

    const RegistryEntry rvalues[] = {      
      RegistryEntry(L"Cities", RegistryValue_SZ(KeyDirectoryMap.at(L"Cities"))),
      RegistryEntry(L"Data", RegistryValue_SZ(KeyDirectoryMap.at(L"Data"))),
      RegistryEntry(L"Goodies", RegistryValue_SZ(KeyDirectoryMap.at(L"Goodies"))),
      RegistryEntry(L"Graphics", RegistryValue_SZ(KeyDirectoryMap.at(L"Graphics"))),
      RegistryEntry(L"Home", RegistryValue_SZ(exe_parent_path.wstring())),
      RegistryEntry(L"Music", RegistryValue_SZ(KeyDirectoryMap.at(L"Music"))),
      RegistryEntry(L"SaveGame", RegistryValue_SZ(KeyDirectoryMap.at(L"SaveGame"))),
      RegistryEntry(L"Scenarios", RegistryValue_SZ(KeyDirectoryMap.at(L"Scenarios"))),
      RegistryEntry(L"TileSets", RegistryValue_SZ(KeyDirectoryMap.at(L"TileSets")))
    };

    if (!Registry::SetValues(rkey, rvalues, 9))
      goto label_start;
    
  }
  
  //---------- Registration ----------
  {
   
    printf("\n");
    std::wstring mayor_name, company_name;

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    wprintf(L"Im SimCity 2000, you will become the fearless leader of many living sims.\nWhat should they call you?\n");
    SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);    
    std::getline(std::wcin, mayor_name);

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf("Please tell me again, %ls, from what fine company do you hail?\n", mayor_name.c_str());
    SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
    std::getline(std::wcin, company_name);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GRAY);
    printf("\n");
  
    RegistryKey rkey;
    rkey.hKey = HKEY_CURRENT_USER;
    rkey.SubKey = L"Software\\Maxis\\SimCity 2000\\Registration";
    RegistryEntry rvalues[] = 
    {
      RegistryEntry(L"Mayor Name", RegistryValue_SZ(mayor_name)),
      RegistryEntry(L"Company Name", RegistryValue_SZ(company_name))
    };
    if (!Registry::SetValues(rkey, rvalues, 2))
      goto label_start;
  }

  SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
  printf("\nFinished! SimCity 2000 (Win95) is now installed and patched.\n");
  SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
  _getch();
}