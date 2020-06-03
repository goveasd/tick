#include <iostream>
#include <memory>
#include <string>
#include <getopt.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

#include <general/tick_client_version.h>
#include <tickproto/tick.pb.h>
#include <tickproto/tick.grpc.pb.h>

class TickClientImpl {
    int32_t current_price{100};
    int32_t current_size{600};
    std::unique_ptr<Price::Tob::Stub> stub_;
public:
    TickClientImpl(std::shared_ptr<Channel> channel, int32_t init_price)
        : stub_(Price::Tob::NewStub(channel)), current_price(init_price) {
    }

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::pair<int32_t, int32_t> CurrentPrice() {
        // Data we are sending to the server.
        Price::TickRequest request;
        request.set_symbol("IBM");

        // Container for the data we expect from the server.
        Price::TickDelta reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->CurrentPrice(&context, request, &reply);

        // Act upon its status.
        if (status.ok()) {
            current_price += reply.delta_price();
            current_size += reply.delta_size();
            return std::make_pair(current_price, current_size);
        }
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;

        return std::make_pair(-1, -1);
    }
};


int main( int argc, char **argv) {

    int32_t initial_price = 100;
    int32_t server_port = 50081;


    struct option longopts[] = {
            { "initial_price",  required_argument,   nullptr, 'i' },
            { "server_port",    required_argument,   nullptr, 's' },
            { nullptr,  0,                  nullptr,  0 }
    };

    int32_t ch;
    while ((ch = getopt_long(argc, argv, "i:s:", longopts, NULL)) != -1) {
        switch(ch) {
            case 'i':
                initial_price = atoi(optarg);
                break;
            case 's':
                server_port = atoi(optarg);
                break;
            case '?':
            default:
                exit(-1);
        }
    }

    std::cout << "Server Listen Port: " << server_port << " InitialPrice=" << initial_price << std::endl;
    std::string serverDest{"localhost"};
    serverDest.append(":").append(std::to_string(server_port));

    TickClientImpl client(grpc::CreateChannel(serverDest, grpc::InsecureChannelCredentials()), initial_price);

    while(true) {
        auto currState = client.CurrentPrice();
        if (std::get<0>(currState) > 0)
            std::cout << "Price: " << std::get<0>(currState) << " Size:" << std::get<1>(currState) << std::endl;
        sleep(1);
    }

    return 0;
}
