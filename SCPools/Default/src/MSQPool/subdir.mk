################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/MSQPool/MSQTaskPool.cpp 

OBJS += \
./src/MSQPool/MSQTaskPool.o 

CPP_DEPS += \
./src/MSQPool/MSQTaskPool.d 


# Each subdirectory must supply rules for building sources it contributes
src/MSQPool/%.o: ../src/MSQPool/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"../src/LIFOPool" -I"../src" -I"../src/ChunkBased" -I"../src/MSQPool" -I"../src/Benchmark" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


