################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ChunkBased/hp/hp.cpp 

C_SRCS += \
../src/ChunkBased/hp/simpleSet.c \
../src/ChunkBased/hp/stack.c 

OBJS += \
./src/ChunkBased/hp/hp.o \
./src/ChunkBased/hp/simpleSet.o \
./src/ChunkBased/hp/stack.o 

C_DEPS += \
./src/ChunkBased/hp/simpleSet.d \
./src/ChunkBased/hp/stack.d 

CPP_DEPS += \
./src/ChunkBased/hp/hp.d 


# Each subdirectory must supply rules for building sources it contributes
src/ChunkBased/hp/%.o: ../src/ChunkBased/hp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"../src/LIFOPool" -I"../src" -I"../src/ChunkBased" -I"../src/MSQPool" -I"../src/Benchmark" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ChunkBased/hp/%.o: ../src/ChunkBased/hp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


