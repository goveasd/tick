mkdir -p /tmp/consul/data
chmod 777 /tmp/consul/data

#consul agent -server -dev -ui -booststrap-expect 1 -bind 172.20.20.41 -client 0.0.0.0 -data-dir /tmp/consul/data &
consul agent -server -dev -ui -bind 172.20.20.41 -client 0.0.0.0 -data-dir /tmp/consul/data -log-level trace -log-file /tmp/consul/log/ &
#consul agent -dev -bind 172.20.20.21 -client 0.0.0.0 &
