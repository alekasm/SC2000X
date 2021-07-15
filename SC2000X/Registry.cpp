#include "Registry.h"
#include <winreg.h>

BOOL Registry::SetValues(const RegistryKey key, const RegistryValue values[], size_t size)
{
  HKEY hKey;
  DWORD disposition;
  LSTATUS status_createkeyex = RegCreateKeyExW(key.hKey, key.SubKey.c_str(),
    0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &disposition);
  if (status_createkeyex != ERROR_SUCCESS)
  {
    printf("[Registry::SetValues] RegCreateKeyExW (%ls) = %d\n", 
      key.SubKey.c_str(), status_createkeyex);
    return FALSE;
  }
  printf("Current Key=%ls\n", key.SubKey.c_str());

  for (size_t index = 0; index < size; ++index)
  {
    const RegistryValue registryValue = values[index];

    DWORD queryType;
    WCHAR queryData[256] = { 0 };
    DWORD cbData = sizeof(queryData) - 1;
    LSTATUS status_queryvalue = RegQueryValueExW(hKey,
      registryValue.ValueName.c_str(), NULL, &queryType, 
      (LPBYTE)queryData, &cbData);
    if (status_queryvalue == ERROR_SUCCESS)
    {
      std::wstring data_wstring(queryData);
      printf("(Existing) %ls=%ls\n", registryValue.ValueName.c_str(), data_wstring.c_str());
    }    
  }
  LSTATUS status_closekey = RegCloseKey(hKey);
  if (status_closekey != ERROR_SUCCESS)
  {
    printf("[Registry::SetValues] RegCloseKey (%ls) = %d\n",
      key.SubKey.c_str(), status_closekey);
    //If we set all our values, still return TRUE
  }
  return TRUE;
}