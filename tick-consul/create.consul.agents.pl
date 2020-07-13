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

# hostname sometime shows up as macbook-pro.lan and other times as MacBook-Pro.local
my $hlocation = hostname();
my $location = index(uc($hlocation), "MACBOOK") == 0 ?  "local-node" : "$hlocation";

print "Location: $location\n";


die "Valid Location is needed: @map_keys\n" unless defined $location and exists($mappings{$location});

my $IP = $mappings{$location};
print "IP:$IP\n";


# Clean any previous tmp files
unlink glob "/tmp/tmpfile*";

my ($fh, $tmpfilename ) = mkstemps( "/tmp/tmpfileXXXXXX", '.json');

my $built_json = 
"
    {
        \"advertise_addr\": \"$IP\",
        \"retry_join\": [\"172.20.20.31\"],
        \"data_dir\": \"/tmp/consul\",
	\"client_addr\": \"0.0.0.0\"
    }
";

my $built_ln_json = 
"
    {
        \"ui\": true,
        \"advertise_addr\": \"$IP\",
        \"retry_join\": [\"172.20.20.31\"],
        \"data_dir\": \"/tmp/consul\"
    }
";

my $use_json = $location eq "local-node" ? $built_ln_json : $built_json;

print "File=$tmpfilename Built: $use_json\n";

print $fh "$use_json\n";
close($fh);


my $hasConfigFile = undef();

my $cmdToRun = "consul agent -config-file $tmpfilename ";
if (index($location, "web") == 0) {
	$hasConfigFile = "web.service.json";
	$cmdToRun .= " -config-file $hasConfigFile";
} elsif (index($location, "lb") == 0) {
	$hasConfigFile = "lb.service.json";
	$cmdToRun .= " -config-file $hasConfigFile";
}

$cmdToRun .= " &";

# Test that provided Config is valid
if (defined $hasConfigFile) {
	my $checkCmdToRun = "consul validate $hasConfigFile";

	print "Running Config test: $checkCmdToRun\n";
	system("$checkCmdToRun") || die "Config $hasConfigFile is Invalid";
}

system("$cmdToRun") || die "Failed to run system call";

print "Created agent on $hlocation!!\n";
