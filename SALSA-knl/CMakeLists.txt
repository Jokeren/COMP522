project (salsa)

# Mode
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -std=c++11")

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

find_library(JEMALLOC NAMES jemalloc PATHS ${JEMALLOC_DIR} PATH_SUFFIXES lib NO_DEFAULT_PATH)
if (JEMALLOC)
  TARGET_LINK_LIBRARIES(Main ${JEMALLOC})
endif()
