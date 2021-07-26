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
    const RegistryEntry& registryEntry = values[index];    
    wchar_t w_buffer[256];
    swprintf(w_buffer, sizeof(w_buffer), L"%ls", registryEntry.Value->Data);   

    DWORD queryType;
    WCHAR queryData[256] = { 0 };
    DWORD cbData = sizeof(queryData) - 1;
    LSTATUS status_queryvalue = RegQueryValueExW(hKey,
      registryEntry.Name.c_str(), NULL, &queryType, 
      (LPBYTE)queryData, &cbData);

    bool overwrite = false;

    if (status_queryvalue == ERROR_SUCCESS)
    {      
      std::wstring data_wstring(queryData);      
      if (data_wstring.compare(w_buffer) == 0)
      {
        printf("Existing %ls=%ls\n", registryEntry.Name.c_str(),
          data_wstring.c_str());
        continue;
      }
      overwrite = true;
    }  
    
    LSTATUS status_setvalue = RegSetValueExW(hKey,
      registryEntry.Name.c_str(), NULL,
      registryEntry.Value->GetType(),
      registryEntry.Value->Data,
      registryEntry.Value->Size);

    printf("%s %ls=", overwrite ? "Overwriting" : "Setting",
      registryEntry.Name.c_str());
    printf("%ls\n", registryEntry.Value->Data);
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