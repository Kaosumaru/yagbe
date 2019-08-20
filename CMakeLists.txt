cmake_minimum_required (VERSION 2.8)


project (yagbe)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/Modules CACHE TYPE STRING FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
include(CommonFunctions)

include_directories( ${PROJECT_SOURCE_DIR} )
include_directories( "${PROJECT_SOURCE_DIR}/include" )

option(BUILD_TESTS "Build tests only" OFF)

if (MSVC)
add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4005 /wd4996 /nologo /permissive-" )
endif(MSVC)

add_folder(vm)
add_folder(vm/gpu)
add_folder(vm/apu)
add_folder(vm/rom)
add_folder(serializer)

if(BUILD_TESTS)
	add_definitions(-DYAGBE_ROMS="${PROJECT_SOURCE_DIR}/tests/roms/")
	add_definitions(-DYAGBE_TESTS)
	add_executable (yagbe tests/main.cpp ${SRCS})
else(BUILD_TESTS)
	add_folder(renderer)
	
	add_executable (yagbe main.cpp ${SRCS})

	if (EMSCRIPTEN)
		message("Building for emscripten ${CMAKE_CURRENT_BINARY_DIR}/bin ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

		set(CMAKE_EXECUTABLE_SUFFIX ".html")
		add_definitions("-std=c++1z")

		target_compile_options(yagbe PUBLIC "-s" "USE_SDL=2" --use-preload-plugins)
		target_link_libraries(yagbe PUBLIC "-s USE_SDL=2" --use-preload-plugins)
		target_link_libraries(yagbe PUBLIC "--preload-file ../tests/roms/")

		add_definitions(-DYAGBE_ROMS="/tests/roms/")
	else(EMSCRIPTEN)
		message("Building for native")

		find_package(SDL2 REQUIRED)
		include_directories(${SDL2_INCLUDE_DIR})
		target_link_libraries(yagbe ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY})
		file(COPY tests/roms/ DESTINATION bin/roms/)
		add_definitions(-DYAGBE_ROMS="roms/")
	endif(EMSCRIPTEN)
	
	
endif(BUILD_TESTS)
