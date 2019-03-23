1. Steal

In `NoFIFOStealing.cpp`, enable stealing:

    // enable steal
    #define THRESHOLD 1

Aslo in `config.txt`, change producers and consumers:

    producersNum=1
    consumersNum=4

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

2. HP race 

WARNING: ThreadSanitizer: data race (pid=70019)
  Write of size 8 at 0x7b0c00001808 by thread T2:
    #0 HP::setHP(int, void*, HP::HPLocal_t*) hp/hp.cpp:316 (Main+0x000000417a62)
    #1 SwLinkedList::getLast(HP::HPLocal_t*) /home/kz21/Codes/COMP522/SALSA-atomic/SwLinkedList.cpp:110 (Main+0x00000040f62d)
    #2 NoFIFOPool::getStealNode(int&) /home/kz21/Codes/COMP522/SALSA-atomic/NoFIFOStealing.cpp:49 (Main+0x00000041007c)
    #3 NoFIFOPool::steal(SCTaskPool*, AtomicStatistics*) /home/kz21/Codes/COMP522/SALSA-atomic/NoFIFOStealing.cpp:56 (Main+0x000000410103)
    #4 Consumer::consume(Task*&) /home/kz21/Codes/COMP522/SALSA-atomic/Consumer.cpp:57 (Main+0x00000040cb82)
    #5 consRun(void*) /home/kz21/Codes/COMP522/SALSA-atomic/Threads.cpp:171 (Main+0x000000412e91)

  Previous write of size 8 at 0x7b0c00001808 by thread T1:
    #0 HP::setHP(int, void*, HP::HPLocal_t*) hp/hp.cpp:316 (Main+0x000000417a62)
    #1 SwLinkedList::SwLinkedListIterator::SwLinkedListIterator(SwLinkedList*, HP::HPLocal_t*) /home/kz21/Codes/COMP522/SALSA-atomic/SwLinkedList.cpp:125 (Main+0x00000040f774)
    #2 NoFIFOPool::consume(Task*&, AtomicStatistics*) /home/kz21/Codes/COMP522/SALSA-atomic/NoFIFOPool.cpp:75 (Main+0x000000415f8b)
    #3 Consumer::consume(Task*&) /home/kz21/Codes/COMP522/SALSA-atomic/Consumer.cpp:39 (Main+0x00000040c737)
    #4 consRun(void*) /home/kz21/Codes/COMP522/SALSA-atomic/Threads.cpp:171 (Main+0x000000412e91)

3. *getlast* and *append*

WARNING: ThreadSanitizer: data race (pid=70019)
  Read of size 8 at 0x7b2000004048 by thread T4:
    #0 SwLinkedList::getLast(HP::HPLocal_t*) /home/kz21/Codes/COMP522/SALSA-atomic/SwLinkedList.cpp:109 (Main+0x00000040f60c)
    #1 NoFIFOPool::getStealNode(int&) /home/kz21/Codes/COMP522/SALSA-atomic/NoFIFOStealing.cpp:49 (Main+0x00000041007c)
    #2 NoFIFOPool::steal(SCTaskPool*, AtomicStatistics*) /home/kz21/Codes/COMP522/SALSA-atomic/NoFIFOStealing.cpp:56 (Main+0x000000410103)
    #3 Consumer::consume(Task*&) /home/kz21/Codes/COMP522/SALSA-atomic/Consumer.cpp:49 (Main+0x00000040c97d)
    #4 consRun(void*) /home/kz21/Codes/COMP522/SALSA-atomic/Threads.cpp:171 (Main+0x000000412e91)

  Previous write of size 8 at 0x7b2000004048 by thread T7:
    #0 SwLinkedList::append(SPChunk*, HP::HPLocal_t*) /home/kz21/Codes/COMP522/SALSA-atomic/SwLinkedList.cpp:72 (Main+0x00000040f33f)
    #1 NoFIFOPool::ProdCtx::produceImpl(Task&, bool&, bool, AtomicStatistics*) /home/kz21/Codes/COMP522/SALSA-atomic/NoFIFOProducer.cpp:36 (Main+0x00000041501d)
    #2 NoFIFOPool::ProdCtx::produce(Task&, bool&, AtomicStatistics*) /home/kz21/Codes/COMP522/SALSA-atomic/NoFIFOProducer.cpp:14 (Main+0x000000414e1e)
    #3 Producer::produce(Task&) /home/kz21/Codes/COMP522/SALSA-atomic/Producer.cpp:40 (Main+0x000000403a46)
    #4 prodRun(void*) /home/kz21/Codes/COMP522/SALSA-atomic/Threads.cpp:71 (Main+0x000000412574)

3. Drawbacks of ThreadSanitizer

False positive on a lot of cases, including data races on atomic variables, single-writer multiple-reader data structures, and places where data race does not affect the correctness.


