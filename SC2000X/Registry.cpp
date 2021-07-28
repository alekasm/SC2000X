#include "Registry.h"
#include <winreg.h>

BOOL Registry::SetValues(const RegistryKey key, const RegistryEntry values[], size_t size)
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
    const RegistryEntry& entry = values[index];
    const size_t v_size = entry.Value->Size;

    DWORD queryType;
    WCHAR queryData[256] = { 0 };
    DWORD cbData = sizeof(queryData) - 1;
    LSTATUS status_queryvalue = RegQueryValueExW(hKey,
      entry.Name.c_str(), NULL, &queryType, 
      (LPBYTE)queryData, &cbData);

    bool overwrite = false;
    if (status_queryvalue == ERROR_SUCCESS)
    {      
      std::wstring data_wstring(queryData);
      if (data_wstring.compare(entry.Value->wstring.c_str()) == 0)
      {
        printf("Existing %ls=%ls\n", entry.Name.c_str(),
          data_wstring.c_str());
        continue;
      }
      overwrite = true;
    }  
    
    printf("%s %ls=%ls\n", overwrite ? "Overwriting" : "Setting",
      entry.Name.c_str(), entry.Value->wstring.c_str());

    LSTATUS status_setvalue = RegSetValueExW(hKey,
        entry.Name.c_str(), NULL,
        entry.Value->dwType,
        entry.Value->Data,
        entry.Value->Size);    

    if (status_setvalue != ERROR_SUCCESS)
    {
      printf("Unable to set the registry key. LSTATUS=%d\n", status_setvalue);
      return FALSE;
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