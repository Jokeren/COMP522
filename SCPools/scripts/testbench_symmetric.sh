#!/bin/bash
   				  
for poolType in "NoFIFOCASPool" "LIFOPool" "NoFIFOPool" "MSQTaskPool" ; do
	for prodMigrate in "true" "false"; do
		for t in 1 2 4 8 16 32; do
			echo "$poolType,$t,$t"
		
			for aff1 in "0_8_1_9_4_5_6_7_2_10_3_11_12_13_14_15*16_24_17_25_18_26_19_27_20_28_21_29_22_30_23_31*0_8_16_24:1_9_17_25:2_10_18_26:3_11_19_27:4_12_20_28:5_13_21_29:6_14_22_30:7_15_23_31"; do
			echo "Affinity 1"
			for i in `seq 5`; do 
				perl runTest.pl --consumers=$t --producers=$t --poolType=$poolType --force="yes" --prodMigrate=$prodMigrate --affinity=$aff1 >> res_25_01.csv
			done
			done
		
			for aff2 in "0_1_2_3_4_5_6_7_8_9_10_11_12_13_14_15*16_17_18_19_20_21_22_23_24_25_26_27_28_29_30_31*0_8_16_24:1_9_17_25:2_10_18_26:3_11_19_27:4_12_20_28:5_13_21_29:6_14_22_30:7_15_23_31"; do
			echo "Affinity 2"
			for i in `seq 5`; do 
				perl runTest.pl --consumers=$t --producers=$t --poolType=$poolType --force="yes" --prodMigrate=$prodMigrate --affinity=$aff2 >> res_25_01.csv
			done
			done
		
			for aff3 in "0_16_2_18_4_20_6_22_8_24_10_26_12_28_14_30*1_17_3_19_5_21_7_23_9_25_11_27_13_29_15_31*0_1_16_17:2_3_18_19:4_5_20_21:6_7_22_23:8_9_24_25:10_11_26_27:12_13_28_29:14_15_30_31"; do
			echo "Affinity 3"
			for i in `seq 5`; do 
				perl runTest.pl --consumers=$t --producers=$t --poolType=$poolType --force="yes" --prodMigrate=$prodMigrate --affinity=$aff3 >> res_25_01.csv
			done
			done
		
			for aff4 in "0_1_2_3_4_5_6_7_8_9_10_11_12_13_14_15*18_19_16_17_22_23_20_21_26_27_24_25_30_31_28_29*0_1_18_19:2_3_16_17:4_5_22_23:6_7_20_21:8_9_26_27:10_11_24_25:12_13_30_31:14_15_28_29"; do
			echo "Affinity 4"
			for i in `seq 5`; do 
				perl runTest.pl --consumers=$t --producers=$t --poolType=$poolType --force="yes" --prodMigrate=$prodMigrate --affinity=$aff4 >> res_25_01.csv
			done
			done
		
			echo "no affinity"
			for i in `seq 5`; do 
				perl runTest.pl --consumers=$t --producers=$t --poolType=$poolType --force="no" --prodMigrate=$prodMigrate  >> res_25_01.csv
			done
		done 
	done
done
