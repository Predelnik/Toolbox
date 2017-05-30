cmake_minimum_required(VERSION 3.5)
project (toolbox CXX)

file (GLOB_RECURSE source_files *.cpp *.h *.hpp)

set (CMAKE_CXX_STANDARD 14)
add_library (toolbox ${source_files})
set_target_properties(toolbox PROPERTIES LINKER_LANGUAGE CXX)

