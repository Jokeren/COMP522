################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ArchEnvironment.cpp \
../src/AtomicStatistics.cpp \
../src/ConfigFile.cpp \
../src/Configuration.cpp \
../src/Consumer.cpp \
../src/MSQTest.cpp \
../src/Producer.cpp 

OBJS += \
./src/ArchEnvironment.o \
./src/AtomicStatistics.o \
./src/ConfigFile.o \
./src/Configuration.o \
./src/Consumer.o \
./src/MSQTest.o \
./src/Producer.o 

CPP_DEPS += \
./src/ArchEnvironment.d \
./src/AtomicStatistics.d \
./src/ConfigFile.d \
./src/Configuration.d \
./src/Consumer.d \
./src/MSQTest.d \
./src/Producer.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"../src/LIFOPool" -I"../src" -I"../src/ChunkBased" -I"../src/MSQPool" -I"../src/Benchmark" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


