cmake_minimum_required (VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/Modules CACHE TYPE STRING FORCE)
include(CommonFunctions)

project (yagbe)

#include(conanbuildinfo.cmake)
#conan_basic_setup()


if (MSVC)
add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4005 /wd4996 /nologo" )
endif(MSVC)


add_folder(vm)

add_executable (yagbe main.cpp ${SRCS})
target_link_libraries(yagbe ${CONAN_LIBS})
