#!/usr/bin/env bash

sudo apt-get update
sudo apt-get install -y unzip

CONSUL_VERSION=1.7.4

curl https://releases.hashicorp.com/consul/${CONSUL_VERSION}/consul_${CONSUL_VERSION}_linux_386.zip -o consul.zip
unzip consul.zip

sudo chmod +x consul

sudo mv consul /usr/bin/consul

rm consul.zip
