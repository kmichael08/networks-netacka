cmake_minimum_required(VERSION 3.7)
project(networks_netacka)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -O2 ")

set(SOURCE_FILES err.h err.cpp utils.h server.cpp main.cpp crc32.h crc32.cpp
        event.h event.cpp datagramClientToServer.cpp datagramClientToServer.h
        datagramServerToClient.cpp datagramServerToClient.h game.cpp game.h utils.cpp clock.cpp clock.h)

set(TEST_S_TO_C err.h err.cpp utils.h server.cpp test_datagrams_server_to_client.cpp crc32.h crc32.cpp
        event.h event.cpp datagramClientToServer.cpp datagramClientToServer.h
        datagramServerToClient.cpp datagramServerToClient.h game.cpp game.h utils.cpp client_utils.h client_utils.cpp clock.cpp clock.h)

set(TEST_TIME utils.cpp utils.h err.cpp err.h crc32.cpp crc32.h test_gettime.cpp)

set(CLIENT client_utils.h client_utils.cpp event.h event.cpp crc32.h crc32.cpp utils.h utils.cpp
        err.h err.cpp datagramServerToClient.cpp datagramServerToClient.h
        datagramClientToServer.h datagramClientToServer.cpp client.cpp clock.cpp clock.h)

add_executable(siktacka-server ${SOURCE_FILES})
add_executable(test_s_to_c ${TEST_S_TO_C})
add_executable(test_time ${TEST_TIME})
add_executable(siktacka-client ${CLIENT})