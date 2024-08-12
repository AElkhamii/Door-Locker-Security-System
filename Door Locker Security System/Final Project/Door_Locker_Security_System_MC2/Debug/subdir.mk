################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../buzzer.c \
../dc_motor.c \
../door_locker_security_system_mc2.c \
../external_eeprom.c \
../gpio.c \
../i2c.c \
../pwm.c \
../timer.c \
../uart.c 

OBJS += \
./buzzer.o \
./dc_motor.o \
./door_locker_security_system_mc2.o \
./external_eeprom.o \
./gpio.o \
./i2c.o \
./pwm.o \
./timer.o \
./uart.o 

C_DEPS += \
./buzzer.d \
./dc_motor.d \
./door_locker_security_system_mc2.d \
./external_eeprom.d \
./gpio.d \
./i2c.d \
./pwm.d \
./timer.d \
./uart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega16 -DF_CPU=1000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


