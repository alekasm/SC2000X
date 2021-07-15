#include <iostream>
#include <cstdio>
#include <string>
#include <conio.h>
#include <fstream>
#include <windows.h>
#include <winreg.h>

#include "Registry.h"
#include "Hash.h"

int main()
{
  printf("Welcome to SC2000X - An Open-Source Win95 SimCity 2000 Patcher\n");
  printf("Current Version: 0.0\n");
  printf("Aleksander Krimsky - alekasm.com | krimsky.net\n\n");
  printf("Current Features:\n");
  printf("- Performs installation/setup\n");
  printf("- Fixes Save-As crashing\n\n"); 

label_start:
  printf("Enter your SIMCITY.EXE file location (relative or absolute path): ");
  std::wstring input;
  std::getline(std::wcin, input);
  if (!std::wifstream(input).good())
  {
    printf("Could not open %ls\n", input.c_str());
    goto label_start;
  }

label_md5:
  std::string hash;
  bool hash_result = Hash::GenerateMD5(input, hash);
  if (hash_result)
    printf("md5sum=%s\n", hash.c_str());
  else
    goto label_start;

  RegistryKey rkey_localize;
  rkey_localize.hKey = HKEY_CURRENT_USER;
  rkey_localize.SubKey = L"Software\\Maxis\\SimCity 2000\\Localize";

  RegistryValue rval_localize_language;
  rval_localize_language.dwType = REG_SZ;
  rval_localize_language.ValueName = L"Language";
  rval_localize_language.Data = L"USA";

  const RegistryValue rvals_localize[] = { rval_localize_language };
  Registry::SetValues(rkey_localize, rvals_localize, 1);


  RegistryKey rkey_paths;
  rkey_paths.hKey = HKEY_CURRENT_USER;
  rkey_paths.SubKey = L"Software\\Maxis\\SimCity 2000\\Paths";

  const RegistryValue rval_paths[] = {
    RegistryValue(REG_SZ, L"Cities", L""),
    RegistryValue(REG_SZ, L"Data", L""),
    RegistryValue(REG_SZ, L"Goodies", L""),
    RegistryValue(REG_SZ, L"Graphics", L""),
    RegistryValue(REG_SZ, L"Home", L""),
    RegistryValue(REG_SZ, L"Music", L""),
    RegistryValue(REG_SZ, L"SaveGame", L""),
    RegistryValue(REG_SZ, L"Scenarios", L""),
    RegistryValue(REG_SZ, L"TileSets", L""),
  };
  Registry::SetValues(rkey_paths, rval_paths, 9);

  _getch();
}