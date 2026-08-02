# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\sqlite_visual_editor_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\sqlite_visual_editor_autogen.dir\\ParseCache.txt"
  "sqlite_visual_editor_autogen"
  )
endif()
