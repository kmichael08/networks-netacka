#include "server.h"

int main(int argc, char* argv[]) {
    Server server = Server(argc, argv);
    server.print_arguments();
}
