cmake_minimum_required(VERSION 3.10)
project(MyProject)
# Define a common output directory for executables and DLLs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(Core)
add_subdirectory(SQLParser)

