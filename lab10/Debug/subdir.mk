################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../AuthRequestsDispatcher.cpp \
../MThread.cpp \
../MultipleTCPSocketsListener.cpp \
../PeersRequestsDispatcher.cpp \
../TCPMessengerServer.cpp \
../TCPSessionBroker.cpp \
../TCPSocket.cpp \
../User.cpp \
../main.cpp 

OBJS += \
./AuthRequestsDispatcher.o \
./MThread.o \
./MultipleTCPSocketsListener.o \
./PeersRequestsDispatcher.o \
./TCPMessengerServer.o \
./TCPSessionBroker.o \
./TCPSocket.o \
./User.o \
./main.o 

CPP_DEPS += \
./AuthRequestsDispatcher.d \
./MThread.d \
./MultipleTCPSocketsListener.d \
./PeersRequestsDispatcher.d \
./TCPMessengerServer.d \
./TCPSessionBroker.d \
./TCPSocket.d \
./User.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


