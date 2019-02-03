--includes common files by extension in standard libary setup
function includeCommonFiles()
   files 
   { 
      "Include/**.h", 
      "Include/**.inl", 
      "Source/**.c", 
      "Source/**.cpp",
   }
end

--exclude platform specific files
--For cmake ExcludeFromBuild doesnt work, so we remove it instead
function excludePlatformSepecificFilesIfNeeded()
   if _ACTION == "cmake" then
      filter {"system:windows" }
         removefiles { "**_OSX.*", "**_UNIX.*"} 

      filter {"system:macosx" }
         removefiles { "**_WIN.*", "**_LINUX.*"} 

      filter {"system:linux" }
         removefiles { "**_WIN.*", "**_OSX.*"} 
   else
      filter {"system:windows", "files:**_OSX.* or files:**_UNIX.*"}
         flags {"ExcludeFromBuild"}

      filter {"system:macosx", "files:**_WIN.* or files:**_LINUX.*"}
         flags {"ExcludeFromBuild"}

      filter {"system:linux", "files:**_WIN.* or files:**_OSX.*"}
         flags {"ExcludeFromBuild"}
   end 

   filter { }
end