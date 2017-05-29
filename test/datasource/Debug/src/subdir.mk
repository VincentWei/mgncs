################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/datasource.c \
../src/datasource_main.c \
../src/open_ds.c \
../src/selectdlg.c 

OBJS += \
./src/datasource.o \
./src/datasource_main.o \
./src/open_ds.o \
./src/selectdlg.o 

C_DEPS += \
./src/datasource.d \
./src/datasource_main.d \
./src/open_ds.d \
./src/selectdlg.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/ -I/usr/local/include/ -I../include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


