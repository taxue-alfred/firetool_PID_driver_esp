# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/ESP-IDF/esp-idf/components/bootloader/subproject"
  "D:/Projects/PersonalProject/firetool_PID_driver_esp/cmake-build-debug/bootloader"
  "D:/Projects/PersonalProject/firetool_PID_driver_esp/cmake-build-debug/bootloader-prefix"
  "D:/Projects/PersonalProject/firetool_PID_driver_esp/cmake-build-debug/bootloader-prefix/tmp"
  "D:/Projects/PersonalProject/firetool_PID_driver_esp/cmake-build-debug/bootloader-prefix/src/bootloader-stamp"
  "D:/Projects/PersonalProject/firetool_PID_driver_esp/cmake-build-debug/bootloader-prefix/src"
  "D:/Projects/PersonalProject/firetool_PID_driver_esp/cmake-build-debug/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Projects/PersonalProject/firetool_PID_driver_esp/cmake-build-debug/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Projects/PersonalProject/firetool_PID_driver_esp/cmake-build-debug/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
