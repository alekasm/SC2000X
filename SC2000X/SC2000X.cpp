#include <iostream>
#include <cstdio>
#include <string>
#include <conio.h>
#include <fstream>
#include <windows.h>
#include <winreg.h>
#include <filesystem>

#include "Registry.h"
#include "Hash.h"

int main()
{
  printf("Welcome to SC2000X - An Open-Source Win95 SimCity 2000 Patcher\n");
  printf("Current Version: 0.1\n");
  printf("Aleksander Krimsky - alekasm.com | krimsky.net\n\n");
  printf("Current Features:\n");
  printf("- Performs installation/setup\n");
  printf("- Fixes Save-As crashing\n\n"); 
  
label_check_admin:
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
  printf("WARNING:\n");
  printf("It appears you are running SC2000X without elevated permissions!\n\n");
  printf("If you run into errors, try any of the following:\n");
  printf("1. Right-click -> \"Run-as-Administrator\"\n");
  printf("2. Move the game files to a user directory (ie Desktop)\n");
  printf("3. Ensure the game is not located on read-only/removable storage ie usb/cd/iso/zip\n\n");

label_start:
  printf("Enter your SIMCITY.EXE file location: ");
  std::wstring input;
  std::getline(std::wcin, input);
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
    }
    rval_paths[i] = RegistryValue(REG_SZ, subdir_paths[i], subdir_path);
  }

  if (!Registry::SetValues(rkey_paths, rval_paths, path_size)) goto label_start;

  printf("\nFinished! SimCity 2000 (Win95) is now installed and patched.\n");
  _getch();
}