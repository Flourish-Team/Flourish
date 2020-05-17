project "UnitTestRunner"
   kind "ConsoleApp"
   language "C++"
   targetdir "../Bin/%{cfg.buildcfg}"
   systemversion "latest"
   includedirs 
   { 
      "Include", 
      "../../Libs/Core/Include",
      "../../Libs/Core/Tests",
      "../../Libs/UnitTests/Include",
      "../../3rdParty/googletest/include" 
   }

   defines { "FL_RUNNING_TESTS" }

   links { "Core", "GoogleTest" }

   files { 
      "Include/**.h", 
      "Source/**.cpp",
      "../../Libs/**/Tests/**.cpp",
      "../../Libs/**/Tests/**.h",
      "../../Libs/**/Tests/**.inl"
   }

   excludePlatformSepecificFilesIfNeeded()
   
   filter {"system:macosx"}
      linkoptions  { "-std=c++17", "-stdlib=libc++" }
      buildoptions { "-std=c++17", "-stdlib=libc++" }