include "../../3rdParty/googletest/premake5.lua"

project "Test"
   kind "StaticLib"
   language "C++"
   targetdir "../../Bin/%{cfg.buildcfg}"
   systemversion "latest"
   includedirs 
   { 
      "Include", 
      "../Core/Include",
      "../../3rdParty/googletest/include" 
   }

   files 
   { 
      "Tests/**.cpp" 
   }

   includeCommonFiles()
   excludePlatformSepecificFilesIfNeeded()


   links { "Core" }

   filter {"system:macosx"}
      linkoptions  { "-std=c++17", "-stdlib=libc++" }
      buildoptions { "-std=c++17", "-stdlib=libc++" }