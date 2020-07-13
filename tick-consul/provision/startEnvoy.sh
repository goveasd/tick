# 9901 - Admin Port
# 9101 - gRPC-Web Client Port
docker run -d --name envoy -p 9901:9901 -p 9101:9101 envoy:v1
