cmake_minimum_required (VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/Modules CACHE TYPE STRING FORCE)

project (Tests)

#include(conanbuildinfo.cmake)
#conan_basic_setup()


if (MSVC)
add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4005 /wd4996 /nologo" )
endif(MSVC)

include_directories ("${PROJECT_SOURCE_DIR}/../../")

add_executable (Tests main.cpp ${SRCS})
target_link_libraries(Tests ${CONAN_LIBS})
