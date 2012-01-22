#!/usr/bin/perl
foreach $dir (@ARGV) {
	$outDir = $dir;
	$outDir =~ s/input/output/;
	$iterations = 5;
	system("mkdir -p $outDir");
	system("perl SALSA_test.pl $dir $outDir $iterations");
	system("perl SALSA_csv.pl $outDir $iterations");
}
