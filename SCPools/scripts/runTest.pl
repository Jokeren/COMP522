#!/usr/bin/perl

use Getopt::Long;

my $force="no";
my $consNum = 1;
my $prodNum = 1;
my $poolType = "NoFIFOPool";
my $fileName = "SCPools";
my $prodFluct="false";
my $consFluct="false";
my $prodDisable=0;
my $consDisable=0;
my $pausedThreads=0;
my $prodMigrate="true";
my $prodCores="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15";
my $consCores="16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31";
my $chips="0 8 16 24:1 9 17 25:2 10 18 26:3 11 19 27:4 12 20 28:5 13 21 29:6 14 22 30:7 15 23 31";
my $affinity_str="x";
my @affinity;
my $affinityIdx="0";

GetOptions( "consumers=i" => \$consNum,
	    "producers=i" => \$prodNum,
	    "poolType=s"  => \$poolType,
	    "file=s"  => \$fileName,
	    "force=s" => \$force,
		"prodFluct=s" => \$prodFluct,
		"consFluct=s" => \$consFluct,
		"pausedThreads=i" => \$pausedThreads,
		"prodMigrate=s" => \$prodMigrate,
		"prodDisable=i" => \$prodDisable,
		"consDisable=i" => \$consDisable,
		"affinity=s" => \$affinity_str,
		"affinityIdx=s" => \$affinityIdx
		);
		
if($affinity_str ne "x")
{
	@affinity = split(/\*/,$affinity_str);
	$prodCores = $affinity[0];
	$consCores = $affinity[1];
	$chips = $affinity[2];
	$prodCores =~ s/_/ /g;
	$consCores =~ s/_/ /g;
	$chips =~ s/_/ /g;
}

$conf = <<END;
#force affinity ?
forceAssignment=$force
#if forced affinity - producers/comsumers thread cores
prods=$prodCores
cons=$consCores
chips=$chips

#number of producer/consumer threads
producersNum=$prodNum
consumersNum=$consNum

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

# disable threads
prodDisable=$prodDisable
consDisable=$consDisable

END

open (OUT, '>tmpConf');
print OUT $conf;
close OUT;

my $res = `./$fileName ./tmpConf`;
#print $res;
$res =~ /CSV:([A-Za-z0-9-\+,\.]+)/;
print "$affinityIdx,$1\n"; 

unlink("./tmpConf");

