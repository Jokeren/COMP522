#!/bin/bash
   				  
for poolType in "NoFIFOCASPool" "LIFOPool" "NoFIFOPool"  "MSQTaskPool"; do
	for p in 8; do
		for c in 24; do
			echo "$poolType,$p,$c"
			for prodMigrate in "true" "false"; do
		
				for aff1 in "0_1_2_3_4_5_6_7*8_9_10_11_12_13_14_15_16_17_18_19_20_21_22_23_24_25_26_27_28_29_30_31*0_8_16_24:1_9_17_25:2_10_18_26:3_11_19_27:4_12_20_28:5_13_21_29:6_14_22_30:7_15_23_31"; do
				echo "Affinity 1"
				for i in `seq 5`; do 
					perl runTest.pl --consumers=$c --producers=$p --poolType=$poolType --force="yes" --prodMigrate=$prodMigrate --affinity=$aff1 --affinityIdx="5" >> res_asymmetric.csv
				done
				done
		
				for aff2 in "0_1_2_3_8_9_10_11*4_5_6_7_12_13_14_15_16_17_18_19_20_21_22_23_24_25_26_27_28_29_30_31*0_8_16_24:1_9_17_25:2_10_18_26:3_11_19_27:4_12_20_28:5_13_21_29:6_14_22_30:7_15_23_31"; do
				echo "Affinity 2"
				for i in `seq 5`; do 
					perl runTest.pl --consumers=$c --producers=$p --poolType=$poolType --force="yes" --prodMigrate=$prodMigrate --affinity=$aff2 --affinityIdx="6" >> res_asymmetric.csv
				done
				done
				
				echo "no affinity"
				for i in `seq 5`; do 
					perl runTest.pl --consumers=$c --producers=$p --poolType=$poolType --force="no" --prodMigrate=$prodMigrate --affinityIdx="0" >> res_asymmetric.csv
				done
			done
		done
	done
		
	for p in 24; do
		for c in 8; do
			echo "$poolType,$p,$c"
			for prodMigrate in "true" "false"; do
				
				for aff3 in "8_9_10_11_12_13_14_15_16_17_18_19_20_21_22_23_24_25_26_27_28_29_30_31*0_1_2_3_4_5_6_7*0_8_16_24:1_9_17_25:2_10_18_26:3_11_19_27:4_12_20_28:5_13_21_29:6_14_22_30:7_15_23_31"; do
				echo "Affinity 3"
				for i in `seq 5`; do 
					perl runTest.pl --consumers=$c --producers=$p --poolType=$poolType --force="yes" --prodMigrate=$prodMigrate --affinity=$aff3 --affinityIdx="7" >> res_asymmetric.csv
				done
				done
		
				for aff4 in "4_5_6_7_12_13_14_15_16_17_18_19_20_21_22_23_24_25_26_27_28_29_30_31*0_1_2_3_8_9_10_11*0_8_16_24:1_9_17_25:2_10_18_26:3_11_19_27:4_12_20_28:5_13_21_29:6_14_22_30:7_15_23_31"; do
				echo "Affinity 4"
				for i in `seq 5`; do 
					perl runTest.pl --consumers=$c --producers=$p --poolType=$poolType --force="yes" --prodMigrate=$prodMigrate --affinity=$aff4 --affinityIdx="8" >> res_asymmetric.csv
				done
				done
		
				echo "no affinity"
				for i in `seq 5`; do 
					perl runTest.pl --consumers=$c --producers=$p --poolType=$poolType --force="no" --prodMigrate=$prodMigrate --affinityIdx="0"  >> res_asymmetric.csv
				done
			done 
		done
	done
done
