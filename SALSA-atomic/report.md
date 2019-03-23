1. Steal bug

In `NoFIFOStealing.cpp`, enable stealing:

    // enable steal
    #define THRESHOLD 1

log:

    Read of size 4 at 0x7b0800004108 by thread T1:
      #0 NoFIFOPool::steal(SCTaskPool*, AtomicStatistics*) /home/kz21/Codes/COMP522/SALSA-atomic/NoFIFOStealing.cpp:90 (Main+0x000000410384)
      #1 Consumer::consume(Task*&) /home/kz21/Codes/COMP522/SALSA-atomic/Consumer.cpp:57 (Main+0x00000040cb82)
      #2 consRun(void*) /home/kz21/Codes/COMP522/SALSA-atomic/Threads.cpp:171 (Main+0x000000412e91)

    Previous write of size 4 at 0x7b0800004108 by thread T4:
      #0 NoFIFOPool::takeTask(SwNode*, AtomicStatistics*) /home/kz21/Codes/COMP522/SALSA-atomic/NoFIFOPool.cpp:122 (Main+0x0000004163ea)
      #1 NoFIFOPool::consume(Task*&, AtomicStatistics*) /home/kz21/Codes/COMP522/SALSA-atomic/NoFIFOPool.cpp:66 (Main+0x000000415ee4)
      #2 Consumer::consume(Task*&) /home/kz21/Codes/COMP522/SALSA-atomic/Consumer.cpp:39 (Main+0x00000040c737)
      #3 consRun(void*) /home/kz21/Codes/COMP522/SALSA-atomic/Threads.cpp:171 (Main+0x000000412e91)

So we have the following data race:

    122: n->consumerIdx++;

and 

    66: newNode->consumerIdx = prevNode->consumerIdx;

Right now, I do not think thread yielding could help avoid the race, but I need to read Dave's paper more carefully.

