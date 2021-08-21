#include "Patcher.h"

const DWORD WIN32_PE_ENTRY = 0x400000;
bool Patcher::Patch(std::string fname, std::vector<Instructions> ins)
{
  PEINFO info;
  if (!GetSectionInfo(fname.c_str(), info)) return false;
  auto module_it = info.data_map.find(".text");
  if (module_it == info.data_map.end())
  {
    printf("The .text section does not appear to be loaded!\n");
    return false;
  }
  FILE* efile;
  int result = fopen_s(&efile, fname.c_str(), "rb+");
  if (efile == nullptr)
  {
    printf("Failed to open: %s, Error code: %d", fname.c_str(), result);
    return false;
  }
  size_t bytes_written = 0;
  for (Instructions is : ins)
  {
    for (Instruction in : is.GetInstructions())
    {
      DWORD address = GetFileOffset(in.address, module_it->second);
      fseek(efile, address, SEEK_SET);
      fprintf(efile, "%c", in.byte);
      ++bytes_written;
    }
  }
  printf("Total bytes patches: %u\n", bytes_written);
  fclose(efile);
  return true;
}

DWORD Patcher::GetFileOffset(DWORD address, const PEDATA& data)
{
  return (address - data.RealVirtualAddress) + data.RawDataPointer;
}

bool Patcher::GetSectionInfo(const char* filepath, PEINFO& info)
{
  HANDLE file = CreateFile(filepath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file == INVALID_HANDLE_VALUE)
  {
    printf("Invalid handle to %s, GetLastError: %lu\n", filepath, GetLastError());
    return false;
  }

  DWORD fileSize = GetFileSize(file, NULL);
  BYTE* pByte = new BYTE[fileSize];
  DWORD dw;
  if (!ReadFile(file, pByte, fileSize, &dw, NULL))
  {
    printf("ReadFile failed on: %s, GetLastError: %lu\n", filepath, GetLastError());
    return false;
  }

  PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pByte;
  if (dos->e_magic != IMAGE_DOS_SIGNATURE)
  {
    printf("Error with retrieving IMAGE_DOS_SIGNATURE (MZ check failed)\n");
    return false;
  }

  PIMAGE_FILE_HEADER FH = (PIMAGE_FILE_HEADER)(pByte + dos->e_lfanew + sizeof(DWORD));
  PIMAGE_OPTIONAL_HEADER OH = (PIMAGE_OPTIONAL_HEADER)(pByte + dos->e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER));
  PIMAGE_SECTION_HEADER SH = (PIMAGE_SECTION_HEADER)(pByte + dos->e_lfanew + sizeof(IMAGE_NT_HEADERS));
  for (WORD i = 0; i < FH->NumberOfSections; ++i)
  {
    std::string name(reinterpret_cast<char const*>(SH[i].Name));
    info.data_map[name].VirtualAddress = SH[i].VirtualAddress;
    info.data_map[name].RealVirtualAddress = SH[i].VirtualAddress + WIN32_PE_ENTRY;
    info.data_map[name].RawDataPointer = SH[i].PointerToRawData;
    info.data_map[name].VirtualSize = SH[i].Misc.VirtualSize;
    printf("Loaded section: %s\n", name.c_str());
  }  
  return CloseHandle(file);
}