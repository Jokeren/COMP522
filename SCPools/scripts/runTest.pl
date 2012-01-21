#!/usr/bin/perl

use Getopt::Long;

my $force="no";
my $cons = 1;
my $prod = 1;
my $poolType = "NoFIFOPool";
my $fileName = "SCPools";
my $prodFluct="false";
my $consFluct="false";
my $pausedThreads=0;
my $prodMigrate="true";

GetOptions( "consumers=i" => \$cons,
	    "producers=i" => \$prod,
	    "poolType=s"  => \$poolType,
	    "file=s"  => \$fileName,
	    "force=s" => \$force,
		"prodFluct=s" => \$prodFluct,
		"consFluct=s" => \$consFluct,
		"pausedThreads=i" => \$pausedThreads,
		"prodMigrate=s" => \$prodMigrate
		);

$conf = <<END;
#force affinity ?
force=$force
#if forced affinity - producers/comsumers thread cores
cons=0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
prods=16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
chips=0 8 16 24:1 9 17 25:2 10 18 26:3 11 19 27:4 12 20 28:5 13 21 29:6 14 22 30:7 15 23 31

#number of producer/consumer threads
producersNum=$prod
consumersNum=$cons

#type of container
poolType=$poolType

#chunk pool size
initialPoolSize=1000

#Producers input intervals
peakLength=1000000
timeBetweenPeaks=0

#Test runtime
timeToRun=20000

#Num of attempts before going to steal
stealIterations=1

#Fluctuations on/off
prodFluctuations=$prodFluct
consFluctuations=$consFluct
pausedThreads=$pausedThreads

#Producer migration on/off
producerMigrate=$prodMigrate

END

open (OUT, '>tmpConf');
print OUT $conf;
close OUT;

my $res = `./$fileName ./tmpConf`;
#print $res;
$res =~ /CSV:([A-Za-z0-9-,\.]+)/;
print $1 . "\n"; 

unlink("./tmpConf");

