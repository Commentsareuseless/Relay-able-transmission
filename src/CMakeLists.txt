cmake_minimum_required(VERSION 3.3)

project("RelayableTransmitter")

add_executable(${PROJECT_NAME}
   "main.c"
   "FileHandler.c"
   "FileHandler.h"
   "ClParser.c"
   "ClParser.h"
   "NetHandler.h"
   "NetHandler.c"
)

if (CMAKE_COMPILER_IS_GNUCC)
   target_compile_options(${PROJECT_NAME} PRIVATE
      "-Wall"
      "-Wextra"
      "-Wpedantic"
      "-Wshadow"
   )
else()
   target_compile_options(${PROJECT_NAME} PRIVATE
      "-Wall"
   )
endif()
