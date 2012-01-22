#!/usr/bin/perl
$inputDir = $ARGV[0];
$outputDir = $ARGV[1];
$count = $ARGV[2];

opendir(IMD, $inputDir) || die("Cannot open directory $inputDir"); 
@thefiles = readdir(IMD);
closedir(IMD);

foreach $f (@thefiles) {
	if($f =~ /\.txt/) {
		for($i=0; $i < $count; $i++ ) {
			print "Running $inputDir/$f test\n";
			$redirect = $i > 0 ? ">>" : ">";
			# make sure executable's name is "SALSA"
			system("./SALSA $inputDir/$f $redirect $outputDir/$f.out");
		}
	}
}
