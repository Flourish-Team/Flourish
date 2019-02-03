project "Core"
   kind "StaticLib"
   language "C++"
   targetdir "../../Bin/%{cfg.buildcfg}"
   systemversion "latest"
   includedirs 
   { 
      "Include", 
      "../../3rdParty/dbghelp/include"  
   }
   
   files 
   { 
      "../FlourishConfig.h",

      -- Add the Premake bat files for easy project rebuilding (Use 
      -- http://stackoverflow.com/questions/5605885/how-to-run-a-bat-from-inside-the-ide to launch from IDE) 
      "Premake/*"  
   }

   includeCommonFiles()
   excludePlatformSepecificFilesIfNeeded()

   -- Link to the correct dbghelp.lib on windows debug
   filter {"system:windows", "configurations:Debug", "architecture:x32"}
      libdirs { "../../3rdParty/dbghelp/lib/x86/" }
      links { "DbgHelp" }

   filter {"system:windows", "configurations:Debug", "architecture:x64"}
      libdirs { "../../3rdParty/dbghelp/lib/x64/" }
      links { "DbgHelp" }

   filter {"system:macosx"}
      linkoptions  { "-std=c++17", "-stdlib=libc++" }
      buildoptions { "-std=c++17", "-stdlib=libc++" }