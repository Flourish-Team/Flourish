include "../../3rdParty/googletest/premake5.lua"

project "Test"
   kind "StaticLib"
   language "C++"
   targetdir "../../Bin/%{cfg.buildcfg}"
   systemversion "latest"
   includedirs 
   { 
      "Include", 
      "../../3rdParty/googletest/include" 
   }

   files 
   { 
      "Tests/**.cpp" 
   }

   includeCommonFiles()
   excludePlatformSepecificFilesIfNeeded()

   filter {"system:macosx"}
      linkoptions  { "-std=c++11", "-stdlib=libc++" }
      buildoptions { "-std=c++11", "-stdlib=libc++" }