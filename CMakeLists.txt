cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-Wall -pedantic)

option(USE_CLANG "build application with clang" OFF)

IF(USE_CLANG)
	SET (CMAKE_C_COMPILER "/usr/bin/clang")
ELSE()
	SET (CMAKE_C_COMPILER "/usr/bin/gcc")
	ADD_DEFINITIONS(-std=c99 -D_XOPEN_SOURCE=700)
ENDIF()

INCLUDE_DIRECTORIES(include)
ADD_SUBDIRECTORY(src/)

INSTALL	(	FILES phnxchown.cfg
			DESTINATION /etc/
		)
