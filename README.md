# tick
First cut gRPC based tick server/client. Also installs proto/tick/server into docker containers

Notes:
When building ngix server and TLS on OS/X, make sure you uninstall "brew unistall binutils". You know you haven't done so if a build of tls results in link errors and a warning about ranlib.
