#pragma once
#include <windows.h>
#include <string>
#include <minwindef.h>


struct RegistryValue {
  const DWORD dwType;
  const DWORD Size;
  LPBYTE Data; 

  explicit RegistryValue(const std::wstring& value) : 
    dwType(REG_SZ), Size((value.size() + 1) * sizeof(wchar_t))
  {
    Data = (LPBYTE) malloc(Size);
    if (Data == nullptr) throw std::bad_alloc();
    memcpy(Data, value.c_str(), Size);
  }

  explicit RegistryValue(const DWORD& value) :
    dwType(REG_DWORD), Size(sizeof(DWORD))
  {
    Data = (LPBYTE) malloc(Size);    
    if (Data == nullptr) throw std::bad_alloc();
    memcpy(Data, &value, Size);
  }

  RegistryValue(const RegistryValue& v) : 
    dwType(v.dwType), Size(v.Size)    
  {
    Data = (LPBYTE)malloc(Size);
    if (Data == nullptr) throw std::bad_alloc();
    memcpy(Data, v.Data, Size);
  }

  ~RegistryValue()
  {
    free(Data);
    Data = nullptr;
  }

};

struct RegistryEntry {
  const std::wstring Name;  
  const RegistryValue* Value;
  RegistryEntry(std::wstring Name, const RegistryValue& Value) :
    Name(Name), Value(new RegistryValue(Value))
  {
  }
  RegistryEntry() = delete;
  ~RegistryEntry()
  {
    delete Value;
    Value = nullptr;
  }
};

struct RegistryKey {
  HKEY hKey;
  std::wstring SubKey;
  RegistryKey(HKEY hKey, std::wstring SubKey)
  {
    this->hKey = hKey;
    this->SubKey = SubKey;
  }
  RegistryKey() = default;  
};

namespace Registry {
  BOOL SetValues(const RegistryKey, const RegistryEntry[], size_t);
}