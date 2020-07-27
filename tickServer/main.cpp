#include <iostream>
#include <memory>
#include <string>
#include <getopt.h>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpc/support/log.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

#include <general/tick_server_version.h>
#include <tickproto/tick.pb.h>
#include <tickproto/tick.grpc.pb.h>

#include "../tickDirectionClient/tickDirectionClientImpl.h"

class TickServerImpl final : public Price::Tob::Service {

public:

	TickServerImpl(int32_t listen_on, const std::string& direction_srvr_id, int32_t cur_price, int32_t cur_sz ) 
		: ds_location(direction_srvr_id), current_price(cur_price), current_size(cur_sz) {
		
		server_address.append(":").append(std::to_string(listen_on));
	
	}

	Status CurrentPrice(ServerContext * context,
                        const Price::TickRequest * request,
                        Price::TickDelta * reply) override {

		createDirectionServerClient();

		auto currState = ds_client->Move();
		if (std::get<0>(currState) != TickDirectionClientImpl::INVALID_PRICE) {
			//std::cout << "Direction[ price:" << std::get<0>(currState) << " size:" << std::get<1>(currState) << " ]" << std::endl;

			current_price += std::get<0>(currState);
			current_size += std::get<1>(currState);

			reply->set_delta_price(current_price);
			reply->set_delta_size(current_size);
			reply->set_status(Price::TickDelta_Status_GOOD);
		}
		else {
			reply->set_status(Price::TickDelta_Status_ERROR);
		}


		return Status::OK;
	}

	void Run();

private:

	void createDirectionServerClient() {

		if (ds_client)
			return;

		std::cout << "Connecting to Direction Server: " << ds_location << std::endl;

		ds_client.reset( new TickDirectionClientImpl(grpc::CreateChannel(ds_location, grpc::InsecureChannelCredentials())) );
	}

private:
	std::unique_ptr<TickDirectionClientImpl> ds_client;
	std::string server_address{"0.0.0.0"};
	std::string ds_location;
	int32_t current_price{0};
	int32_t current_size{0};
};

void TickServerImpl::Run() {

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

	// Delay connection to the Direction server until the first connection
	// comes in.

	// Wait for the server to shutdown. Note that some other thread must be
	// responsible for shutting down the server for this call to ever return.
	server->Wait();
}

int main( int argc, char **argv) {

	int32_t listen_port = 50081;
	int32_t initial_price{500};
	int32_t initial_size{100};
	std::string direction_loc{"localhost:50091"};

	struct option longopts[] = {
		{ "listen_port",    required_argument,   nullptr, 'l' },
		{ "direction_loc", required_argument,   nullptr, 'd' },
		{ "initial_price", required_argument,   nullptr, 'p' },
		{ "initial_size", required_argument,   nullptr, 's' },
		{ nullptr,  0,                  nullptr,  0 }
	};

	int32_t ch;
	while ((ch = getopt_long(argc, argv, "l:d:p:s:", longopts, NULL)) != -1) {
		switch(ch) {
			case 'l':
				listen_port = atoi(optarg);
				break;
			case 'd':
				direction_loc = optarg;
				break;
			case 'p':
				initial_price = atoi(optarg);
				break;
			case 's':
				initial_size = atoi(optarg);
				break;
			case '?':
			default:
				std::cerr << "Usage: " << argv[0] << " [-l listen_port] [-d direction_server_port] [-p initial_price] [-s initialia_size]" << "\n"
					<< "l   - Port to listen to for client requests. Default 50081" << "\n"
					<< "d   - Direction Server Location to connect to. Default localhost:50091" << "\n"
					<< "p   - Iniital Price. Default 500." << "\n"
					<< "s   - Iniital Size. Default 100." << "\n"
					<< std::endl;
				exit(-1);
		}
	}

	if (listen_port < 100) {
		std::cerr << "Server port not specified or invalid" << std::endl;
		exit(-2);
	}

	std::cout << "Listen Port: " << listen_port << "   Direction server location: " << direction_loc << "\n"
	          << "Initial Price: " << initial_price << "   Initial Size: " << initial_size << std::endl;

	// Create connection to TickDirectionServer
	//

	TickServerImpl server(listen_port, direction_loc, initial_price, initial_size);
	server.Run();

	return 0;
}
