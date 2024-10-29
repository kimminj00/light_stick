################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../yes/my_uart.c \
../yes/parameter_ci.c \
../yes/yes_radio.c 

OBJS += \
./yes/my_uart.o \
./yes/parameter_ci.o \
./yes/yes_radio.o 

C_DEPS += \
./yes/my_uart.d \
./yes/parameter_ci.d \
./yes/yes_radio.d 


# Each subdirectory must supply rules for building sources it contributes
yes/my_uart.o: ../yes/my_uart.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"yes/my_uart.d" -MT"yes/my_uart.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

yes/parameter_ci.o: ../yes/parameter_ci.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"yes/parameter_ci.d" -MT"yes/parameter_ci.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

yes/yes_radio.o: ../yes/yes_radio.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"yes/yes_radio.d" -MT"yes/yes_radio.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


