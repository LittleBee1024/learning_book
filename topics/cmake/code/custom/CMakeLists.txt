######################
# Global Setting
######################

cmake_minimum_required(VERSION 3.13)

# Setting a project name
project(exec VERSION 1.0
   DESCRIPTION "CUSTOM Project"
   LANGUAGES CXX)

# set the C++ standard
set(CMAKE_CXX_STANDARD 20)
# Use STD c++ instead of GNU
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "[ROOT] CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}")
message(STATUS "[ROOT] CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}")
message(STATUS "[ROOT] CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER}")

######################
# Add Targets
######################
add_subdirectory(src)
