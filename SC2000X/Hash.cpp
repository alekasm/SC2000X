#include "Hash.h"
#include <Windows.h>
//f1ad828513d75004345e3324b3d97e46
bool Hash::GenerateMD5(const std::string& filename, std::string& hash)
{
  std::wstring filename_wstring = std::wstring(filename.begin(), filename.end());
  return GenerateMD5(filename_wstring, hash);
}

bool Hash::GenerateMD5(const std::wstring& filename_wstring, std::string& hash)
{
	LPCWSTR filename = filename_wstring.c_str();

	DWORD cbHash = 16;
	HCRYPTHASH hHash = 0;
	HCRYPTPROV hProv = 0;
	BYTE rgbHash[16];
	CHAR rgbDigits[] = "0123456789abcdef";
	HANDLE hFile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Hash::GenerateMD5 has failed because CreateFileW has an invalid handle.\n");
		return false;
	}

	CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);

	BOOL bResult = FALSE;
	DWORD BUFSIZE = 4096;
	BYTE rgbFile[4096];
	DWORD cbRead = 0;
	while (bResult = ReadFile(hFile, rgbFile, BUFSIZE, &cbRead, NULL))
	{
		if (0 == cbRead)
			break;
		CryptHashData(hHash, rgbFile, cbRead, 0);
	}

	std::string md5_hash = "";
	if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
	{
		for (DWORD i = 0; i < cbHash; i++)
		{
			char buffer[3]; //buffer needs terminating null
			sprintf_s(buffer, 3, "%c%c", rgbDigits[rgbHash[i] >> 4], rgbDigits[rgbHash[i] & 0xf]);
			md5_hash.append(buffer);
		}
		hash = md5_hash;
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		CloseHandle(hFile);
		return true;
	}
	else
	{
		CloseHandle(hFile);
		printf("Hash::GenerateMD5 has failed because CryptGetHashParam returned false.\n");
		return false;
	}
}

bool Hash::ValidateMD5(const std::string& hash)
{
	return false;
}