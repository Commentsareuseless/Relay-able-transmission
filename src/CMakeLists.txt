cmake_minimum_required(VERSION 3.3)

project("RelayableTransmitter")

add_executable(${PROJECT_NAME}
   "main.c"
   "FileHandler.c"
   "FileHandler.h"
   "ClParser.c"
   "ClParser.h"
)
