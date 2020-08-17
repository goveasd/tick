#!/bin/bash

docker run -it -d --rm -p 50081:50081 --net=host --name tick-server dgoveas/tick-server-exe:v1 "-d" "localhost:10091"
