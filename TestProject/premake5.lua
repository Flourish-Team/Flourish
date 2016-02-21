project "TestProject"
   kind "ConsoleApp"
   language "C++"
   targetdir "../Bin/%{cfg.buildcfg}"
   includedirs { "../Libs/Test/Include", "../3rdParty/googletest/include" }
   links { "Test", "GoogleTest" }

   files { 
      "Source/**.cpp",
      "../Libs/Test/Tests/**.cpp"
   }

   filter "configurations:Debug"
      defines { "DEBUG" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"