cmake_minimum_required(VERSION 2.6)
project(raspimobot)

file(GLOB_RECURSE sources src/*.c)

find_library(WIRINGPI_LIB NAMES wiringPi)

if(NOT WIRINGPI_LIB)
	add_library(wiringPi_stub lib/wiringPi.c)
	set(WIRINGPI_LIB wiringPi_stub)
	include_directories(lib)
endif(NOT WIRINGPI_LIB)

add_executable(raspimobot ${sources})
target_link_libraries(raspimobot pulse sndfile pthread ${WIRINGPI_LIB})
