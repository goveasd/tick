#!/usr/bin/perl

use strict;
use warnings;
use Data::Dumper qw(Dumper);

use File::Temp qw/ :mktemp /;
use Socket;
use Sys::Hostname;

# TODO: Can use ifconfig and look for eth1 ip
my %mappings = ('local-node' => '172.20.20.1',
                'web1'       => '172.20.20.21',
                'web2'       => '172.20.20.22',
                'web3'       => '172.20.20.23',
                'lb'         => '172.20.20.11');

my (@map_keys) = keys %mappings;

#my ($location) = shift(@ARGV);

my $hlocation = hostname();
my $location = $hlocation eq "macbook-pro.lan" ?  "local-node" : "$hlocation";

print "Location: $location\n";


die "Valid Location is needed: @map_keys\n" unless defined $location and exists($mappings{$location});

my $IP = $mappings{$location};
print "IP:$IP\n";


my $nginx_inp_file = "/home/vagrant/nginx.html";
if (! -e $nginx_inp_file) {
	open my $nginx_FILE, "+>", $nginx_inp_file or die "Couldn't open nginx serveup file $nginx_inp_file, $!";
	print $nginx_FILE "<h1>We are on $location with IP $IP</h1>\n";

	close $nginx_FILE;
}


# Run nginx via docker
# mount the nginx.html page as a volume in nginx html dir
my $nginx_conf_file = "/vagrant/nginx/tick.html";
my $nginx_conf1_file = "/vagrant/nginx/conf/nginx.conf";
my $nginx_conf2_file = "/vagrant/nginx/conf/nginx.virtual.conf";
my $nginx_conf3_file = "/vagrant/nginx/conf/nginx.mime.conf";
my $nginx_confJs_file = "/vagrant/nginx/dist/main.js";
my $docker_cmd =
"docker run -d --name tick-web -p 8080:80 -p 9101:9101 -p 9002:9002 -p 8081:8081 -p 9001:9001 --restart unless-stopped -v $nginx_inp_file:/usr/share/nginx/html/index.html:ro -v $nginx_conf_file:/usr/share/nginx/conf/tick.html -v $nginx_conf1_file:/usr/share/nginx/conf/nginx.conf:ro -v $nginx_conf2_file:/usr/share/nginx/conf/nginx.virtual.conf:ro -v $nginx_conf3_file:/usr/share/nginx/conf/nginx.mime.conf:ro -v $nginx_confJs_file:/usr/share/nginx/conf/dist/main.js nginx -c /usr/share/nginx/conf/nginx.conf";
system("$docker_cmd") 
