cmake_minimum_required(VERSION 3.7)
project(networks_netacka)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -O2 ")

set(SOURCE_FILES err.h err.cpp utils.h server.cpp main.cpp
        event.h event.cpp datagramClientToServer.cpp datagramClientToServer.h
        datagramServerToClient.cpp datagramServerToClient.h game.cpp game.h utils.cpp)
add_executable(siktacka-server ${SOURCE_FILES})