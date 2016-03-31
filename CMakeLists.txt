cmake_minimum_required (VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/Modules CACHE TYPE STRING FORCE)
include(CommonFunctions)

project (yagbe)
include_directories( ${PROJECT_SOURCE_DIR} )
include_directories( "${PROJECT_SOURCE_DIR}/include" )

option(BUILD_TESTS "Build tests only" OFF)
option(YAGBE_TRAVIS "On travis" OFF)

#include(conanbuildinfo.cmake)
#conan_basic_setup()


if (MSVC)
add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4005 /wd4996 /nologo" )
endif(MSVC)



add_folder(vm)
add_folder(vm/gpu)
add_folder(vm/rom)
add_folder(serializer)

if(YAGBE_TRAVIS)
	add_definitions(-DYAGBE_TRAVIS)
endif(YAGBE_TRAVIS)


	message("TEST")
if(BUILD_TESTS)
    #add_subdirectory(tests)
	add_definitions(-DYAGBE_ROMS="${PROJECT_SOURCE_DIR}/tests/roms/")
	add_definitions(-DYAGBE_TESTS)
	add_executable (yagbe tests/main.cpp ${SRCS})
else(BUILD_TESTS)
	add_folder(renderer)
	
	add_executable (yagbe main.cpp ${SRCS})

	if (EMSCRIPTEN)
		message("Building for emscripten")

		set(CMAKE_EXECUTABLE_SUFFIX ".html")
		add_definitions("-std=c++14")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 --use-preload-plugins")
		set_target_properties(yagbe PROPERTIES LINK_FLAGS "-s USE_SDL=2 --use-preload-plugins --preload-file ../tests/roms/")
		add_definitions(-DYAGBE_ROMS="/tests/roms/")
	else(EMSCRIPTEN)
		message("Building for native")

		add_definitions(-DYAGBE_ROMS="${PROJECT_SOURCE_DIR}/tests/roms/")
		
		#include(FindPkgConfig)
		#pkg_search_module(SDL2 REQUIRED sdl2)
		#include_directories(${SDL2_INCLUDE_DIRS} )
		#target_link_libraries(yagbe ${SDL2_LIBRARIES} )

		find_package(SDL2 REQUIRED)
		include_directories(${SDL2_INCLUDE_DIR})
		target_link_libraries(yagbe ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY})		
		
	endif(EMSCRIPTEN)	
	
	
endif(BUILD_TESTS)
