################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include My/Sheduler/subdir.mk
-include My/OLED/subdir.mk
-include My/MiddleLevel/subdir.mk
-include My/Main/subdir.mk
-include My/Drivers/subdir.mk
-include My/CMSIS/src/subdir.mk
-include My/Application/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

BUILD_ARTIFACT_NAME := f103c8t6_i2c_oled_ds18b20
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX := 
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME).$(BUILD_ARTIFACT_EXTENSION)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
f103c8t6_i2c_oled_ds18b20.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
f103c8t6_i2c_oled_ds18b20.list \

OBJCOPY_BIN += \
f103c8t6_i2c_oled_ds18b20.bin \


# All Target
all: main-build

# Main-build Target
main-build: f103c8t6_i2c_oled_ds18b20.elf secondary-outputs

# Tool invocations
f103c8t6_i2c_oled_ds18b20.elf: $(OBJS) $(USER_OBJS) E:\011_STM32CubeIDE_Prj\Git_Board_F103C8T6\f103c8t6_I2C_OLED_DS18B20\Startup\STM32F103C8TX_FLASH.ld
	arm-none-eabi-gcc -o "f103c8t6_i2c_oled_ds18b20.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"E:\011_STM32CubeIDE_Prj\Git_Board_F103C8T6\f103c8t6_I2C_OLED_DS18B20\Startup\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="f103c8t6_i2c_oled_ds18b20.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

f103c8t6_i2c_oled_ds18b20.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "f103c8t6_i2c_oled_ds18b20.list"
	@echo 'Finished building: $@'
	@echo ' '

f103c8t6_i2c_oled_ds18b20.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "f103c8t6_i2c_oled_ds18b20.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents fail-specified-linker-script-missing warn-no-linker-script-specified
.SECONDARY:

-include ../makefile.targets
