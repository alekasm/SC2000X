#include "AssemblyData.h"

//Only one function is being patched currently
void AssemblyData::GenerateData(const VersionInfo& vinfo, std::vector<Instructions>& ins)
{
  AssemblyData::PatchDialogCrashing(vinfo, ins);
}

//The issue stems from calling DIALOG_DISPLAY_CALC. If this returns 0, the programmers
//just simply assumed calling GetWindowLong + GetParent would return the correct result
//and fail to null check it. This happens in both DIALOG_CONTROLLER and in "DialogFunc".
//The GetWindowLong + GetParent "backup" never returns a desired result...
void AssemblyData::PatchDialogCrashing(const VersionInfo& vinfo, std::vector<Instructions>& ins)
{
  DWORD function_entry = vinfo.offsets->functions.DIALOG_CONTROLLER;
  Instructions instructions(function_entry + 0xA);
  //Bypasses 2nd arg (UINT) comparison against 0x30/0x110
  //This prevents calling DialogFunc which apparently isn't needed to show Save/Load Tile Set
  instructions << ByteArray{ 0x75, 0x20 }; //jnz short 0x20 instead of 0x7 bytes

  instructions.relocate(function_entry + 0x6A);
  //DIALOG_DISPLAY_CALC does useful things, but there is never a point where
  //It fails to make a calculation (0 return), and then GetWindowLong+GetParent
  //fixes the issue. If we reach this point, just return.
  instructions << ByteArray{ 0xEB, 0xEB }; //jmp to xor eax, eax block
  ins.push_back(instructions);
  printf("Generated Dialog Crashing Patch\n");
}