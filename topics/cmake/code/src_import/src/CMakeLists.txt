######################
# Target Setting
######################

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src1 SRCS1)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src2 SRCS2)

message(STATUS "[SRC] CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "[SRC] CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "[SRC] CMAKE_CURRENT_LIST_DIR ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "[SRC] SRCS1 ${SRCS1}")
message(STATUS "[SRC] SRCS2 ${SRCS2}")

add_executable(SRC_IMPORT_EXEC ${SRCS1} ${SRCS2} main.cpp)

# The option should not have any white space, use semicolon to split different options
separate_arguments(COMPILE_OPTION UNIX_COMMAND "-Wall -Werror -Wpedantic -I.")
separate_arguments(LINK_OPTION UNIX_COMMAND "-Wl,-Map=output.map,--print-memory-usage -L.")

# https://cmake.org/cmake/help/v3.22/manual/cmake-properties.7.html#target-properties
# https://caiorss.github.io/C-Cpp-Notes/compiler-flags-options.html
set_target_properties(SRC_IMPORT_EXEC PROPERTIES
   COMPILE_OPTIONS "${COMPILE_OPTION}"
   LINK_OPTIONS "${LINK_OPTION}"
   RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
