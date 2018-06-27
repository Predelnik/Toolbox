cmake_minimum_required(VERSION 3.5)
project (toolbox_test)

file (GLOB_RECURSE source_files *.cpp *.h *.hpp)


include_directories (../src)
include_directories (.)

set (CMAKE_CXX_STANDARD 17)
add_executable (toolbox_test ${source_files})
if (MSVC)
target_compile_options (toolbox_test PRIVATE /std:c++latest /permissive-)
endif ()
