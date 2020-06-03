x=`docker image ls | grep tick-server-exe | awk '{print $3}'`
echo == Server Image ID $x ==
docker container run -it --entrypoint /bin/bash $x
