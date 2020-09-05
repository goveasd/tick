#!/bin/bash

#docker run -it -d --rm -p 50081:50081 --net=host --name tick-server dgoveas/tick-server-exe:v1 "-d" "localhost:10091"
#docker run -it -d --rm -p 50081:50081 --net=host --name tick-server dgoveas/tick-server-exe:v1 "-l" "50081" "-d" "localhost:10091"

docker run -it -d --rm -p 50081:50081 --name tick-server dgoveas/tick-server-exe:v1 "-l" "50081" "-d" "172.20.20.21:10091"

#docker run -it -d --rm -p 50081:50081 --name tick-server dgoveas/tick-server-exe:v1 "-l" "50081" "-d" "0.0.0.0:10091"
#docker run -it -d --rm -p 50081:50081 --name tick-server dgoveas/tick-server-exe:v1 "-d" "localhost:10091"
