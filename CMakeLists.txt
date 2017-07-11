cmake_minimum_required(VERSION 2.8)

add_executable(music main.cpp)
target_link_libraries(music sfml-graphics sfml-window sfml-system sfml-audio curses)

if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
endif()
