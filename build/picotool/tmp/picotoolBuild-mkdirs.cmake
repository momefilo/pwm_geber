# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/momefilo/pico/projekte/geany/pwm_geber/build/_deps/picotool-src"
  "/home/momefilo/pico/projekte/geany/pwm_geber/build/_deps/picotool-build"
  "/home/momefilo/pico/projekte/geany/pwm_geber/build/_deps"
  "/home/momefilo/pico/projekte/geany/pwm_geber/build/picotool/tmp"
  "/home/momefilo/pico/projekte/geany/pwm_geber/build/picotool/src/picotoolBuild-stamp"
  "/home/momefilo/pico/projekte/geany/pwm_geber/build/picotool/src"
  "/home/momefilo/pico/projekte/geany/pwm_geber/build/picotool/src/picotoolBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/momefilo/pico/projekte/geany/pwm_geber/build/picotool/src/picotoolBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/momefilo/pico/projekte/geany/pwm_geber/build/picotool/src/picotoolBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
