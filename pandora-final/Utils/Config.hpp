#pragma once
#include <Windows.h>
#include <vector>
#include <string>

namespace ConfigManager
{
   BOOL DirectoryExists( LPCTSTR szPath );
   std::vector<std::string> GetConfigs( );
   void LoadConfig( std::string configname, bool load_user_data = false );
   void SaveConfig( std::string configname, bool load_user_data = false );
   void RemoveConfig( std::string configname );
   void CreateConfig( std::string configname );
   void ResetConfig( );
   void OpenConfigFolder( );
   void ExportToClipboard( );
   void ImportFromClipboard( );
}
