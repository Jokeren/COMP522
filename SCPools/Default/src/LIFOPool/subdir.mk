################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/LIFOPool/LIFOPool.cpp 

OBJS += \
./src/LIFOPool/LIFOPool.o 

CPP_DEPS += \
./src/LIFOPool/LIFOPool.d 


# Each subdirectory must supply rules for building sources it contributes
src/LIFOPool/%.o: ../src/LIFOPool/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"../src/LIFOPool" -I"../src" -I"../src/ChunkBased" -I"../src/MSQPool" -I"../src/Benchmark" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


