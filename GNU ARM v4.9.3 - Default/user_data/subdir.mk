################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../user_data/user_data.c 

OBJS += \
./user_data/user_data.o 

C_DEPS += \
./user_data/user_data.d 


# Each subdirectory must supply rules for building sources it contributes
user_data/user_data.o: ../user_data/user_data.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"user_data/user_data.d" -MT"user_data/user_data.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


