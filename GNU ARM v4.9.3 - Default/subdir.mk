################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app_hal.c \
../app_main.c 

OBJS += \
./app_hal.o \
./app_main.o 

C_DEPS += \
./app_hal.d \
./app_main.d 


# Each subdirectory must supply rules for building sources it contributes
app_hal.o: ../app_hal.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"app_hal.d" -MT"app_hal.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

app_main.o: ../app_main.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"app_main.d" -MT"app_main.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


