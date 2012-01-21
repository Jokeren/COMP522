#!/bin/bash

for poolType in "MSQTaskPool" "NoFIFOPool" "NoFIFOCASPool"; do
	for prodMigrate in "true" "false"; do
	for t in 1 2 4 8 16; do
		echo "$poolType,$t,$t"
		for i in `seq 5`; do 
			./runTest.pl --consumers=$t --producers=$t --poolType=$poolType --prodMigrate=$prodMigrate >> res_21_01.csv
		done
	done 
	done
done

for poolType in "MSQTaskPool" "NoFIFOPool" "NoFIFOCASPool"; do
	for prodMigrate in "true" "false"; do
	for prod in 1 2 4 8; do
		echo "$poolType,$prod,16"
		for i in `seq 5`; do 
			./runTest.pl --consumers=16 --producers=$prod --poolType=$poolType --prodMigrate=$prodMigrate >> res_21_01.csv
		done
	done
	done
done

for poolType in "MSQTaskPool" "NoFIFOPool" "NoFIFOCASPool"; do
	for prodMigrate in "true" "false"; do
	for pausedThreads in 1 2 4 8; do
		echo "$poolType,16,16"
		for i in `seq 5`; do 
			./runTest.pl --consumers=16 --producers=16 --poolType=$poolType --prodMigrate=$prodMigrate --consFluct="true" --pausedThreads=$pausedThreads >> res_21_01.csv
		done
	done
	done
done
