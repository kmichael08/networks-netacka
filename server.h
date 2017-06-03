#ifndef NETWORKS_NETACKA_SERVER_H
#define NETWORKS_NETACKA_SERVER_H

#include <iostream>
#include <unistd.h>
#include <cstdint>

using namespace std;

class Server {
private:
    uint64_t seed;
    uint64_t current_random_number;
    uint32_t width = 800, height = 600, port = 12345, speed = 50, turning_speed = 6;
    static const uint64_t RANDOMIZATION_FACTOR = 279470273;
    static const uint64_t RANDOMIZATION_MODULO = 4294967291;
    static const int MAX_CLIENTS = 42;

    uint32_t next_random_number();

    void parse_arguments(int argc, char* argv[]);

public:
    Server(int argc, char* argv[]);

    void print_arguments();

};



#endif //NETWORKS_NETACKA_SERVER_H
