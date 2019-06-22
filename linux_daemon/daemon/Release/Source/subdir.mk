################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Source/daemon_tools.c \
../Source/main.c \
../Source/mutex_data.c \
../Source/signal_register.c \
../Source/socket_child.c \
../Source/socket_server.c \
../Source/sockfunc_child.c \
../Source/sockfunc_parent.c \
../Source/syslog_output.c \
../Source/websocket_func.c 

OBJS += \
./Source/daemon_tools.o \
./Source/main.o \
./Source/mutex_data.o \
./Source/signal_register.o \
./Source/socket_child.o \
./Source/socket_server.o \
./Source/sockfunc_child.o \
./Source/sockfunc_parent.o \
./Source/syslog_output.o \
./Source/websocket_func.o 

C_DEPS += \
./Source/daemon_tools.d \
./Source/main.d \
./Source/mutex_data.d \
./Source/signal_register.d \
./Source/socket_child.d \
./Source/socket_server.d \
./Source/sockfunc_child.d \
./Source/sockfunc_parent.d \
./Source/syslog_output.d \
./Source/websocket_func.d 


# Each subdirectory must supply rules for building sources it contributes
Source/%.o: ../Source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


