#!/usr/bin/env ruby

Dir.chdir('Premake') {
  `bin/linux/premake5 make`
}