#pragma once
#include <string>

struct Hash
{
  static bool GenerateMD5(const std::string&, std::string&);
  static bool GenerateMD5(const std::wstring&, std::string&);
  static bool ValidateMD5(const std::string&);
};