################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../NoFIFOPool/ChunkPool.cpp \
../NoFIFOPool/MwLinkedList.cpp \
../NoFIFOPool/NoFIFOPool.cpp \
../NoFIFOPool/SPChunk.cpp \
../NoFIFOPool/SwLinkedList.cpp 

OBJS += \
./NoFIFOPool/ChunkPool.o \
./NoFIFOPool/MwLinkedList.o \
./NoFIFOPool/NoFIFOPool.o \
./NoFIFOPool/SPChunk.o \
./NoFIFOPool/SwLinkedList.o 

CPP_DEPS += \
./NoFIFOPool/ChunkPool.d \
./NoFIFOPool/MwLinkedList.d \
./NoFIFOPool/NoFIFOPool.d \
./NoFIFOPool/SPChunk.d \
./NoFIFOPool/SwLinkedList.d 


# Each subdirectory must supply rules for building sources it contributes
NoFIFOPool/%.o: ../NoFIFOPool/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../SCPools/SCPools/src -I../SCPools/SCPools/src/Benchmark -I../SCPools/SCPools/src/MSQPool -I../SCPools/src/ChunkBased -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


