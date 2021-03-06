cmake_minimum_required(VERSION 3.8)
project (YGG)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

#set (CMAKE_CXX_STANDARD 14)

# Set a default build type if none was specified
set(default_build_type "Debug")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
          STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
          "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

add_subdirectory(test)
add_subdirectory(examples)


include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-std=c++17 HASCPP17)

find_package(Boost 1.61)

if (Boost_FOUND)
if (${HASCPP17})
	# Find PAPI
	find_package(PAPI)
	if (PAPI_FOUND)
		include_directories(SYSTEM ${PAPI_INCLUDE_DIRS})
		set(LIBS ${LIBS} ${PAPI_LIBRARIES})
		MESSAGE(STATUS, "Enabling statistics collection via PAPI.")
	endif(PAPI_FOUND)

	add_subdirectory (benchmark)
else()
	MESSAGE(WARNING, "Not building benchmarks, your compiler does not support C++17.")
endif()
else()
	MESSAGE(WARNING, "Not building benchmarks, Boost >= 1.61 not found.")
endif()
