#!/bin/bash
docker build . -t envoy_lb:v1 -f Dockerfile.envoy.lb
