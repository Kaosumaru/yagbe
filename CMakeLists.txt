cmake_minimum_required (VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/Modules CACHE TYPE STRING FORCE)
include(CommonFunctions)

project (yagbe)
include_directories( ${PROJECT_SOURCE_DIR} )
include_directories( "${PROJECT_SOURCE_DIR}/include" )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

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
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 --use-preload-plugins --profiling")
		set_target_properties(yagbe PROPERTIES LINK_FLAGS "-s USE_SDL=2 --use-preload-plugins --profiling --preload-file ../tests/roms/")
		add_definitions(-DYAGBE_ROMS="/tests/roms/")
	else(EMSCRIPTEN)
		message("Building for native")

		add_definitions(-DYAGBE_ROMS="${PROJECT_SOURCE_DIR}/tests/roms/")

		find_package(SDL2 REQUIRED)
		include_directories(${SDL2_INCLUDE_DIR})
		target_link_libraries(yagbe ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY})

	endif(EMSCRIPTEN)

	set(EM_BINARY_NAME "yagbe")
	set(EM_GAPI_CLIENT_ID "213817666972-ogjsvovt26iofsb0ie7d4pq2ofp9kt92.apps.googleusercontent.com")
	set(EM_GAPI_SCOPES "https://www.googleapis.com/auth/drive.appdata")

	add_subdirectory (include/EmTools)
	target_link_libraries(yagbe LINK_PUBLIC EmTools)


endif(BUILD_TESTS)
