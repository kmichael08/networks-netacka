#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>
#include "datagramServerToClient.h"
#include "datagramClientToServer.h"
#include "err.h"
#include "client_utils.h"
#include "clock.h"
#include <netinet/tcp.h>

uint32_t game_server_port = 12345, ui_server_port = 12346;
char* ui_server_name;
char* game_server_name;
char* player_name;
uint64_t session_id;

/*================= UDP parameters ============================== */
pollfd* udp_sock = new pollfd;
struct addrinfo udp_addr_hints;
struct addrinfo *udp_addr_result;

int flags, sflags;
ssize_t snd_len, rcv_len;
struct sockaddr_in my_address;
struct sockaddr_in srvr_address;

void init_clock();

socklen_t rcva_len;
const int TIMEOUT_MILLISECS = 2;
/*=================== TCP parameters ============================ */
pollfd* tcp_sock = new pollfd;
const int MAX_RECEIVED_MESSAGE_LENGTH = 20;
struct addrinfo tcp_addr_hints;
struct addrinfo* tcp_addr_result;

/*===================== Client logic  =========================== */
int8_t current_turn_direction = 0;
uint32_t current_game_id = 0;
bool active_game = 0;
uint32_t next_expected_event_no = 0; /* TODO manage the events */
const uint64_t SEND_INTERVAL_IN_MICROSECS = 20000; /* 20ms TODO */
Clock* global_clock;
vector<char*> current_players_names; /* TODO clear with every new game */
/* ================================================ */


void init_connection_with_gui() {
    // 'converting' host/port in string to struct addrinfo
    memset(&tcp_addr_hints, 0, sizeof(struct addrinfo));
    tcp_addr_hints.ai_family = AF_INET; // IPv4
    tcp_addr_hints.ai_socktype = SOCK_STREAM;
    tcp_addr_hints.ai_protocol = IPPROTO_TCP;
    int err = getaddrinfo(ui_server_name, get_string_of_32bit(ui_server_port), &tcp_addr_hints, &tcp_addr_result);
    if (err == EAI_SYSTEM) { // system error
        syserr("getaddrinfo: %s", gai_strerror(err));
    }
    else if (err != 0) { // other error (host not found, etc.)
        fatal("getaddrinfo: %s", gai_strerror(err));
    }

    // initialize socket according to getaddrinfo results
    tcp_sock->fd = socket(tcp_addr_result->ai_family, tcp_addr_result->ai_socktype, tcp_addr_result->ai_protocol);
    if (tcp_sock->fd < 0)
        syserr("socket");

    // connect socket to the server
    if (connect(tcp_sock->fd, tcp_addr_result->ai_addr, tcp_addr_result->ai_addrlen) < 0)
        syserr("connect");

    freeaddrinfo(tcp_addr_result);
    int flag = 1;
    err = setsockopt(tcp_sock->fd,            /* socket affected */
                            IPPROTO_TCP,     /* set option at TCP level */
                            TCP_NODELAY,     /* name of option */
                            (char *) &flag,  /* the cast is historical cruft */
                            sizeof(int));    /* length of option value */

    if (err < 0)
        syserr("NO_DELAY error");
}

void receive_tcp() {
    char* buffer = new char[MAX_RECEIVED_MESSAGE_LENGTH];
    memset(buffer, 0, MAX_RECEIVED_MESSAGE_LENGTH);
    rcv_len = read(tcp_sock->fd, buffer, MAX_RECEIVED_MESSAGE_LENGTH);
    if (rcv_len < 0) {
        syserr("read tcp");
    }
    if (rcv_len == 0)
        syserr("gui disconnected");

    current_turn_direction = take_direction_from_gui_message(buffer, (size_t)rcv_len);
}

/* send tcp with information about the event to gui */
void send_tcp(char* data, size_t len) {
    ssize_t send_len = write(tcp_sock->fd, data, len);
    if (send_len < 0)
        syserr("read");

    if (send_len != ssize_t(len)) {
        syserr("partial / failed write");
    }

}

void init_connection_with_server() {

    // 'converting' host/port in string to struct addrinfo
    (void) memset(&udp_addr_hints, 0, sizeof(struct addrinfo));
    udp_addr_hints.ai_family = AF_INET; // IPv4
    udp_addr_hints.ai_socktype = SOCK_DGRAM;
    udp_addr_hints.ai_protocol = IPPROTO_UDP;
    udp_addr_hints.ai_flags = 0;
    udp_addr_hints.ai_addrlen = 0;
    udp_addr_hints.ai_addr = NULL;
    udp_addr_hints.ai_canonname = NULL;
    udp_addr_hints.ai_next = NULL;
    if (getaddrinfo(game_server_name, NULL, &udp_addr_hints, &udp_addr_result) != 0) {
        syserr("getaddrinfo");
    }

    my_address.sin_family = AF_INET; // IPv4
    my_address.sin_addr.s_addr =
            ((struct sockaddr_in *) (udp_addr_result->ai_addr))->sin_addr.s_addr; // address IP

    my_address.sin_port = htons((uint16_t) game_server_port); // port from the command line
    freeaddrinfo(udp_addr_result);


    udp_sock->fd = socket(PF_INET, SOCK_DGRAM, 0); // creating IPv4 UDP socket
    if (udp_sock->fd < 0)
        syserr("socket");


    sflags = 0;
    rcva_len = (socklen_t) sizeof(my_address);
}

