#include "SC2KRegistry.h"
#include "Registry.h"
#include <iostream>

bool SC2KRegistry::SetLocalization()
{
  RegistryKey rkey;
  rkey.hKey = HKEY_CURRENT_USER;
  rkey.SubKey = L"Software\\Maxis\\SimCity 2000\\Localize";
  const RegistryEntry rvals_localize[] = { RegistryEntry(L"Language", RegistryValue(L"USA")) };
  return Registry::SetValues(rkey, rvals_localize, 1);
}

bool SC2KRegistry::SetPaths(
  const std::filesystem::path& root_path,
  const std::filesystem::path& exe_parent_path)
{
  std::unordered_map<std::wstring, std::wstring> KeyDirectoryMap;
  KeyDirectoryMap[L"Home"] = exe_parent_path.wstring();
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
        Logger::PrintWarning(std::string(buffer));
        return false;
      }
      else
      {
        char buffer[256];
        snprintf(buffer, sizeof(buffer),
          "The optional path does not exist: %ls\nAlthough not required, some game features may not be present.",
          find_path.wstring().c_str());
        Logger::PrintWarning(std::string(buffer));
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

  //Do not use stack-allocated values for RegistryValues, they will be pointer casted.
  const RegistryEntry rvalues[]{
    RegistryEntry(L"Cities", RegistryValue(KeyDirectoryMap.at(L"Cities"))),
    RegistryEntry(L"Data", RegistryValue(KeyDirectoryMap.at(L"Data"))),
    RegistryEntry(L"Goodies", RegistryValue(KeyDirectoryMap.at(L"Goodies"))),
    RegistryEntry(L"Graphics", RegistryValue(KeyDirectoryMap.at(L"Graphics"))),
    RegistryEntry(L"Home", RegistryValue(KeyDirectoryMap.at(L"Home"))),
    RegistryEntry(L"Music", RegistryValue(KeyDirectoryMap.at(L"Music"))),
    RegistryEntry(L"SaveGame", RegistryValue(KeyDirectoryMap.at(L"SaveGame"))),
    RegistryEntry(L"Scenarios", RegistryValue(KeyDirectoryMap.at(L"Scenarios"))),
    RegistryEntry(L"TileSets", RegistryValue(KeyDirectoryMap.at(L"TileSets")))
  };

  return Registry::SetValues(rkey, rvalues, 9);
}

bool SC2KRegistry::SetRegistration()
{
  printf("\n");
  std::wstring mayor_name, company_name;

  SetConsoleTextAttribute(Logger::GetConsole(), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
  wprintf(L"Im SimCity 2000, you will become the fearless leader of many living Sims.\nWhat should they call you? ");
  SetConsoleTextAttribute(Logger::GetConsole(), FOREGROUND_WHITE);
  std::getline(std::wcin, mayor_name);

  SetConsoleTextAttribute(Logger::GetConsole(), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
  printf("Please tell me again, %ls, from what fine company do you hail? ", mayor_name.c_str());
  SetConsoleTextAttribute(Logger::GetConsole(), FOREGROUND_WHITE);
  std::getline(std::wcin, company_name);
  SetConsoleTextAttribute(Logger::GetConsole(), FOREGROUND_GRAY);
  printf("\n");

  RegistryKey rkey;
  rkey.hKey = HKEY_CURRENT_USER;
  rkey.SubKey = L"Software\\Maxis\\SimCity 2000\\Registration";
  RegistryEntry rvalues[] =
  {
    RegistryEntry(L"Mayor Name", RegistryValue(mayor_name)),
    RegistryEntry(L"Company Name", RegistryValue(company_name))
  };
  return Registry::SetValues(rkey, rvalues, 2);
}