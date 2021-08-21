#pragma once
#include <Windows.h>
#include <cstdio>
#include <map>
#include <string>

typedef DWORD(CALLBACK* GetFileVersionInfoSizeExA2)(DWORD, LPCSTR, LPDWORD);
typedef DWORD(CALLBACK* GetFileVersionInfoExA2)(DWORD, LPCSTR, DWORD, DWORD, LPVOID);
typedef DWORD(CALLBACK* VerQueryValueA2)(LPCVOID, LPCSTR, LPVOID*, PUINT);

struct FileVersion
{
  static bool PrintFileVersionInfo(LPCSTR filename)
  {
    //This bullshit of not having the static libraries has been going on for too long
    HINSTANCE hInst = LoadLibrary("Version.dll");
    if (hInst == NULL) return false;
    FARPROC _GetFileVersionInfoSizeExA2Address = GetProcAddress(hInst, "GetFileVersionInfoSizeExA");
    FARPROC _GetFileVersionInfoExA2Address = GetProcAddress(hInst, "GetFileVersionInfoExA");
    FARPROC _VerQueryValueA2Address = GetProcAddress(hInst, "VerQueryValueA");
    if (_GetFileVersionInfoSizeExA2Address == NULL) return false;
    if (_GetFileVersionInfoExA2Address == NULL) return false;
    if (_VerQueryValueA2Address == NULL) return false;
    GetFileVersionInfoSizeExA2 GetFileVersionInfoSizeExA_ = (GetFileVersionInfoSizeExA2)_GetFileVersionInfoSizeExA2Address;
    GetFileVersionInfoExA2 GetFileVersionInfoExA_ = (GetFileVersionInfoExA2)_GetFileVersionInfoExA2Address;
    VerQueryValueA2 VerQueryValueA_ = (VerQueryValueA2)_VerQueryValueA2Address;

    DWORD flags = FILE_VER_GET_NEUTRAL | FILE_VER_GET_LOCALISED;
    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSizeExA_(flags, filename, &handle);
    if (size == 0)
    {
      printf("GetFileVersionInfoSizeExA returned a size of 0\n");
      return false;
    }
    LPVOID pVersionInfo = new BYTE[size];
    if (!GetFileVersionInfoExA_(flags, filename, NULL, size, pVersionInfo))
    {
      printf("GetFileVersionInfoExA returned false\n");
      return false;
    }

    UINT puLen2;
    struct LANGANDCODEPAGE {
      WORD wLanguage;
      WORD wCodePage;
    } *lpTranslate;

    if (!VerQueryValueA_(pVersionInfo, "\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &puLen2))
    {
      printf("VerQueryValueA returned false\n");
      return false;
    }

    const std::string descriptions[4] = {
      "FileVersion", "CompanyName", "FileDescription", "OriginalFilename"
    };

    for (size_t i = 0; i < 4; ++i)
    {
      UINT puLen;
      char buffer[256];
      sprintf_s(buffer, sizeof(buffer), 
        "\\StringFileInfo\\%04lx%04lx\\%s",
        lpTranslate->wLanguage,
        lpTranslate->wCodePage,
        descriptions[i].c_str());
      LPCSTR lpBuffer;
      if (!VerQueryValueA_(pVersionInfo, buffer, (LPVOID*)&lpBuffer, &puLen))
      {
        printf("VerQueryValueA returned false\n");
        return false;
      }
      printf("%s: %s\n", descriptions[i].c_str(), lpBuffer);
    }
    return true;
  }
};