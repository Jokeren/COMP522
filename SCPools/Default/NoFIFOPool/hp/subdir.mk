################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../NoFIFOPool/hp/hp.cpp 

C_SRCS += \
../NoFIFOPool/hp/simpleSet.c \
../NoFIFOPool/hp/stack.c 

OBJS += \
./NoFIFOPool/hp/hp.o \
./NoFIFOPool/hp/simpleSet.o \
./NoFIFOPool/hp/stack.o 

C_DEPS += \
./NoFIFOPool/hp/simpleSet.d \
./NoFIFOPool/hp/stack.d 

CPP_DEPS += \
./NoFIFOPool/hp/hp.d 


# Each subdirectory must supply rules for building sources it contributes
NoFIFOPool/hp/%.o: ../NoFIFOPool/hp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../SCPools/SCPools/src -I../SCPools/SCPools/src/Benchmark -I../SCPools/SCPools/src/MSQPool -I../SCPools/src/ChunkBased -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

NoFIFOPool/hp/%.o: ../NoFIFOPool/hp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I../SCPools/SCPools/src -I../SCPools/SCPools/src/Benchmark -I../SCPools/SCPools/src/MSQPool -I../SCPools/src/ChunkBased -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


