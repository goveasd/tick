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

#include <tickproto/tick.pb.h>
#include <tickproto/tick.grpc.pb.h>

class TickDirectionClientImpl {
    std::unique_ptr<Price::Direction::Stub> stub_;
public:
	constexpr static int32_t INVALID_PRICE{ -6660666 };

    TickDirectionClientImpl(std::shared_ptr<Channel> channel)
        : stub_(Price::Direction::NewStub(channel)) {
    }

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
	std::pair<int32_t, int32_t>  Move() {
        // Data we are sending to the server.
        Price::TickDirectionRequest request;
        request.set_symbol("IBM");

        // Container for the data we expect from the server.
        Price::TickDirectionDelta reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->Move(&context, request, &reply);

        // Act upon its status.
		int32_t current_price_delta{INVALID_PRICE};
		int32_t current_size_delta{0};
        if (status.ok()) {
            current_price_delta = reply.delta_price();
            current_size_delta = reply.delta_size();
        }
		else {
			std::cout << status.error_code() << ": " << status.error_message() << std::endl;
		}

        return std::make_pair(current_price_delta, current_size_delta);
    }
};



