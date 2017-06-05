#include "server.h"

int main(int argc, char* argv[]) {
    Server server = Server(argc, argv);
    server.print_arguments();

    while (true) {
        if (!server.is_game_active()) {
            if (server.all_players_ready())
                server.new_game();
        }
        else {
            server.receive_udp();
            server.send_to_all();
            if (server.get_current_game()->game_ended()) {
                server.get_current_game()->end_game();
                server.finish_game();
            }
        }
    }

}
