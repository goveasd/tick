#!/bin/bash

docker run -it -d --rm -p 50091:50091 --net=host --name tick-directionserver dgoveas/tick-directionserver-exe:v1 
