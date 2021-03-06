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
#include <grpcpp/ext/health_check_service_server_builder_option.h>
#include <grpcpp/health_check_service_interface.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

#include <general/tick_client_version.h>
#include <tickproto/tick.pb.h>
#include <tickproto/tick.grpc.pb.h>
#include <tickproto/tick.health.pb.h>
#include <tickproto/tick.health.grpc.pb.h>

class TickClientImpl {
    int32_t current_price{100};
    int32_t current_size{600};
    std::unique_ptr<Price::Tob::Stub> stub_;
    std::unique_ptr<grpc::health::v1::Health::Stub> hlthstub_;

public:

    TickClientImpl(std::shared_ptr<Channel> channel)
        : stub_(Price::Tob::NewStub(channel)),
	      hlthstub_(grpc::health::v1::Health::NewStub(channel)) {
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
        if (status.ok() ) {
			if ( reply.status() == Price::TickDelta_Status_GOOD) {
				current_price = reply.delta_price();
				current_size = reply.delta_size();

				return std::make_pair(current_price, current_size);
			} 

			std::cout << "Server Internal error"
	                  << std::endl;

			return std::make_pair(-888888, -1);
        } 

		std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;

		return std::make_pair(-999999, -1);
	}

	bool isServerGood() const {
		
		grpc::health::v1::HealthCheckRequest hlth_req;
		hlth_req.set_service("Price.Health");

		grpc::health::v1::HealthCheckResponse hlth_res;

        ClientContext context;
		Status status = hlthstub_->Check(&context, hlth_req, &hlth_res);

		if (!status.ok()) {
			std::cerr << "Server HLTH CHECK != OK" << std::endl;
			return false;
		}

		if (hlth_res.status() != grpc::health::v1::HealthCheckResponse::SERVING) {
			std::cerr << "Server HLTH CHECK - NOT SERVING" << std::endl;
			return false;
		}

		std::cout << "Tick Server is healthy!!" << std::endl;

		return true;
	}
};



