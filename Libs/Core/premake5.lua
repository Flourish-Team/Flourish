project "Core"
   kind "StaticLib"
   language "C++"
   targetdir "../../Bin/%{cfg.buildcfg}"
   includedirs { "Include", "../../3rdParty/debugbreak/" }
   links { "Test", "GoogleTest" }

   files 
   { 
      "Include/**.h", 
      "Include/**.inl", 
      "Source/**.c", 
      "Source/**.cpp",

      -- Add the Premake bat files for easy project rebuilding (Use 
      -- http://stackoverflow.com/questions/5605885/how-to-run-a-bat-from-inside-the-ide to launch from IDE) 
      "Premake/*"  
   }



   configuration "macosx"
            linkoptions  { "-std=c++11", "-stdlib=libc++" }
            buildoptions { "-std=c++11", "-stdlib=libc++" }