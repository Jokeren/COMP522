cmake_minimum_required (VERSION 3.1)
project (salsa)

# Mode
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    "ArchEnvironment.cpp"
    "AtomicStatistics.cpp"
    "ChunkPool.cpp"
    "ConfigFile.cpp"
    "Configuration.cpp"
    "Consumer.cpp"
    "ConsumerThread.cpp"
    "Main.cpp"
    "NoFIFOPool.cpp"
    "NoFIFOProducer.cpp"
    "NoFIFOStealing.cpp"
    "Producer.cpp"
    "SPChunk.cpp"
    "SwLinkedList.cpp"
    "Threads.cpp"
    "hp/hp.cpp"
    "hp/simpleSet.c"
    "hp/stack.c")
 
# add the executable
add_executable(Main ${SOURCES})

TARGET_LINK_LIBRARIES(Main pthread rt)

