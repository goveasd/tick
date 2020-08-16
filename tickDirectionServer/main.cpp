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

	TickDirectionServerImpl(const std::string& listen_on)
	    : server_address(listen_on) {
		
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
	std::string server_address;
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

	grpc::HealthCheckServiceInterface* hlth_service = server->GetHealthCheckService();
	hlth_service->SetServingStatus("Price.Direction.Health", true); 

	// Wait for the server to shutdown. Note that some other thread must be
	// responsible for shutting down the server for this call to ever return.
	server->Wait();
}

int main( int argc, char **argv) {

    constexpr char dflt_ip_endpoint[] = "localhost:50091";

	std::string server_ep;

	struct option longopts[] = {
		{ "listen_ep",    required_argument,   nullptr, 'l' },
		{ nullptr,  0,    nullptr,  0 }
	};

	int32_t ch;
	while ((ch = getopt_long(argc, argv, "l:", longopts, NULL)) != -1) {
		switch(ch) {
			case 'l':
				{
					int32_t listen_port = atoi(optarg);
					if (listen_port > 0) {
						server_ep.append("localhost:").append(std::to_string(listen_port));
					}
					else {
						// Assume provided EndPoint is a Unix socket
						server_ep = optarg;
					}
				}
				break;
			case '?':
			default:
				std::cerr << "Usage: " << argv[0] << " [-l listen_port]" << "\n"
					<< "l   - IP Port or Unix socket to listen to for client requests. Default: " << dflt_ip_endpoint << "\n"
					<< std::endl;
				exit(-1);
		}
	}

	if (server_ep.empty()) {
		server_ep = dflt_ip_endpoint;
		std::cout << "Defaulting to listen at Endpoint: " << server_ep << std::endl;
	}

	std::cout << "Direction Server Listen Port: " << server_ep << std::endl;

	TickDirectionServerImpl server(server_ep);
	server.Run();

	return 0;
}
