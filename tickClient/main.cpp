#include "TickClientImpl.h"

int main( int argc, char **argv) {

	constexpr char dflt_unix_endpoint[] = "unix:/tmp/tickServer";
    constexpr char dflt_ip_endpoint[] = "localhost:50081";
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
                std::cout << "tickclient [-s ServerListenEndPoint] [-h]" << "\n\n"
					      << "ServerListenEndPoint can be a IP:PORT or a UnixDomainSocket File"
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

    TickClientImpl client(grpc::CreateChannel(server_ep, grpc::InsecureChannelCredentials()) );

	uint32_t cnt{0};
    while(true) {
        auto currState = client.CurrentPrice();
        std::cout << "Price: " << std::get<0>(currState) << " Size:" << std::get<1>(currState) << std::endl;
        sleep(1);

		if (++cnt%10 == 0)
			client.isServerGood();
    }

    return 0;
}
