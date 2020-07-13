# 8600 is the dns port printed when the local-host agent was started up
echo "**** consul-server.node.consul ****"
dig @localhost -p 8600 consul-server.node.consul

echo "**** consul.service.consul - only get IP ****"
dig @localhost -p 8600 consul.service.consul

echo "**** consul.service.consul SRV Get IP and Port of services (Server is a service - listens on port 8300) ****"
dig @localhost -p 8600 consul.service.consul SRV

echo "**** web.service.consul SRV Get IP and Port of services (Server is a service - listens on port 8080) ****"
dig @localhost -p 8600 web.service.consul SRV
