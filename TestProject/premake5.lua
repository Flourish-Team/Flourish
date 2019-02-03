project "TestProject"
   kind "ConsoleApp"
   language "C++"
   targetdir "../Bin/%{cfg.buildcfg}"
   systemversion "latest"
   includedirs 
   { 
      "../Libs/Core/Include",
      "../Libs/Core/Tests",
      "../Libs/Test/Include",
      "../3rdParty/googletest/include" 
   }

   defines { "FL_RUNNING_TESTS" }

   links { "Core", "Test", "GoogleTest" }

   files { 
      "Source/**.cpp",
      "../Libs/**/Tests/**.cpp",
      "../Libs/**/Tests/**.h",
      "../Libs/**/Tests/**.inl"
   }

   excludePlatformSepecificFilesIfNeeded()
   
   filter {"system:macosx"}
      linkoptions  { "-std=c++17", "-stdlib=libc++" }
      buildoptions { "-std=c++17", "-stdlib=libc++" }