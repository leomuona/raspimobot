cmake_minimum_required(VERSION 2.6)
project(raspimobot)

file(GLOB_RECURSE sources src/*.c)

add_executable(raspimobot ${sources})
target_link_libraries(raspimobot pulse sndfile)
