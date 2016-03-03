cmake_minimum_required (VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/Modules CACHE TYPE STRING FORCE)
include(CommonFunctions)

project (yagbe)
include_directories( ${PROJECT_SOURCE_DIR} )


option(BUILD_TESTS "Build tests only" OFF)
option(YAGBE_TRAVIS "On travis" OFF)

#include(conanbuildinfo.cmake)
#conan_basic_setup()


if (MSVC)
add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4005 /wd4996 /nologo" )
endif(MSVC)



add_folder(vm)

if(YAGBE_TRAVIS)
	add_definitions(-DYAGBE_TRAVIS)
endif(YAGBE_TRAVIS)

if(BUILD_TESTS)
    #add_subdirectory(tests)
	add_definitions(-DYAGBE_TESTS)
	add_definitions(-DYAGBE_ROMS="${PROJECT_SOURCE_DIR}/tests/roms/")
	add_executable (yagbe tests/main.cpp ${SRCS})
else(BUILD_TESTS)
	add_folder(renderer)
	
	find_package(SDL2 REQUIRED)
	include_directories(${SDL2_INCLUDE_DIR})
	target_link_libraries(yagbe ${SDL2_LIBRARY})

	add_executable (yagbe main.cpp ${SRCS})
endif(BUILD_TESTS)



target_link_libraries(yagbe ${CONAN_LIBS})
