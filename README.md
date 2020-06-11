# tick
First cut gRPC based tick server/client. Also installs proto/tick/server into docker containers

Notes:
When building ngix server and TLS on OS/X, make sure you uninstall "brew unistall binutils". You know you haven't done so if a build of tls results in link errors and a warning about ranlib.

Download nginx
mkdir ~/nginx
curl -OL http://nginx.org/download/nginx-1.18.0.tar.gz
tar -xvzf nginx-1.18.0.tar.gz && rm nginx-1.18.0.tar.gz

Download openSsl
mkdir ~/openssl
curl --remote-name https://www.openssl.org/source/openssl-1.1.1a.tar.gz
tar xvzf openssl-1.1.0g.tar.gz && rm openssl-1.1.0g.tar.gz 


./configure --http-client-body-temp-path=/tmp/cache/nginx/client_temp
--http-proxy-temp-path=/tmp/cache/nginx/proxy_temp
--http-fastcgi-temp-path=/tmp/cache/nginx/fastcgi_temp
--http-uwsgi-temp-path=/tmp/cache/nginx/uwsgi_temp
--http-scgi-temp-path=/tmp/cache/nginx/scgi_temp 
--with-http_realip_module
--with-http_addition_module
--with-http_sub_module
--with-http_dav_module
--with-http_flv_module
--with-http_mp4_module
--with-http_gunzip_module
--with-http_gzip_static_module
--with-http_random_index_module
--with-http_secure_link_module
--with-http_stub_status_module
--with-http_auth_request_module
--with-threads
--with-stream
--with-http_slice_module
--with-mail
--with-http_v2_module
--with-ipv6 
--with-http_ssl_module
--with-stream_ssl_module
--with-mail_ssl_module
--with-openssl=/locationOfOppenSslSource/openssl
