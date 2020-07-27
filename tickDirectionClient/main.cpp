#include "tickDirectionClientImpl.h"
#include <general/tick_direction_client_version.h>

int main( int argc, char **argv) {

    int32_t server_port = 50091;


    struct option longopts[] = {
            { "server_port",    required_argument,   nullptr, 's' },
            { "help",           no_argument      ,   nullptr, 'h' },
            { nullptr,  0,                  nullptr,  0 }
    };

    int32_t ch;
    while ((ch = getopt_long(argc, argv, "i:s:h", longopts, NULL)) != -1) {
        switch(ch) {
            case 's':
                server_port = atoi(optarg);
                break;
            case 'h':
            case '?':
            default:
                std::cout << "tickDirectionclient [-s ServerListenPort] [-h]" << std::endl;
                exit( -1 );
        }
    }

    std::cout << "Direction Server Listen Port: " << server_port << std::endl;
    std::string serverDest{"localhost"};
    serverDest.append(":").append(std::to_string(server_port));

    TickDirectionClientImpl client(grpc::CreateChannel(serverDest, grpc::InsecureChannelCredentials()));

    while(true) {
		std::cout << "Before Move" << std::endl;
        auto currState = client.Move();
		std::cout << "After Move" << std::endl;;
        if (std::get<0>(currState) != TickDirectionClientImpl::INVALID_PRICE)
            std::cout << "Direction[ price:" << std::get<0>(currState) << " size:" << std::get<1>(currState) << " ]" << std::endl;
        sleep(1);
    }

    return 0;
}
