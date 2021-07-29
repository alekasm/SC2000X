#pragma once
#include <string>
#include <unordered_map>
//df136923ca204e3451ed4349f08b5d1f (my version, August 14, 1995 - 1.23MB) - SVERSION.INF matches this one
//f1ad828513d75004345e3324b3d97e46 (archive.org version, March 6, 1996 - 1.32MB)

enum VersionType { VERSION_AUG1995, VERSION_MAR1996};
struct VersionInfo {
  const std::string description;
  const VersionType version;
  VersionInfo() = delete;
  VersionInfo(VersionType version, std::string description) :
    version(version), description(description)
  {

  }
};
namespace SC2KVersion
{
  const std::unordered_map<std::string, VersionInfo> VersionInfoMap = {
    {"df136923ca204e3451ed4349f08b5d1f", VersionInfo(VERSION_AUG1995, "August 14, 1995 (1.23MB)")},
    {"f1ad828513d75004345e3324b3d97e46", VersionInfo(VERSION_MAR1996, "March 6, 1996 (1.32MB)")}
  };
}