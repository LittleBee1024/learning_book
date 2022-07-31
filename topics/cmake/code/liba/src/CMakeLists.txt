######################
# Target Setting
######################
message(STATUS "[SRC] CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "[SRC] CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "[SRC] CMAKE_CURRENT_LIST_DIR ${CMAKE_CURRENT_LIST_DIR}")

file(GLOB_RECURSE SRC_CPP ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
# Set STATIC to build so library
add_library(mylib STATIC ${SRC_CPP})
# Set public so that its dependency can get the include directory
target_include_directories(mylib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# https://cmake.org/cmake/help/v3.22/manual/cmake-properties.7.html#target-properties
# https://caiorss.github.io/C-Cpp-Notes/compiler-flags-options.html
set_target_properties(mylib PROPERTIES
   COMPILE_OPTIONS ""
   LINK_OPTIONS ""
   RUNTIME_OUTPUT_DIRECTORY ""
)
