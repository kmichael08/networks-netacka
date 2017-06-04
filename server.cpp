#include <algorithm>
#include "server.h"

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
    uint64_t result = current_random_number;
    current_random_number = (current_random_number * RANDOMIZATION_FACTOR) % RANDOMIZATION_MODULO;
    return (uint32_t ) result;
}

void Server::print_arguments() {
    cout << width << " " << height << " " << port << " " << speed << " " << turning_speed << " " << seed << endl;
}

Server::Server(int argc, char **argv) {
    parse_arguments(argc, argv);
}

bool compare_players(Player* pl1, Player* pl2) {
    return pl1->get_name() < pl2->get_name();
}

void Server::init_players(vector<Player *>& current_players, Game* game) {
    uint8_t player_num = 0;
    for (Player* player : players) {
        uint32_t headx = next_random_number() % width;
        uint32_t heady = next_random_number() % height;
        double direction = next_random_number() % 360;
        player->set_parameters(headx, heady, direction);
        player->reborn();
        if (game->get_board()->is_occupied(headx, heady)) {
            game->add_player_eliminated(player_num);
        }
        else {
            game->get_board()->take(headx, heady);
            game->add_pixel(player_num, headx, heady);
        }
        player_num++;
    }

}

Game* Server::new_game(uint32_t width, uint32_t height, vector<Player*> &players) {
    uint32_t game_id = next_random_number();
    vector<char*> players_names;
    vector<Player*> current_players(players);
    sort(current_players.begin(), current_players.end(), compare_players);

    for (Player* player: current_players)
        players_names.push_back(player->get_name());

    Game* game = new Game(game_id, width, height, current_players, turn_time(), turning_speed);

    game->add_new_game(width, height, players_names);

    init_players(current_players, game);

    return game;
}

uint32_t Server::turn_time() {
    return 1000 / speed;
}