void send_datagram(char *datagram, size_t len) {
    snd_len = sendto(udp_sock->fd, datagram, len, sflags,
                     (struct sockaddr *) &my_address, rcva_len);
    if (snd_len != (ssize_t) len) {
        syserr("partial/failed write");
    }
}

/* Is any input on udp */
bool udp_listen() {
    udp_sock->events = POLLIN;
    udp_sock->revents = 0;
    return poll(udp_sock, 1, TIMEOUT_MILLISECS) > 0;
}

/* Is any input on tcp */
bool tcp_listen() {
    tcp_sock->events = POLLIN;
    tcp_sock->revents = 0;
    return poll(tcp_sock, 1, TIMEOUT_MILLISECS) > 0;
}

Datagram* receive_datagram() {
    char* buffer = new char[DatagramServerToClient::MAX_DATAGRAM_SIZE];
    (void) memset(buffer, 0, DatagramServerToClient::MAX_DATAGRAM_SIZE);
    flags = 0;
    size_t len = (size_t) DatagramServerToClient::MAX_DATAGRAM_SIZE - 1;
    rcva_len = (socklen_t) sizeof(srvr_address);
    rcv_len = recvfrom(udp_sock->fd, buffer, len, flags,
                       (struct sockaddr *) &srvr_address, &rcva_len);

    if (rcv_len < 0)
        syserr("read udp");

    return new Datagram(buffer, (size_t)rcv_len);
}

void init_session_id() { session_id = get_time_microseconds(); }

void print_arguments() {
    cout << player_name << " " << game_server_name << ":" << game_server_port << " " << ui_server_name << ":" << ui_server_port << endl;
}

/**
 * @param arg string from arguments
 * @param name name to be modified
 * @param port port to be modified
 */
void parse_names(char* arg, char*& name, uint32_t& port) {
    char* separator = strchr(arg, ':');
    if (separator == nullptr) {
        name = arg;
    }
    else {
        size_t name_len = separator - arg;
        if (name_len == 0)
            syserr("Name should not be empty");


        name = new char[name_len + 1];
        strncpy(name, arg, name_len);
        name[name_len] = '\0';

        port = get_number(separator + 1);
    }

}

void parse_arguments(int argc, char* argv[]) {
    if (argc < 3 || argc > 4)
        syserr("Bad arguments number - usage ./siktacka-client player_name game_server_host[:port] [ui_server_host[:port]]");
    player_name = argv[1];

    if (!DatagramClientToServer::valid_player_name(player_name))
        syserr("Wrong player name");

    parse_names(argv[2], game_server_name, game_server_port);

    if (argc == 4) {
        parse_names(argv[3], ui_server_name, ui_server_port);
    }
    else {
        ui_server_name = new char[10];
        strncpy(ui_server_name, "localhost", 9);
        ui_server_name[10] = '\0';
    }


}

void init_clock() {
    global_clock = new Clock(SEND_INTERVAL_IN_MICROSECS);
}

/* Process received datagram and send information to gui */
void process_datagram(Datagram* datagram) {
    size_t len = datagram->get_len();
    char* raw_datagram = datagram->get_data();
    DatagramServerToClient* datagramServerToClient = DatagramServerToClient::parse_datagram(raw_datagram, len);


    for (Event* event : datagramServerToClient->get_events()) {
        if (event->get_event_type() == 0) {
            current_players_names.clear();
            NewGame* newGame = (NewGame*) event;
            active_game = true;
            current_game_id = datagramServerToClient->get_game_id();

            for (char* name: newGame->get_players_name_list())
                current_players_names.push_back(name);
        }


        Datagram* tcp_data = message_sent_to_gui(event, current_players_names);
        if (event->get_event_type() == 3) { /* event game_over */
            active_game = false;
            next_expected_event_no = 0;
        }

        else if (active_game) { /* skip messages from the previous game */
            send_tcp(tcp_data->get_data(), tcp_data->get_len());
        }

        next_expected_event_no = event->get_event_no() + 1;
    }

}

/**
 * Datagram made of current data
 */
Datagram* datagram_to_send() {
    DatagramClientToServer* datagramClientToServer =
            new DatagramClientToServer(
                    session_id, current_turn_direction, next_expected_event_no, player_name);
    return datagramClientToServer->get_raw_datagram();
}

int main(int argc, char* argv[]) {
    init_session_id();
    parse_arguments(argc, argv);
    print_arguments();
    init_connection_with_server();
    init_connection_with_gui();
    init_clock();
    while (true) {
        if (global_clock->end_turn()) {
            Datagram* datagram = datagram_to_send();
            send_datagram(datagram->get_data(), datagram->get_len());
            global_clock->next_turn();
            current_turn_direction = 0;
        }
        else { /* receive from server or communicate with gui */
            if (udp_listen()) {
                Datagram* received_datagram = receive_datagram();
                process_datagram(received_datagram);
            }
            /* Communicate with gui */
            if (tcp_listen()) {
                receive_tcp();
            }
        }
    }
}

