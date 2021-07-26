#pragma once
#include <unordered_map>
#include <string>
#include <filesystem>
#include "Logger.h"

struct PathRegistryKeyValue
{
  bool Required;
  std::vector<std::wstring> KeyValues;
  PathRegistryKeyValue(const std::vector<std::wstring>& KeyValues, bool Required)
  {
    this->KeyValues = KeyValues;
    this->Required = Required;
  }
  PathRegistryKeyValue() = delete;
};

namespace SC2KRegistry
{
  static const std::unordered_map<std::wstring, PathRegistryKeyValue> RequiredSubDirectories =
  {
    {L"SC2K/CITIES", PathRegistryKeyValue({L"Cities", L"SaveGame"}, true)},
    {L"SC2K/DATA", PathRegistryKeyValue({L"Data"}, true)},
    {L"GOODIES", PathRegistryKeyValue({L"Goodies"}, false)},
    {L"SC2K/BITMAPS", PathRegistryKeyValue({L"Graphics"}, true)},
    {L"SC2K/SOUNDS", PathRegistryKeyValue({L"Music"}, true)},
    {L"SC2K/SCENARIO", PathRegistryKeyValue({L"Scenarios"}, true)},
    {L"SC2K/SCURKART", PathRegistryKeyValue({L"TileSets"}, true)}
  };

  extern bool SetLocalization();
  extern bool SetPaths(
    const std::filesystem::path& root_path,
    const std::filesystem::path& exe_parent_path);
  extern bool SetRegistration();
}