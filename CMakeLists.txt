cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-Wall -pedantic)
ADD_DEFINITIONS(-std=c99 -D_XOPEN_SOURCE=700)

IF(CMAKE_BUILD_TYPE MATCHES "Rel")
	ADD_DEFINITIONS(-D_FORTIFY_SOURCE=2)
ENDIF()

SET(CONFIG_PATH "/etc/userchown.cfg" CACHE FILEPATH
	"Path hard-coded in executable where the configuration file is")

SET(CONFIG_SOURCE "cfg/userchown.cfg" CACHE FILEPATH
	"Path to configuration file to use")

SET(PROG_NAME "userchown" CACHE STRING
    "Name of the executable to generate")

INCLUDE_DIRECTORIES(include)
ADD_SUBDIRECTORY(src/)

OPTION(WANT_MANPAGE "build and install a manpage for the command" ON)

IF(WANT_MANPAGE)
	ADD_SUBDIRECTORY(man/)
ENDIF()

GET_FILENAME_COMPONENT(CFG_REAL_PATH ${CONFIG_PATH} PATH)
GET_FILENAME_COMPONENT(CFG_REAL_NAME ${CONFIG_PATH} NAME)

INSTALL	(	FILES ${CONFIG_SOURCE}
			DESTINATION "${CFG_REAL_PATH}/"
			RENAME "${CFG_REAL_NAME}"
		)
