cmake_minimum_required(VERSION 3.10)
project(SQLParserLib)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 14)

# Manually specify the paths to the SQLite3 include directory and library
set(SQLite3_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/SQLParser/sqlite3/include")
set(SQLite3_LIBRARIES "${CMAKE_SOURCE_DIR}/SQLParser/sqlite3/lib/sqlite3.lib")



# Ensure the include directory for SQLite3 is set

include_directories(include)

# Define the library
add_library(SQLParserLib SHARED
    src/AST.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/Visitor.cpp
    src/SQLParserAPI.cpp
    src/SQLCompletionEngine.cpp
)

# Set compile definitions
target_compile_definitions(SQLParserLib PRIVATE SQLPARSERLIB_EXPORTS)

# Include directories for the target
target_include_directories(SQLParserLib PUBLIC include ${SQLite3_INCLUDE_DIRS})

# Link the SQLite3 library to the target
target_link_libraries(SQLParserLib ${SQLite3_LIBRARIES})

# Set the runtime output directory for the DLL
set_target_properties(SQLParserLib PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
