#pragma once
#include "Instructions.h"
#include "SC2KVersion.h"

//Currently does NOT use a "DetourMaster" since there is no current need
//to extend the Windows PE. This may change in the future.
class AssemblyData {
public:
  static void GenerateData(const VersionInfo&, std::vector<Instructions>&);
  private:
    static void PatchDialogCrashing(const VersionInfo&, std::vector<Instructions>&);
};