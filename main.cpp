#include "server.h"

int main(int argc, char* argv[]) {
    Server server = Server(argc, argv);
    //server.print_arguments();

    while (true) {
        if (server.udp_listen()) { /* receive udp */
            server.receive_udp();
        }

        if (!server.is_game_active()) { /* check if the new game can be started */
            if (server.all_players_ready())
                server.new_game();
        }
        else {
            server.send_to_all(); /* send all recent communicates */

            if (server.get_current_game()->game_ended()) {
                server.get_current_game()->end_game();
                server.finish_game();
            }
            else { /* if game not ended, check if the turn is not ending */
                if (server.get_current_game()->end_turn()) { /* turn time ended */
                    server.get_current_game()->move_snakes();
                    server.get_current_game()->next_turn();
                }
            }
        }
        server.disconnect_not_responding_users();
    }

}
