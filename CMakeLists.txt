cmake_minimum_required(VERSION 2.6)

include_directories(include)

FILE(GLOB CSources *.c)
add_executable(phnxchown ${CSources})

