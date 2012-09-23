cmake_minimum_required(VERSION 2.6)

INCLUDE_DIRECTORIES(include)
ADD_SUBDIRECTORY(src/)

INSTALL	(	FILES phnxchown.cfg
			DESTINATION /etc/
		)