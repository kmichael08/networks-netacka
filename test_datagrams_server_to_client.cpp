#include <cstring>
#include "server.h"
#include "datagramServerToClient.h"
#include <cassert>
#include "event.h"
#include "utils.h"
#include "client_utils.h"

int main(int argc, char* argv[]) {
   Server server = Server(argc, argv);
    uint32_t game_id = 30020;
    vector<Event*> events;
    Event* first = new GameOver(10);
    events.push_back(first);
    Event* second = new Pixel(12, 3, 4 ,5);
    events.push_back(second);
    Event* third = new PlayerEliminated(4, 44);
    events.push_back(third);
    vector<char*> players_names_list = {"Jan", "Marian", "Arnold" };
    Event* fourth = new NewGame(11, 1200, 800, players_names_list);
    events.push_back(fourth);

    DatagramServerToClient* datagram = new DatagramServerToClient(game_id, events);
    vector<Datagram*> datagrams = datagram->datagrams();

    assert(datagrams.size() == 1);

    uint32_t g_id;
    memcpy(&g_id, datagrams[0]->get_data(), 4);
    cout << ntohl(g_id) << endl;

    DatagramServerToClient* parsed_datagram =
            DatagramServerToClient::parse_datagram(datagrams[0]->get_data(), datagrams[0]->get_len());


     cout << parsed_datagram->get_game_id() << endl;

    cout << parsed_datagram->get_events().size() << endl;

    for (Event* event: parsed_datagram->get_events()) {
        uint8_t type = event->get_event_type();
        cout << (unsigned long)type << " TYP "<< endl;

        if (type == 0) {
            NewGame* newGame = (NewGame*) event;
            cout << newGame->get_maxx() << " " << newGame->get_maxy() << endl;
            for (char* name: newGame->get_players_name_list()) {
                cout << name << endl;
            }
        }
        else if (type == 1) {
            Pixel* pixel = (Pixel*) event;
            cout << (unsigned long)(pixel->get_player_number()) << " " << pixel->get_x() << " " << pixel->get_y() << endl;
        }
        else if (type == 2) {
            PlayerEliminated* playerEliminated = (PlayerEliminated*) event;
            cout << (unsigned long) (playerEliminated->get_player_number()) << " PL eliminated pl number" << endl;
        }
        else if (type == 3) {
            GameOver* gameOver = (GameOver*) event;
            cout << (unsigned long)(gameOver->get_event_type()) << " GAME OVER" << endl;
        }

    }

    for (int i = 0; i < 100; i++)
        events.push_back(new NewGame(11, 1200, 800, players_names_list));

    DatagramServerToClient* dgram = new DatagramServerToClient(game_id, events);
    vector<Datagram*> dgrams = dgram->datagrams();

    cout << dgrams.size() << endl;

    /* =============================================== */
    cout << endl << endl;

    DatagramServerToClient* parsed_dgram =
            DatagramServerToClient::parse_datagram(dgrams[1]->get_data(), dgrams[1]->get_len());


    cout << parsed_dgram->get_game_id() << endl;

    cout << parsed_dgram->get_events().size() << endl;

    for (Event* event: parsed_dgram->get_events()) {
        uint8_t type = event->get_event_type();
        cout << (unsigned long)type << " TYP "<< endl;

        if (type == 0) {
            NewGame* newGame = (NewGame*) event;
            cout << newGame->get_maxx() << " " << newGame->get_maxy() << endl;
            for (char* name: newGame->get_players_name_list()) {
                cout << name << endl;
            }
        }
        else if (type == 1) {
            Pixel* pixel = (Pixel*) event;
            cout << (unsigned long)(pixel->get_player_number()) << " " << pixel->get_x() << " " << pixel->get_y() << endl;
        }
        else if (type == 2) {
            PlayerEliminated* playerEliminated = (PlayerEliminated*) event;
            cout << (unsigned long) (playerEliminated->get_player_number()) << " PL eliminated pl number" << endl;
        }
        else if (type == 3) {
            GameOver* gameOver = (GameOver*) event;
            cout << (unsigned long)(gameOver->get_event_type()) << " GAME OVER" << endl;
        }

    }


    return 0;
}
