#!/bin/bash
   				  
for poolType in "NoFIFOCASPool" "LIFOPool" "NoFIFOPool" "MSQTaskPool" ; do
	for t in 16; do
		echo "$poolType"
		for prodMigrate in "true" "false"; do		
		
			for aff1 in "0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15*25_16_24_17_27_18_26_19_29_20_28_21_31_22_30_23*0_8_16_24:1_9_17_25:2_10_18_26:3_11_19_27:4_12_20_28:5_13_21_29:6_14_22_30:7_15_23_31"; do
			echo "unfair mapping"
			for i in `seq 5`; do 
				perl runTest.pl --consumers=$t --producers=$t --poolType=$poolType --force="yes" --prodMigrate=$prodMigrate --affinity=$aff1 --affinityIdx="1" >> res_unfair.csv
			done
			done
		
			for aff2 in "0_1_2_3_4_5_6_7_8_9_10_11_12_13_14_15*16_17_18_19_20_21_22_23_24_25_26_27_28_29_30_31*0_8_16_24:1_9_17_25:2_10_18_26:3_11_19_27:4_12_20_28:5_13_21_29:6_14_22_30:7_15_23_31"; do
			echo "Good mapping - prod fluct"
			for i in `seq 5`; do 
				perl runTest.pl --consumers=$t --producers=$t --poolType=$poolType --force="yes" --prodFluct="true" --pausedThreads="8" --prodMigrate=$prodMigrate --affinity=$aff2 --affinityIdx="2" >> res_unfair.csv
			done
			done
			
			for aff3 in "0_1_2_3_4_5_6_7_8_9_10_11_12_13_14_15*16_17_18_19_20_21_22_23_24_25_26_27_28_29_30_31*0_8_16_24:1_9_17_25:2_10_18_26:3_11_19_27:4_12_20_28:5_13_21_29:6_14_22_30:7_15_23_31"; do
			echo "Good mapping - cons fluct"
			for i in `seq 5`; do 
				perl runTest.pl --consumers=$t --producers=$t --poolType=$poolType --force="yes" --consFluct="true" --pausedThreads="8" --prodMigrate=$prodMigrate --affinity=$aff3 --affinityIdx="3" >> res_unfair.csv
			done
			done
		
		done 
	done
done
