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

class TickServerImpl final : public Price::Tob::Service {

public:
	enum class Direction : char  {
		UP = 'u',
		DOWN = 'd'
	};

	TickServerImpl(Direction side, int32_t listen_on)
		: direction(side) {
		
		server_address.append(":").append(std::to_string(listen_on));
	}

	Status CurrentPrice(ServerContext * context,
                        const Price::TickRequest * request,
                        Price::TickDelta * reply) override {
		++count;

		reply->set_delta_price(count%2 == 0 ? 1 : -1);
		reply->set_delta_size(count%2 == 0 ? 1 : -1);

		return Status::OK;
	}

	void Run();

private:
	Direction direction{Direction::UP};
	std::string server_address{"0.0.0.0"};
	uint32_t count{0};
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

	// Wait for the server to shutdown. Note that some other thread must be
	// responsible for shutting down the server for this call to ever return.
	server->Wait();
}

int main( int argc, char **argv) {

	TickServerImpl::Direction direction = TickServerImpl::Direction::DOWN;
	int32_t listen_port = 50081;

	struct option longopts[] = {
		{ "direction",      required_argument,   nullptr, 'd' },
		{ "listen_port",    required_argument,   nullptr, 'l' },
		{ nullptr,  0,                  nullptr,  0 }
	};

	int32_t ch;
	while ((ch = getopt_long(argc, argv, "d:l:", longopts, NULL)) != -1) {
		switch(ch) {
			case 'd':
				if (strcasecmp(optarg, "UP") == 0)
					direction = TickServerImpl::Direction::UP;
				break;
			case 'l':
				listen_port = atoi(optarg);
				break;
			case '?':
			default:
				std::cerr << "Usage: " << argv[0] << " [-d [UP|DOWN]] [-l listen_port]" << "\n"
					<< "d   - Direction Up or Down 1 cent. Default Down" << "\n"
					<< "l   - Port to listen to for client requests" << "\n"
					<< std::endl;
				exit(-1);
		}
	}

	std::cout << "Listen Port: " << listen_port << " Direction=" << static_cast<char>(direction) << std::endl;

	TickServerImpl server(direction, listen_port);
	server.Run();

	return 0;
}
