#include "TickClientImpl.h"

int main( int argc, char **argv) {

    int32_t server_port = 50081;


    struct option longopts[] = {
            { "server_port",    required_argument,   nullptr, 's' },
            { "help",           no_argument      ,   nullptr, 'h' },
            { nullptr,  0,                  nullptr,  0 }
    };

    int32_t ch;
    while ((ch = getopt_long(argc, argv, "s:h", longopts, NULL)) != -1) {
        switch(ch) {
            case 's':
                server_port = atoi(optarg);
                break;
            case 'h':
                std::cout << "tickclient [-s ServerListenPort] [-h]" << std::endl;
		exit( 0 );
            case '?':
            default:
                exit( -1 );
        }
    }

    std::cout << "Server Listen Port: " << server_port << std::endl;
    std::string serverDest{"localhost"};
    serverDest.append(":").append(std::to_string(server_port));

    TickClientImpl client(grpc::CreateChannel(serverDest, grpc::InsecureChannelCredentials()) );

    while(true) {
        auto currState = client.CurrentPrice();
        std::cout << "Price: " << std::get<0>(currState) << " Size:" << std::get<1>(currState) << std::endl;
        sleep(1);
    }

    return 0;
}
