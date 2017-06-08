#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "datagramServerToClient.h"
#include "datagramClientToServer.h"
#include "err.h"
#include "client_utils.h"
#include "clock.h"

uint32_t game_server_port = 12345, ui_server_port = 12346;
char* ui_server_name;
char* game_server_name;
char* player_name;
uint64_t session_id;

/*================================================= */
int sock;
struct addrinfo addr_hints;
struct addrinfo *addr_result;

int flags, sflags;
ssize_t snd_len, rcv_len;
struct sockaddr_in my_address;
struct sockaddr_in srvr_address;

void init_clock();

socklen_t rcva_len;

/*================================================= */
int8_t current_turn_direction = 0;
uint32_t current_game_id = 0;
uint32_t next_expected_event_no = 0; /* TODO manage the events */
const uint64_t SEND_INTERVAL_IN_MICROSECS = 20000; /* 20ms */
Clock* global_clock;
/* ================================================ */


void init_connection_with_server() {

    // 'converting' host/port in string to struct addrinfo
    (void) memset(&addr_hints, 0, sizeof(struct addrinfo));
    addr_hints.ai_family = AF_INET; // IPv4
    addr_hints.ai_socktype = SOCK_DGRAM;
    addr_hints.ai_protocol = IPPROTO_UDP;
    addr_hints.ai_flags = 0;
    addr_hints.ai_addrlen = 0;
    addr_hints.ai_addr = NULL;
    addr_hints.ai_canonname = NULL;
    addr_hints.ai_next = NULL;
    if (getaddrinfo(game_server_name, NULL, &addr_hints, &addr_result) != 0) {
        syserr("getaddrinfo");
    }

    my_address.sin_family = AF_INET; // IPv4
    my_address.sin_addr.s_addr =
            ((struct sockaddr_in *) (addr_result->ai_addr))->sin_addr.s_addr; // address IP

    my_address.sin_port = htons((uint16_t) game_server_port); // port from the command line
    freeaddrinfo(addr_result);

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        syserr("socket");

    sflags = 0;
    rcva_len = (socklen_t) sizeof(my_address);
}

void send_datagram(char *datagram, size_t len) {
    snd_len = sendto(sock, datagram, len, sflags,
                     (struct sockaddr *) &my_address, rcva_len);
    if (snd_len != (ssize_t) len) {
        syserr("partial/failed write");
    }
}

Datagram* receive_datagram() {
    char* buffer = new char[DatagramServerToClient::MAX_DATAGRAM_SIZE];
    (void) memset(buffer, 0, sizeof(buffer));
    flags = 0;
    size_t len = (size_t) sizeof(buffer) - 1;
    rcva_len = (socklen_t) sizeof(srvr_address);
    rcv_len = recvfrom(sock, buffer, len, flags,
                       (struct sockaddr *) &srvr_address, &rcva_len);

    if (rcv_len < 0)
        syserr("read");

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
    init_clock();

    while (true) {
        if (global_clock->end_turn()) {
            Datagram* datagram = datagram_to_send();
            send_datagram(datagram->get_data(), datagram->get_len());
            global_clock->next_turn();
        }
        else { /* receive from server or communicate with gui */

        }
    }
}

