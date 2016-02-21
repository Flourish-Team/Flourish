-- premake5.lua
workspace "Flourish"
   configurations { "Debug", "Release" }
   location("../Projects/" .. _ACTION)
   flags { "Unicode" }
   -- Stripping symbols from OSX in release causes an error in LD
   if os.get() == "macosx" then
    flags { "Symbols" }
   end

-- include "../Libs/Core"
include "../Libs/Test"

-- Include applications that need all the libs last
include "../TestProject"