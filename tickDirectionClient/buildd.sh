docker build . -f Dockerfile.build -t tick-client-exe:1.0.0
docker build . -f tickDirectionClient/Dockerfile.build -t tick-directionclient-exe:1.0.0 -t dgoveas/tick-directionclient-exe:v1
