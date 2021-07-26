#pragma once
#include <windows.h>
#include <string>
#include <minwindef.h>


struct RegistryValue {
  virtual DWORD GetType() const = 0;
  LPBYTE Data;
  DWORD Size;
};

struct RegistryValue_SZ : RegistryValue {
  RegistryValue_SZ(const std::wstring& value)
  {
    Data = (LPBYTE)value.c_str();
    Size = (value.size() * sizeof(wchar_t)) + 1;
  }
  DWORD GetType() const
  {
    return REG_SZ;
  }
};

struct RegistryValue_DWORD : RegistryValue {
  RegistryValue_DWORD(const DWORD& value)
  {
    Data = (LPBYTE)value;
    Size = sizeof(DWORD);
  }
  DWORD GetType() const
  {
    return REG_DWORD;
  }
};

struct RegistryEntry {
  std::wstring Name;  
  const RegistryValue* Value;
  RegistryEntry(std::wstring Name, const RegistryValue& Value)
  {
    this->Name = Name;
    this->Value = &Value;
  }
  RegistryEntry() { Value = nullptr; Name = L""; }
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
  BOOL SetValues(const RegistryKey, const RegistryEntry[], size_t);
}