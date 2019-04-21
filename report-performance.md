1. Relaxed memory order


Table 1. Throughput comparisons of SALSA implementations on x86 with different consistency models

| Threads | x86 | x86-atomics | x86-atomics-seq-cst |
|:---:|:---:|:---:|:---:|
|1|63854.7|64990.2|4858.45|
|2|121092|114362|21948.1|
|4|244902|233379|41218.1|
|8|438431|460750|81884.8|
|16|868180|915810|177685|
|32|1079990|1048580|501238|
|64|Limit|Limit|Failed|
|128|Limit|Limit|Failed|

Table 2. Throughput comparisons of SALSA implementations on knl with different consistency models

| Threads | knl | knl-atomics | knl-atomics-seq-cst |
|:---:|:---:|:---:|:---:|
|1|14189.9|14157.9|10031.9|
|2|28528.7|28151.3|20905.8|
|4|56770.9|55927.8|41790.1|
|8|112708|109864|83078.4|
|16|217125|219441|166760|
|32|410803|433665|329171|
|64|Failed| 418205|368184|
|128|Failed | 351742|369356|

Table 3. Throughput comparisons of SALSA implementations on ppc64le with different consistency models

| Threads | x86 | ppc64le-atomics | ppc64le-atomics-seq-cst |
|:---:|:---:|:---:|:---:|
|1|33805.3|34311.7|14390|
|2|66994.5|68479|28761.6|
|4|133647|135402|57040.8|
|8|266456|270545|112708|
|16|420960|426122|204229|
|32|540824|540505|307293|
|64|624135|Failed|362346|
|128|Limit|Limit|Limit|

2. Thread binding

Table 4. Throughput comparisons of SALSA implementations on different platforms with different binding mechanisms

| Threads | x86-unbind | x86-bind | knl-unbind | knl-bind | ppc64le-unbind | ppc64le-bind |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|1|63854.7|64539.6|14189.9|14474.1|34103.1|33805.3|
|2|121092|117882|28528.7|28761.5|62110.6|66994.5|
|4|244902|232078|56770.9|57235.5|104429|133647|
|8|438431|456584|112708|113454|217123|266456|
|16|868180|899336|217125|222630|347746|420960|
|32|1079990|1053280|410803|431995|387694|540824|
|64|Limit|Limit|Failed| Failed|437461|624135|
|128|Limit|Limit|Failed | Failed|Limit|Limit|

3. Memory usage

Table 5. Memory consumption of SALSA x86 implementation (MB/10s)

| Threads | x86 |
|:---:|:---:|
|1| 161 |
|2| 292 |
|4| 276 |
|8| 243 |
|16| 141 |
|32| 243 |

4. 1 producer and N consumer

Table 6. Throughput comparisons of SALSA implementations on different platforms with 1 producer and N consumer

| Consumer Threads | knl-bind | x86-bind | ppc64le-bind |
|:---:|:---:|:---:|:---:|:---:|
|1|14449.1|67424|12750|
|2|17099.5|96722|18260|
|4|Failed|96247|17817|
|8|Failed|91192|Failed|
|16|Failed|93713|15226|
|32|Failed|95211|15799|
|64|Failed|Limit|Failed|
|128|Failed|Limit|Failed|
