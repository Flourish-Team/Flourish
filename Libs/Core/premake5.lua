project "Core"
   kind "StaticLib"
   language "C++"
   targetdir "../../Bin/%{cfg.buildcfg}"
   includedirs { "Include", "../../3rdParty/dbghelp/include"  }
   links { "Test", "GoogleTest" }

   files 
   { 
      "../FlourishConfig.h",
      "Include/**.h", 
      "Include/**.inl", 
      "Source/**.c", 
      "Source/**.cpp",

      -- Add the Premake bat files for easy project rebuilding (Use 
      -- http://stackoverflow.com/questions/5605885/how-to-run-a-bat-from-inside-the-ide to launch from IDE) 
      "Premake/*"  
   }

   -- Link to the correct dbghelp.lib on windows debug
   filter {"system:windows", "configurations:Debug", "architecture:x32"}
      links { "../../3rdParty/dbghelp/lib/x86/dbghelp" }

   filter {"system:windows", "configurations:Debug", "architecture:x64"}
      links { "../../3rdParty/dbghelp/lib/x64/dbghelp" }

   filter {"system:macosx"}
      linkoptions  { "-std=c++11", "-stdlib=libc++" }
      buildoptions { "-std=c++11", "-stdlib=libc++" }