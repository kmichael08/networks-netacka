#include "server.h"
#include "utils.h"

void Server::parse_arguments(int argc, char **argv) {

    int c;
    seed = (uint64_t) time(NULL);

    while ((c = getopt(argc, argv, "W:H:p:s:t:r:")) != -1) {
        switch(c) {
            case 'W':
                width = get_number(optarg);
                break;
            case 'H':
                height = get_number(optarg);
                break;
            case 'p':
                port = get_number(optarg);
                break;
            case 's':
                speed = get_number(optarg);
                break;
            case 't':
                turning_speed = get_number(optarg);
                break;
            case 'r':
                seed = (uint64_t) get_number(optarg);
                break;
            default:
                exit(1);
        }
    }
    current_random_number = seed;
}

uint32_t Server::next_random_number() {
    current_random_number = (current_random_number * RANDOMIZATION_FACTOR) % RANDOMIZATION_MODULO;
    return (uint32_t ) current_random_number;
}

void Server::print_arguments() {
    cout << width << " " << height << " " << port << " " << speed << " " << turning_speed << " " << seed << endl;
}

Server::Server(int argc, char **argv) {
    parse_arguments(argc, argv);

}