1. Relaxed memory order


Table 1. Throughput comparisons of SALSA implementations on x86 with different consistency models

| Threads | x86 | x86-atomics | x86-atomics-seq-cst |
|:---:|:---:|:---:|:---:|
|1|63854.7|64539.6|14189.9|
|2|121092|117882|28528.7|
|4|244902|232078|56770.9|
|8|438431|456584|112708|
|16|868180|899336|217125|
|32|1079990|1053280|410803|
|64|Limit|Limit|Failed|
|128|Limit|Limit|Failed|

Table 2. Throughput comparisons of SALSA implementations on knl with different consistency models

| Threads | knl | knl-atomics | knl-atomics-seq-cst |
|:---:|:---:|:---:|:---:|
|1|63854.7|64539.6|14189.9|
|2|121092|117882|28528.7|
|4|244902|232078|56770.9|
|8|438431|456584|112708|
|16|868180|899336|217125|
|32|1079990|1053280|410803|
|64|Limit|Limit|Failed|
|128|Limit|Limit|Failed|

Table 3. Throughput comparisons of SALSA implementations on ppc64le with different consistency models

| Threads | x86 | ppc64le-atomics | ppc64le-atomics-seq-cst |
|:---:|:---:|:---:|:---:|
|1|33805.3|34311.7|14189.9|
|2|66994.5|68479|28528.7|
|4|133647|135402|56770.9|
|8|266456|270545|112708|
|16|420960|426122|217125|
|32|540824|540505|410803|
|64|624135|Failed|Failed|
|128|Limit|Limit|Failed|

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
