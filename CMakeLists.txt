cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-Wall -pedantic)

OPTION(USE_CLANG "build application with clang" OFF)

IF(USE_CLANG)
	SET (CMAKE_C_COMPILER "/usr/bin/clang")
ELSE()
	SET (CMAKE_C_COMPILER "/usr/bin/gcc")
	ADD_DEFINITIONS(-std=c99 -D_XOPEN_SOURCE=700)
ENDIF()

SET(CONFIG_PATH "/etc/userchown.cfg" CACHE FILEPATH
	"Path hard-coded in executable where the configuration file is")

SET(CONFIG_SOURCE "cfg/userchown.cfg" CACHE FILEPATH
	"Path to configuration file to use")

INCLUDE_DIRECTORIES(include)
ADD_SUBDIRECTORY(src/)

INSTALL	(	FILES ${CONFIG_SOURCE}
			DESTINATION ${CONFIG_PATH}
		)
