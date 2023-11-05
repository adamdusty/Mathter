cmake_minimum_required(VERSION 3.27.0)

## Mathter CMake
project(
	Mathter
        VERSION 0.1.0 # necessary for install
	LANGUAGES CXX
)

# Options
option(ENABLE_LLVM_COV "Adds compiler flags to generate LLVM source-based code coverage. Only works with Clang." OFF)
option(MATHTER_USE_XSIMD "Uses XSimd for vectorization of math routines. Uses scalar fallback if turned off." ON)
set(MATHTER_INIT_MODE "DEFAULT" CACHE STRING "Set default initialization of Mathter types.")
set_property(CACHE MATHTER_INIT_MODE PROPERTY STRINGS DEFAULT NULL INVALID UNINITIALIZED)

include(cmake/warning-guard.cmake)

add_library(mathter_mathter INTERFACE)
add_library(mathter::mathter ALIAS mathter_mathter)

set_target_properties(mathter_mathter
	PROPERTIES
		EXPORT_NAME mathter
)

target_sources(mathter_mathter
	INTERFACE
		include/Mathter/Mathter.natvis
)

target_include_directories(
    mathter_mathter ${warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

if (${MATHTER_USE_XSIMD})
	find_package(xsimd)
	target_link_libraries(Mathter INTERFACE xsimd)
endif()

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# Necessary compiler flags
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
	target_compile_options(mathter_mathter INTERFACE -msse2)
	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2 -lpthread")
	if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	target_compile_options(mathter_mathter INTERFACE -Wa -mbig-obj)
	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
	endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
	target_compile_options(mathter_mathter INTERFACE /MP)
	# add_compile_options("/MP")
	# set(RELWITHDEBINFO_OPTIONS "/Ob2")
	# set(RELWITHDEBINFO_REMOVE "/Ob1")
	# foreach(option ${RELWITHDEBINFO_REMOVE})
	# 	string(REPLACE ${option} "" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
	# endforeach()
	# add_compile_options("$<$<CONFIG:RELWITHDEBINFO>:${RELEASE_OPTIONS}>")
endif()

# Optional compiler flags
if (MATHTER_INIT_MODE STREQUAL "NULL")
	target_compile_definitions(mathter_mathter PRIVATE MATHTER_NULL_INITIALIZE=1)
	# add_compile_definitions(MATHTER_NULL_INITIALIZE=1)
elseif(MATHTER_INIT_MODE STREQUAL "INVALID")
	target_compile_definitions(mathter_mathter PRIVATE MATHTER_INVALID_INITIALIZE=1)
	# add_compile_definitions(MATHTER_INVALID_INITIALIZE=1)
elseif(MATHTER_INIT_MODE STREQUAL "UNINITIALIZED")
	target_compile_definitions(mathter_mathter PRIVATE MATHTER_DONT_INITIALIZE=1)
	# add_compile_definitions(MATHTER_DONT_INITIALIZE=1)
endif()

if (${MATHTER_USE_XSIMD})
	message("Vectorization is enabled via the XSimd library.")
	target_compile_definitions(mathter_mathter PRIVATE MATHTER_USE_SIMD=1)
	# add_compile_definitions(MATHTER_USE_XSIMD=1)
else()
	message("Vectorization is disabled.")
endif()

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

# Subdirectories
#add_subdirectory(include/Mathter)
#add_subdirectory(test)
#add_subdirectory(benchmark)

