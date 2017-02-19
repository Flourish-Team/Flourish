include "../../3rdParty/googletest/premake5.lua"

project "Test"
   kind "StaticLib"
   language "C++"
   targetdir "../../Bin/%{cfg.buildcfg}"
   links { "GoogleTest" }
   includedirs { "Include", "../../3rdParty/googletest/include" }

   files 
   { 
      "../FlourishConfig.h",
      "Include/**.h", 
      "Include/**.inl", 
      "Source/**.c", 
      "Source/**.cpp", 
      "Tests/**.cpp" 
   }

   filter {"system:macosx"}
      linkoptions  { "-std=c++11", "-stdlib=libc++" }
      buildoptions { "-std=c++11", "-stdlib=libc++" }