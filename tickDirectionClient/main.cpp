#include "tickDirectionClientImpl.h"
#include <general/tick_direction_client_version.h>

int main( int argc, char **argv) {

	constexpr char dflt_unix_endpoint[] = "unix:/tmp/DirectionServer";
    constexpr char dflt_ip_endpoint[] = "localhost:50091";
	const char * server_ep{nullptr};

    struct option longopts[] = {
            { "server_ep",    required_argument,   nullptr, 's' },
            { "help",         no_argument      ,   nullptr, 'h' },
            { nullptr,  0,    nullptr,  0 }
    };

    int32_t ch;
    while ((ch = getopt_long(argc, argv, "s:h", longopts, NULL)) != -1) {
        switch(ch) {
            case 's':
                server_ep = optarg;
                break;
            case 'h':
                std::cout << "tickDirectionClient [-s ServerListenEndPoint] [-h]" << "\n\n"
					      << "ServerListenEndPoint can be a IP:PORT or a UnixDomainSocket File in the format unix:/FileName "
						  << std::endl;
				exit( 0 );
            case '?':
            default:
                exit( -1 );
        }
    }

	if (!server_ep) {
		std::cout << "** Defaulting Server EndPoint to " << dflt_ip_endpoint << std::endl;
		server_ep = dflt_ip_endpoint;
	}

    std::cout << "Connecting to Tick Server over: " << server_ep << std::endl;

    TickDirectionClientImpl client(grpc::CreateChannel(server_ep, grpc::InsecureChannelCredentials()));

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
