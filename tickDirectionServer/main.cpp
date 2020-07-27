#include <iostream>
#include <memory>
#include <string>
#include <getopt.h>
#include <cstdlib>   // rand()
#include <time.h>	 // for srand

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpc/support/log.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

#include <general/tick_direction_server_version.h>
#include <tickproto/tick.pb.h>
#include <tickproto/tick.grpc.pb.h>

class TickDirectionServerImpl final : public Price::Direction::Service {

public:

	TickDirectionServerImpl(int32_t listen_on) {
		
		server_address.append(":").append(std::to_string(listen_on));
	    srand(time(nullptr));
	}

	Status Move(ServerContext * context,
                const Price::TickDirectionRequest * request,
                Price::TickDirectionDelta * reply) override {
		int32_t price_change = rand()%4+1; // random number 1-4
		int32_t size_change =  rand()%6; // random number 0-5
		int32_t size_direction = (rand()%2 == 0 ? 1 : -1);   // random size direction
		int32_t price_direction = (rand()%2 == 0 ? 1 : -1);  // random price direction

		reply->set_delta_price(price_change*price_direction);
		reply->set_delta_size(size_change*size_direction);

		return Status::OK;
	}

	void Run();

private:
	std::string server_address{"0.0.0.0"};
};

void TickDirectionServerImpl::Run() {

	grpc::EnableDefaultHealthCheckService(true);
	grpc::reflection::InitProtoReflectionServerBuilderPlugin();

	ServerBuilder builder;


	// Listen on the given address without any authentication mechanism.
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

	// Register "service" as the instance through which we'll communicate with
	// clients. In this case it corresponds to an *synchronous* service.
	builder.RegisterService(this);

	// Finally assemble the server.
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;

	// Wait for the server to shutdown. Note that some other thread must be
	// responsible for shutting down the server for this call to ever return.
	server->Wait();
}

int main( int argc, char **argv) {

	int32_t listen_port = 50091;

	struct option longopts[] = {
		{ "listen_port",    required_argument,   nullptr, 'l' },
		{ nullptr,  0,                  nullptr,  0 }
	};

	int32_t ch;
	while ((ch = getopt_long(argc, argv, "l:", longopts, NULL)) != -1) {
		switch(ch) {
			case 'l':
				listen_port = atoi(optarg);
				break;
			case '?':
			default:
				std::cerr << "Usage: " << argv[0] << " [-d [UP|DOWN]] [-l listen_port]" << "\n"
					<< "l   - Port to listen to for client requests" << "\n"
					<< std::endl;
				exit(-1);
		}
	}

	std::cout << "Listen Port: " << listen_port << std::endl;

	TickDirectionServerImpl server(listen_port);
	server.Run();

	return 0;
}
