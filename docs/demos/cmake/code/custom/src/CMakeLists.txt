######################
# Target Setting
######################
message(STATUS "[SRC] CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "[SRC] CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "[SRC] CMAKE_CURRENT_LIST_DIR ${CMAKE_CURRENT_LIST_DIR}")

set(CODEGEN_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/internal")
set(CODEGEN_SRC "${CODEGEN_BINARY_DIR}/animal.cpp")
message(STATUS "[SRC] CODEGEN_SRC ${CODEGEN_SRC}")
message(STATUS "[SRC] CODEGEN_BINARY_DIR ${CODEGEN_BINARY_DIR}")

add_executable(main main.cpp ${CODEGEN_SRC})

# Cannot use add_dependencies here and use add_custom_target in internal folder
# because this target depends on the output of internal folder, and have to set output
# in add_custom_command. Otherwise, cmake config fails because of no source file.
add_custom_command(
   # Copy all files in this folder to $(CODEGEN_BINARY_DIR)
   COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_CURRENT_SOURCE_DIR}/internal ${CODEGEN_BINARY_DIR}
   COMMAND make -C ${CODEGEN_BINARY_DIR}
   OUTPUT ${CODEGEN_SRC}
   COMMENT "Generate C++ code with cog python tool"
   VERBATIM
)

# https://cmake.org/cmake/help/v3.22/manual/cmake-properties.7.html#target-properties
# https://caiorss.github.io/C-Cpp-Notes/compiler-flags-options.html
set_target_properties(main PROPERTIES
   COMPILE_OPTIONS "-I${CODEGEN_BINARY_DIR}"
   LINK_OPTIONS ""
   RUNTIME_OUTPUT_DIRECTORY ""
)
