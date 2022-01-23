CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Wii)

# Preliminary stuff
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	SET(LINUX 1)
	ADD_DEFINITIONS(-DLINUX)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

SET(CMAKE_CXX_STANDARD 11)

SET(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libs")
SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "Release")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Third-party libraries
FIND_PACKAGE(Bluetooth REQUIRED)

# Source code
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(example)

# Config dir
#MAKE_DIRECTORY($ENV{HOME}/.wiic)

# Uninstall Target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

