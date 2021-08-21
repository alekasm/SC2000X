#pragma once
#include <string>
#include <unordered_map>
//df136923ca204e3451ed4349f08b5d1f (my version, August 14, 1995 - 1.23MB) - SVERSION.INF matches this one
//f1ad828513d75004345e3324b3d97e46 (archive.org version, March 6, 1996 - 1.32MB)

enum VersionType { VERSION_AUG1995, VERSION_MAR1996};

struct FunctionOffset
{
  DWORD DIALOG_CONTROLLER;
  DWORD DIALOG_DISPLAY_CALC;
};

struct GameOffsets {
  FunctionOffset functions;
};

const struct VersionAug1995Offsets : GameOffsets
{
  VersionAug1995Offsets()
  {
    functions.DIALOG_CONTROLLER = 0x49EE88;
    functions.DIALOG_DISPLAY_CALC = 0x4A2AF1;
  }
} version_aug1995offsets;

const struct VersionMar1996Offsets : GameOffsets
{
  VersionMar1996Offsets()
  {
    functions.DIALOG_CONTROLLER = 0x4A04EF;
    functions.DIALOG_DISPLAY_CALC = 0x4A3BFD;
  }
} version_mar1996offsets;


struct VersionInfo {
  const std::string description;
  const VersionType version;
  const GameOffsets* offsets;
  VersionInfo() = delete;
  VersionInfo(VersionType version, const GameOffsets* offsets, std::string description) :
    version(version), description(description), offsets(offsets)
  {
  }
};

namespace SC2KVersion
{
  const std::unordered_map<std::string, VersionInfo> VersionInfoMap = {
    {"df136923ca204e3451ed4349f08b5d1f", VersionInfo(VERSION_AUG1995, &version_aug1995offsets, "August 14, 1995 (1.23MB)")},
    {"f1ad828513d75004345e3324b3d97e46", VersionInfo(VERSION_MAR1996, &version_mar1996offsets, "March 6, 1996 (1.32MB)")}
  };
}