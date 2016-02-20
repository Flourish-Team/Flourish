-- premake5.lua
workspace "Flourish"
   configurations { "Debug", "Release" }
   location("../Projects/" .. _ACTION)

-- include "../Libs/Core/premak5.lua"
include "../Libs/Test/premak5.lua"