######################
# Target Setting
######################
message(STATUS "[TEST] CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "[TEST] CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "[TEST] CMAKE_CURRENT_LIST_DIR ${CMAKE_CURRENT_LIST_DIR}")

add_executable(myTest main.cpp)

target_link_libraries(myTest PRIVATE
   mylib
)

# https://cmake.org/cmake/help/v3.22/manual/cmake-properties.7.html#target-properties
# https://caiorss.github.io/C-Cpp-Notes/compiler-flags-options.html
set_target_properties(myTest PROPERTIES
   COMPILE_OPTIONS ""
   LINK_OPTIONS ""
   RUNTIME_OUTPUT_DIRECTORY ""
)
