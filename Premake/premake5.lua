-- premake5.lua
workspace "Flourish"
   configurations { "Debug", "Release" }
   location("../Projects/" .. _ACTION)

-- include "../Libs/Core"
include "../Libs/Test"

-- Include applications that need all the libs last
include "../TestProject"