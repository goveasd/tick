#!/usr/bin/perl

use strict;
use warnings;
use Data::Dumper qw(Dumper);

use File::Temp qw/ :mktemp /;
use File::Path qw(make_path);

use Socket;
use Sys::Hostname;

# TODO: Can use ifconfig and look for eth1 ip
my %mappings = ('local-node' => '172.20.20.1',
                'egress-server' => '172.20.20.51',
                'tick1'       => '172.20.20.21',
                'tick2'       => '172.20.20.22',
                'direction1'  => '172.20.20.31',
                'direction2'  => '172.20.20.32');

my (@map_keys) = keys %mappings;

#my ($location) = shift(@ARGV);

# hostname sometime shows up as macbook-pro.lan and other times as MacBook-Pro.local
my $hlocation = hostname();

my $location = (index(uc($hlocation), "MACBOOK") == 0 or index(uc($hlocation), "MACPRO") == 0) ?  "local-node" : "$hlocation";

print "Location: $location\n";


die "Valid Location is needed: @map_keys\n" unless defined $location and exists($mappings{$location});

my $IP = $mappings{$location};
print "IP:$IP\n";

make_path('/tmp/consul/data', '/tmp/consul/log', '/tmp/envoy/log', {verbose => 1, mode => 0777});

# Clean any previous tmp files
unlink glob "/tmp/tmpfile*";

my ($fh, $tmpfilename ) = mkstemps( "/tmp/tmpfileXXXXXX", '.json');

my $built_json = 
"
    {
        \"advertise_addr\": \"$IP\",
        \"retry_join\": [\"172.20.20.41\"],
        \"data_dir\": \"/tmp/consul/data\",
        \"client_addr\": \"0.0.0.0\"
    }
";

my $built_ln_json = 
"
    {
        \"advertise_addr\": \"$IP\",
        \"retry_join\": [\"172.20.20.41\"],
        \"data_dir\": \"/tmp/consul/data\",
        \"client_addr\": \"0.0.0.0\"
    }
";

my $use_json = $location eq "local-node" ? $built_ln_json : $built_json;

print "File=$tmpfilename Built: $use_json\n";

print $fh "$use_json\n";
close($fh);


my $hasConfigFile = undef();

my $cmdToRun = "consul agent -config-file $tmpfilename ";
if (index($location, "tick") == 0) {
	$hasConfigFile = "tick.service.json";
} elsif (index($location, "direction") == 0 || index($location, "local") == 0) {
	$hasConfigFile = "direction.service.json";
}

$cmdToRun .= " -grpc-port 8502 -log-level trace -log-file /tmp/consul/log/";
if (defined $hasConfigFile) {
   $cmdToRun .= " -config-file $hasConfigFile";
}

$cmdToRun .= " &";

# Test that provided Config is valid
if (defined $hasConfigFile) {
	my $checkCmdToRun = "consul validate $hasConfigFile";

	print "Running Config test: $checkCmdToRun\n";
	system("$checkCmdToRun") || die "Config $hasConfigFile is Invalid";
}

my $startEnvoy = "xx";
#my $startEnvoy = undef;
if (defined $startEnvoy and not defined $hasConfigFile) {
	$startEnvoy = undef();
}

print "Executing ... $cmdToRun\n";
if (defined $startEnvoy) {
	print "Will start ENVOY ..\n";
}

system("$cmdToRun"); // || die "Failed to run system call";
##
print "Created agent on $hlocation!!\n";

if (defined $startEnvoy) {
	print "\n\n          Sleeping a bit to let Consul get setup before starting sidecar proxy\n\n";
	sleep(30);

	my $service = (index($location, "tick") == 0) ? "tick" : "direction";

	my $proxyCmd = "consul connect envoy -sidecar-for $service -grpc-addr=127.0.0.1:8502 -http-addr http://127.0.0.1:8500 -admin-bind 0.0.0.0:19000 -- --log-level debug --log-path /tmp/envoy/log/envoy.log &";
	#my $proxyCmd = "consul connect envoy -sidecar-for $service -grpc-addr=$IP:8502 -http-addr http://$IP:8500 -admin-bind 0.0.0.0:19000 -- --log-level trace --log-path /tmp/envoy/log/envoy.log &";

	print "\n\n\n\n\n\nRunning Proxy Creation  $proxyCmd\n\n\n\n\n\n\n";
	system("$proxyCmd") || die "Creation of Proxy failed";
	print "Created Proxy for Service: $service\n";
} else {
		print "\n\n  Not starting envoy proxy\n\n";
}



