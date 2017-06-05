#include <algorithm>
#include <cstring>
#include "server.h"
#include "err.h"
#include "datagramServerToClient.h"

using VPIT = vector<Player*>::iterator;

/* Parse arguments, exit with code 1 and a message in case of failure */
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
    sock = new pollfd;
    sock->fd = socket(AF_INET, SOCK_DGRAM, 0); // creating IPv4 UDP socket
    if (sock->fd < 0)
        syserr("socket");

    server_address.sin_family = AF_INET; //IPv4
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); //listening on all interfaces
    server_address.sin_port = htons(port);

    // bin the socket to a concrete address
    if (bind(sock->fd, (struct sockaddr *) &server_address, (socklen_t) sizeof(server_address)) < 0)
        syserr("bind");

    snda_len = (socklen_t) sizeof(client_address);

}

/* Comparator to sort players by their names lexicographically */
bool compare_players(Player* pl1, Player* pl2) {
    return pl1->get_name() < pl2->get_name();
}

void Server::init_players(vector<Player *>& current_players, Game* game) {
    uint8_t player_num = 0;
    for (Player* player : players) {
        player->set_player_number(player_num);
        uint32_t headx = next_random_number() % width;
        uint32_t heady = next_random_number() % height;
        double direction = next_random_number() % 360;
        player->set_parameters(headx, heady, direction);
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

void Server::new_game() {
    uint32_t game_id = next_random_number();
    vector<char*> players_names;
    vector<Player*> current_players(players); /* Copy players, only the ones, that were present before start of the game */
    sort(current_players.begin(), current_players.end(), compare_players); /* lexicographical sort by names */

    for (Player* player: current_players)
        players_names.push_back(player->get_name());

    Game* game = new Game(game_id, width, height, current_players, turn_time(), turning_speed);

    game->add_new_game(width, height, players_names);

    init_players(current_players, game);

    active_game = true;

    current_game = game;
}

uint32_t Server::turn_time() {
    return 1000 / speed;
}

bool Server::listen()  {
    sock->events = POLLIN;
    sock->revents = 0;
    return poll(sock, 1, TIMEOUT_MILLISECS) == 1;
}

void Server::send_udp(Player* player, char* datagram) {
    size_t len = strlen(datagram);
    int sflags = 0;
    ssize_t snd_len = sendto(sock->fd, datagram, (size_t) len, sflags,
                             (sockaddr *) player->get_client_address(), snda_len);
    if (snd_len < 0 || size_t(snd_len) != len)
        syserr("error on sending datagram to client socket");
}

int Server::current_players_number() {
    return (uint32_t)spectators.size() + (uint32_t)players.size();
}

void Server::receive_udp() {
    struct sockaddr_in* client_address = new(sockaddr_in);
    rcva_len = (socklen_t) sizeof(*client_address);
    int flags = 0; // we do not request anything special
    char* buffer = new char[MAX_CLIENT_DATAGRAM_SIZE];

    size_t len = (size_t) recvfrom(sock->fd, buffer, (size_t) MAX_CLIENT_DATAGRAM_SIZE, flags,
                                   (sockaddr *) client_address, &rcva_len);

    DatagramClientToServer* datagram = new DatagramClientToServer(buffer);

    Player* player = get_player(client_address);

    if (player == nullptr) { /* First time we hear from the player */
        /* Players number exceeded or name already exists */
        if (current_players_number() == MAX_CLIENTS || name_exist(datagram->get_player_name()))
            return;
        else
        {
            player = add_new_player(datagram, client_address);
        }
    }
    else { /* Existing player */
        player->update();
        /* if the session id is larger than initial, the player is reseted TODO test it */
        if (datagram->get_session_id() > player->get_session_id()) {
            reset_player(player, datagram->no_player_name());
        }
    }

    if (!datagram->is_valid()) {} /* TODO init the game OR move_snake */
    else {
        send_events(datagram->get_next_expected_event_no(), player);
        if (active_game)
            current_game->move_snake(datagram->get_turn_direction(), player);
        else
            player->reborn();
    }
}

Player* Server::add_new_player(DatagramClientToServer *datagram, sockaddr_in *client_address) {
    char* name = datagram->get_player_name();

    Player* player = new Player(datagram->get_session_id(), name, client_address);

    if (datagram->no_player_name())
        spectators.push_back(player);
    else
        players.push_back(player);

    return player;
}


Player* Server::get_player(sockaddr_in* client_address, vector<Player*>& players_list) {
    for (Player* player: players_list)
        if (player->equal_address(client_address))
            return player;
    return nullptr;
}

Player* Server::get_player(sockaddr_in *client_address) {
    Player* result = get_player(client_address, players); /* active players */
    if (result != nullptr)
        return result;
    result = get_player(client_address, spectators); /* search among spectators */
    return result;
}


void Server::send_events(uint32_t first_event, Player *player) {
    if (first_event >= current_game->get_events_number()) /* there is less event */
        return;

    vector<Event*> events_to_send = current_game->get_events_from(first_event);

    DatagramServerToClient* data = new DatagramServerToClient(current_game->get_game_id(), events_to_send);

    for (char* datagram : data->datagrams())
        send_udp(player, datagram);

}
void Server::reset_player(Player *player, vector<Player *> &players_list) {
    for (VPIT iter = players_list.begin(); iter != players_list.end(); iter++)
        if ((*iter) == player)
            (*iter) = new Player(player);

}


void Server::reset_player(Player *player, bool is_spectator) {
    if (is_spectator) {
        reset_player(player, spectators);
    }
    else {
        reset_player(player, players);
    }
}

bool Server::name_exist(char* name) {
    for (Player* player: players)
        if (strncmp(player->get_name(), name, strlen(name)))
            return true;

    return false;
}

void Server::send_to_all() {
    for (Player* player: players)
        send_events(first_event_not_sent_to_all, player);
    for (Player* player: spectators)
        send_events(first_event_not_sent_to_all, player);

    first_event_not_sent_to_all = current_game->get_events_number();
}

bool Server::is_game_active() const {
    return active_game;
}

Game *Server::get_current_game() {
    return current_game;
}

void Server::finish_game() {
    active_game = false;
}

bool Server::all_players_ready() const {
    for (Player* player: players)
        if (!player->is_alive())
            return false;
    return true;
}


