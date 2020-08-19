#!/bin/bash

ENVOY_VER=1.14.2

curl -L https://getenvoy.io/cli | bash -s -- -b /home/vagrant/

/home/vagrant/getenvoy fetch standard:${ENVOY_VER} 

sudo mv /home/vagrant/.getenvoy/builds/standard/${ENVOY_VER}/linux_glibc/bin/envoy /usr/local/bin/
rm -rf /home/vagrant/.getenvoy/
rm -rf /home/vagrant/getenvoy
