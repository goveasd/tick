x=`docker image ls | grep tick-client-exe | awk '{print $3}'`
echo == Client Image ID $x ==
docker container run -it --entrypoint /bin/bash $x
