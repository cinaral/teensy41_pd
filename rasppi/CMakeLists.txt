cmake_minimum_required(VERSION 3.13)

project(
	udp_handler-teensy-pd_control
	VERSION 1.0.0 
	LANGUAGES CXX
)

#* dependencies
Include(FetchContent)

set(cxxopts_VERSION 3.0.0) #* cxxopts, used for CLI argument parsing
set(udp_msg_VERSION 1.0.3) #* udp_msg, used for sending and receiving UDP messages

FetchContent_Declare(
	cxxopts 
	URL https://github.com/jarro2783/cxxopts/archive/refs/tags/v${cxxopts_VERSION}.zip 
)
FetchContent_MakeAvailable(cxxopts)

FetchContent_Declare(
	udp_msg 
	URL https://github.com/cinaral/udp_msg/releases/download/v${udp_msg_VERSION}/src.zip
)
FetchContent_MakeAvailable(udp_msg)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
endif()



if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	option(SINGLE_PRECISION "Single precision" OFF)
	option(BUILD_TESTS "Build tests" ON)
	option(BUILD_EXAMPLES "Build examples" OFF)
	option(BUILD_BENCHMARKS "Build bencmarks" OFF)

	#* where to look for the project header and source files
	set(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(TEST_DIR ${CMAKE_CURRENT_LIST_DIR}/test)
	set(EXAMPLE_DIR ${CMAKE_CURRENT_LIST_DIR}/examples)
	set(BENCHMARK_DIR ${CMAKE_CURRENT_LIST_DIR}/benchmarks)

	#* source files
	set(SRC_FILES
		main
		send_udp
	)



	#* set up output directories
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dat)

	#* project compile options
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)

	add_compile_options(
		-fdiagnostics-color=always #* colored debug output
		-fmessage-length=0 #* disable line wrapping (default=72 char) 
		-Wall #* all warnings
		-Wextra #* more warnings
		-Wpedantic #* more warnings
	)

	if(SINGLE_PRECISION)
		add_compile_options(-DUSE_SINGLE_PRECISION) #* use single precision floats
	endif()

	#***************#
	#* Executables *#
	#***************#
	foreach(ELEMENT ${SRC_FILES})
		add_executable(
			${ELEMENT}	
			${SRC_DIR}/${ELEMENT}.cpp
		)
		target_include_directories(
			${ELEMENT} PRIVATE 
			${INCLUDE_DIR}
			${CMAKE_CURRENT_LIST_DIR}/../shared/include
			${udp_msg_SOURCE_DIR}/include	
		)
		target_compile_options(
			${ELEMENT} PRIVATE
		)
	endforeach(ELEMENT ${SRC_FILES})

	target_include_directories(
		send_udp PRIVATE 
		${cxxopts_SOURCE_DIR}/include
	)
	
endif()
