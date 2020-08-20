#!/bin/bash

#consul connect envoy -gateway=terminating -register -service legacy-direction-service-gateway -address 169.254.30.22:8443
#consul connect envoy -gateway=terminating -register -service legacy-direction-service-gateway -address 172.20.20.1:8443
#consul connect envoy -gateway=terminating -register -service legacy-direction-service-gateway -address 10.0.2.15:8443
consul connect envoy -gateway=terminating -register -service legacy-direction-service-gateway -address 172.20.20.51:8443 -grpc-addr=127.0.0.1:8502 -- -l debug
