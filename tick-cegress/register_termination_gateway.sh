#!/bin/bash

# Write directly to consul-server. this way we dont have to start a local agent
#consul config write -http-addr="172.20.20.41:8500"  termination_gateway.hcl 
consul config write termination_gateway.hcl 
