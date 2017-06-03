#include <iostream>
#include <unistd.h>
#include <time.h>
#include "err.h"

using namespace std;

unsigned long width = 800, height = 600, port = 12345, speed = 50, turning_speed = 6;
uint64_t seed;
uint64_t current_random_number;
const uint64_t RANDOMIZATION_FACTOR = 279470273;
const uint64_t RANDOMIZATION_MODULO = 4294967291;


unsigned long get_number(char *str_num) {
    if (strchr(str_num, '-') != NULL)
        syserr("Not a nonnegative number");
    unsigned long result = strtoul(str_num, NULL, 10);
    if (!result || errno == ERANGE)
        syserr("Wrong number");
    return result;
}


void parse_arguments(int argc, char* argv[]) {
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
                speed = strtoul(optarg, NULL, 10);
                if (!speed)
                    exit(1);
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

uint32_t next_random_number() {
    current_random_number = (current_random_number * RANDOMIZATION_FACTOR) % RANDOMIZATION_MODULO;
    return (uint32_t ) current_random_number;
}

void print_arguments() {
    cout << width << " " << height << " " << port << " " << speed << " " << turning_speed << " " << seed << endl;
}

int main(int argc, char* argv[]) {
    parse_arguments(argc, argv);
    print_arguments();
    cout << next_random_number() << endl;
    cout << next_random_number() << endl;
}

