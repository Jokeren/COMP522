#!/usr/bin/perl

my $input = $ARGV[0];

#field columns
my $AffinityIdx = 0;
my $Pool = 1;
my $Producers = 2;
my $Consumers = 3;
my $InsertionThroughput = 4;
my $SystemThroughput = 5;
my $StealingAttempts = 6;
my $ProdCAS = 7;
my $ProdCASFail = 8;
my $ConsCAS = 9;
my $ConsCASFail = 10;
my $ForceAffinity = 11;
my $ProdMigrate = 12;
my $ProdFlucts = 13;
my $ConsFlucts = 14;
my $PausedThreads = 15;

my $fields = 16;

open(AVG_CSV, ">>avg.csv") || die("can't open avg.csv");
open(RAW_CSV, $input) || die("can't open $input");
print AVG_CSV "Pool Type,Producers,Consumers,ProdMigrate,Affinity,Insertion Throughput,System Throughput,Stealing Attempts,Prod CAS,Prod CAS failures,Cons CAS,Cons CAS failures\n";

my $count = 0;
my $affinity = 1000;
my $curMigrate = 1000;

#define as many lists as the number of iterations with the same set of config parameters. we use 5.
my @cols0;
my @cols1;
my @cols2;
my @cols3;
my @cols4;
my @allCols = (\@cols0, \@cols1, \@cols2, \@cols3, \@cols4);
my @cols;

sub calcAvg
{
	my $InsertionThroughputAvg = 0.0;
	my $SystemThroughputAvg = 0.0;
	my $StealingAttemptsAvg = 0.0;
	my $ProdCASAvg = 0.0;
	my $ProdCASFailAvg = 0.0;
	my $ConsCASAvg = 0.0;
	my $ConsCASFailAvg = 0.0;
	for($i = 0; $i < $count; $i++)
	{
		$InsertionThroughputAvg += $allCols[$i]->[$InsertionThroughput];
		$SystemThroughputAvg += $allCols[$i]->[$SystemThroughput];
		$StealingAttemptsAvg += $allCols[$i]->[$StealingAttempts];
		$ProdCASAvg += $allCols[$i]->[$ProdCAS];
		$ProdCASFailAvg += $allCols[$i]->[$ProdCASFail];
		$ConsCASAvg += $allCols[$i]->[$ConsCAS];
		$ConsCASFailAvg += $allCols[$i]->[$ConsCASFail];
	}
	$InsertionThroughputAvg = $InsertionThroughputAvg/$count;
	$SystemThroughputAvg = $SystemThroughputAvg/$count;
	$StealingAttemptsAvg = $StealingAttemptsAvg/$count;
	$ProdCASAvg = $ProdCASAvg/$count;
	$ProdCASFailAvg = $ProdCASFailAvg/$count;
	$ConsCASAvg = $ConsCASAvg/$count;
	$ConsCASFailAvg = $ConsCASFailAvg/$count;
	print AVG_CSV "$cols0[$Pool],$cols0[$Producers],$cols0[$Consumers],$cols0[$ProdMigrate],$cols0[$AffinityIdx],$InsertionThroughputAvg,$SystemThroughputAvg,$StealingAttemptsAvg,$ProdCASAvg,$ProdCASFailAvg,$ConsCASAvg,$ConsCASFailAvg\n";
}


while($line = <RAW_CSV>)
{
	chomp($line);
	next if($line eq "");
	@cols = split(',',$line);
	if((($cols[$AffinityIdx] != $affinity) || ($curMigrate != $cols[$ProdMigrate])) && ($count > 0))
	{
		calcAvg();
		$count = 0;
	}
	for($i = 0; $i < $fields; $i++)
	{
		$allCols[$count]->[$i] = $cols[$i];
	}
	$count = $count + 1;
	$affinity = $cols[$AffinityIdx];
	$curMigrate = $cols[$ProdMigrate];
}
calcAvg();

close(AVG_CSV);
close(RAW_CSV);
