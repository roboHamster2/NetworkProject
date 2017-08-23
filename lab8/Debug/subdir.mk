################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../MThread.cpp \
../TCPMessengerClient.cpp \
../TCPSocket.cpp \
../UDPGame.cpp \
../main.cpp 

OBJS += \
./MThread.o \
./TCPMessengerClient.o \
./TCPSocket.o \
./UDPGame.o \
./main.o 

CPP_DEPS += \
./MThread.d \
./TCPMessengerClient.d \
./TCPSocket.d \
./UDPGame.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


