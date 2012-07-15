################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ChunkBased/ChunkPool.cpp \
../src/ChunkBased/MwLinkedList.cpp \
../src/ChunkBased/NoFIFOCASPool.cpp \
../src/ChunkBased/NoFIFOPool.cpp \
../src/ChunkBased/NoFIFOProducer.cpp \
../src/ChunkBased/NoFIFOStealing.cpp \
../src/ChunkBased/SPChunk.cpp \
../src/ChunkBased/SwLinkedList.cpp \
../src/ChunkBased/SwedishPool.cpp 

OBJS += \
./src/ChunkBased/ChunkPool.o \
./src/ChunkBased/MwLinkedList.o \
./src/ChunkBased/NoFIFOCASPool.o \
./src/ChunkBased/NoFIFOPool.o \
./src/ChunkBased/NoFIFOProducer.o \
./src/ChunkBased/NoFIFOStealing.o \
./src/ChunkBased/SPChunk.o \
./src/ChunkBased/SwLinkedList.o \
./src/ChunkBased/SwedishPool.o 

CPP_DEPS += \
./src/ChunkBased/ChunkPool.d \
./src/ChunkBased/MwLinkedList.d \
./src/ChunkBased/NoFIFOCASPool.d \
./src/ChunkBased/NoFIFOPool.d \
./src/ChunkBased/NoFIFOProducer.d \
./src/ChunkBased/NoFIFOStealing.d \
./src/ChunkBased/SPChunk.d \
./src/ChunkBased/SwLinkedList.d \
./src/ChunkBased/SwedishPool.d 


# Each subdirectory must supply rules for building sources it contributes
src/ChunkBased/%.o: ../src/ChunkBased/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"../src/LIFOPool" -I"../src" -I"../src/ChunkBased" -I"../src/MSQPool" -I"../src/Benchmark" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


