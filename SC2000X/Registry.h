#pragma once
#include <windows.h>
#include <string>

struct RegistryValue {
  DWORD dwType;
  std::wstring ValueName;  
  std::wstring Data;
  RegistryValue(DWORD dwType, std::wstring ValueName, std::wstring Data)
  {
    this->dwType = dwType;
    this->ValueName = ValueName;
    this->Data = Data;
  }
  RegistryValue() {}
};

struct RegistryKey {
  HKEY hKey;
  std::wstring SubKey;
  RegistryKey(HKEY hKey, std::wstring SubKey)
  {
    this->hKey = hKey;
    this->SubKey = SubKey;
  }
  RegistryKey() {}
};

namespace Registry {
  BOOL SetValues(const RegistryKey, const RegistryValue[], size_t);
}