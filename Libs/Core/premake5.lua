project "Core"
   kind "StaticLib"
   language "C++"
   targetdir "../../Bin/%{cfg.buildcfg}"
   includedirs { "Include" }
   links { "Test", "GoogleTest" }

   files 
   { 
      "Include/**.h", 
      "Include/**.inl", 
      "Source/**.c", 
      "Source/**.cpp",
      "Tests/**.cpp",

      -- Add the Premake bat files for easy project rebuilding (Use 
      -- http://stackoverflow.com/questions/5605885/how-to-run-a-bat-from-inside-the-ide to launch from IDE) 
      "Premake/*"  
   }

   filter "configurations:Debug"
      defines { "DEBUG" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"