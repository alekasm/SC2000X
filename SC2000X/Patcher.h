#pragma once
#include "Instructions.h"
#include <map>

typedef std::string SectionHeaderName;
struct PEDATA
{
  DWORD RealVirtualAddress;
  DWORD VirtualAddress;
  DWORD RawDataPointer;
  DWORD VirtualSize;
};

struct PEINFO
{
  std::map<SectionHeaderName, PEDATA> data_map;
};

class Patcher
{
public:
	static bool Patch(std::string, std::vector<Instructions>);
private:
	static inline DWORD GetFileOffset(DWORD, const PEDATA&);
	static bool GetSectionInfo(const char*, PEINFO&);
	
};
