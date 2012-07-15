################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Benchmark/ConsumerThread.cpp \
../src/Benchmark/Main.cpp \
../src/Benchmark/ProducerThread.cpp \
../src/Benchmark/Threads.cpp 

OBJS += \
./src/Benchmark/ConsumerThread.o \
./src/Benchmark/Main.o \
./src/Benchmark/ProducerThread.o \
./src/Benchmark/Threads.o 

CPP_DEPS += \
./src/Benchmark/ConsumerThread.d \
./src/Benchmark/Main.d \
./src/Benchmark/ProducerThread.d \
./src/Benchmark/Threads.d 


# Each subdirectory must supply rules for building sources it contributes
src/Benchmark/%.o: ../src/Benchmark/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"../src/LIFOPool" -I"../src" -I"../src/ChunkBased" -I"../src/MSQPool" -I"../src/Benchmark" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


