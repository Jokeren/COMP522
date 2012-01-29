#!/bin/bash
   				  
for poolType in "NoFIFOCASPool" "LIFOPool" "NoFIFOPool" "MSQTaskPool" "SwedishPool"; do
	for t in 16; do
		echo "$poolType"
		for disable in 0 1 4; do
			for prodMigrate in "true" "false"; do		
		
				for aff in "28_29_30_31_16_17_18_19_20_21_22_23_24_25_26_27*0_1_2_3_4_5_6_7_8_9_10_11_12_13_14_15*0_8_16_24:1_9_17_25:2_10_18_26:3_11_19_27:4_12_20_28:5_13_21_29:6_14_22_30:7_15_23_31"; do
				for i in `seq 5`; do 
					perl runTest.pl --consumers=$t --producers=$t --poolType=$poolType --force="yes" --prodMigrate=$prodMigrate --prodDisable=$disable --consDisable=$disable --affinity=$aff --affinityIdx=$disable >> res_2901.csv
				done
				done
				
			done
		done 
	done
done
