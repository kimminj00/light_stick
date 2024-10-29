################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include yes/subdir.mk
-include user_data/subdir.mk
-include .metadata/.plugins/org.eclipse.cdt.make.core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: 240417_Zigbee_Board_Source_V1.3.axf

# Tool invocations
240417_Zigbee_Board_Source_V1.3.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "240417_Zigbee_Board_Source_V1.3.ld" -nostdlib -Xlinker --gc-sections -Xlinker -Map="240417_Zigbee_Board_Source_V1.3.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o 240417_Zigbee_Board_Source_V1.3.axf "./.metadata/.plugins/org.eclipse.cdt.make.core/specs.o" "./app_hal.o" "./app_main.o" "./user_data/user_data.o" "./yes/my_uart.o" "./yes/parameter_ci.o" "./yes/yes_radio.o" 
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: 240417_Zigbee_Board_Source_V1.3.hex'
	arm-none-eabi-objcopy -O ihex "240417_Zigbee_Board_Source_V1.3.axf" "240417_Zigbee_Board_Source_V1.3.hex"
	@echo ' '

	@echo 'Building bin file: 240417_Zigbee_Board_Source_V1.3.bin'
	arm-none-eabi-objcopy -O binary "240417_Zigbee_Board_Source_V1.3.axf" "240417_Zigbee_Board_Source_V1.3.bin"
	@echo ' '

	@echo 'Building s37 file: 240417_Zigbee_Board_Source_V1.3.s37'
	arm-none-eabi-objcopy -O srec "240417_Zigbee_Board_Source_V1.3.axf" "240417_Zigbee_Board_Source_V1.3.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "240417_Zigbee_Board_Source_V1.3.axf" -A
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) 240417_Zigbee_Board_Source_V1.3.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
