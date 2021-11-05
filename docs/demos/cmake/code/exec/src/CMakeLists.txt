######################
# Target Setting
######################
message(STATUS "[INFO] CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "[INFO] CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(EXEC main.cpp)

# https://cmake.org/cmake/help/v3.22/manual/cmake-properties.7.html#target-properties
# https://caiorss.github.io/C-Cpp-Notes/compiler-flags-options.html
set_target_properties(EXEC PROPERTIES
   COMPILE_OPTIONS "-Wall;-Werror;-Wpedantic;-I."
   LINK_OPTIONS "-Wl,-Map=output.map,--print-memory-usage;-L."
   RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
