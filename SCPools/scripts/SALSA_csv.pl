#!/usr/bin/perl
$dir = $ARGV[0];
$iterations = $ARGV[1];

opendir(IMD, $dir) || die("Cannot open directory $dir"); 
@thefiles = readdir(IMD);
closedir(IMD);
open(CSV,">>results.csv") || die "$!";

sub printListToCSV
{
	my ($name, $listref) = @_;
	my (@list) = @$listref;
	print CSV "\n$name,";
	$sum = 0.0;
	foreach $val(@list){print CSV "$val,"; $sum += $val;}
	$average = $sum/$iterations;
	print CSV ",$average,";
}

foreach $f(@thefiles)
{
	if($f =~ /\.txt/)
	{
		# experiments' results lists - need to add a list for each result metic we what to test
		my @InsertedTasks;
		my @InsertionThroughput;
		my @RetrievedTasks;
		my @SystemThroughput;
		my @StealingAttempts;
		my @ProdCASoperations;
		my @ProdCASfailures;
		my @ProdFetchAndInc;
		my @ConsCASoperations;
		my @ConsCASfailures;
		my @ConsFetchAndInc;
		my $prod = 1;
		
		open(FH, "$dir/$f") || die("Cannot open file $f");
		while($line = <FH>)
		{
			chomp($line);
			if($line =~ /^Total number of inserted tasks = (.*)/)
			{	
				push(@InsertedTasks, $1);
			}
			if($line =~ /^Peak Insertion throughput = (.*)/)
			{	
				push(@InsertionThroughput, $1);
			}
			if($line =~ /^Total Number of retrieved tasks = (.*)/)
			{	
				push(@RetrievedTasks, $1);
			}
			if($line =~ /^System Throughput = (.*)/)
			{	
				push(@SystemThroughput, $1);
			}
			if($line =~ /^Average Number of Work Stealing Attempts per Consumer = (.*)/)
			{	
				push(@StealingAttempts, $1);
			}
			if($line =~ /^Average Producer Atomic Statistics/)
			{	
				$prod = 1;
			}
			if($line =~ /^Average Consumer Atomic Statistics/)
			{	
				$prod = 0;
			}
			if($line =~ /^CAS operations: (\d*)/)
			{
				if($prod == 1)
				{
					push(@ProdCASoperations,$1);
				}
				else
				{
					push(@ConsCASoperations,$1);
				}
			}
			if($line =~ /^CAS failures: (\d*)/)
			{
				if($prod == 1)
				{
					push(@ProdCASfailures,$1);
				}
				else
				{
					push(@ConsCASfailures,$1);
				}
			}
			if($line =~ /^FetchAndIncDec operations: (\d*)/)
			{
				if($prod == 1)
				{
					push(@ProdFetchAndInc,$1);
				}
				else
				{
					push(@ConsFetchAndInc,$1);
				}
			}
		}
		close(FH);	
		#write results to csv file
		print CSV "$f:";
		printListToCSV("Inserted Tasks", \@InsertedTasks);
		printListToCSV("Insertion Throughput", \@InsertionThroughput);
		printListToCSV("Retrieved Tasks", \@RetrievedTasks);
		printListToCSV("System Throughput", \@SystemThroughput);
		printListToCSV("Stealing Attempts Per Consumer", \@StealingAttempts);
		print CSV "\nAverage Producer Atomic Statistics:";
		printListToCSV("CAS Operations", \@ProdCASoperations);
		printListToCSV("CAS Failures", \@ProdCASfailures);
		printListToCSV("FetchAndInc\Dec Operations", \@ProdFetchAndInc);
		print CSV "\nAverage Consumer Atomic Statistics:";
		printListToCSV("CAS Operations", \@ConsCASoperations);
		printListToCSV("CAS Failures", \@ConsCASfailures);
		printListToCSV("FetchAndInc\Dec Operations", \@ConsFetchAndInc);
		print CSV "\n\n";
	}
}

