################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include My/Startup/subdir.mk
-include My/Sheduler/subdir.mk
-include My/OLED/subdir.mk
-include My/MiddleLevel/subdir.mk
-include My/Main/subdir.mk
-include My/Drivers/subdir.mk
-include My/CMSIS/subdir.mk
-include My/Application/subdir.mk
-include MCUv7/subdir.mk
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

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := f103c8t6_ProtocolEmulatorForMCUv7
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX := 
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME).$(BUILD_ARTIFACT_EXTENSION)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
f103c8t6_ProtocolEmulatorForMCUv7.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
f103c8t6_ProtocolEmulatorForMCUv7.list \

OBJCOPY_BIN += \
f103c8t6_ProtocolEmulatorForMCUv7.bin \


# All Target
all: main-build

# Main-build Target
main-build: f103c8t6_ProtocolEmulatorForMCUv7.elf secondary-outputs

# Tool invocations
f103c8t6_ProtocolEmulatorForMCUv7.elf: $(OBJS) $(USER_OBJS) D:\04_Git\Git_Board_F103C8T6\07_f103c8t6_ProtocolEmulatorForMCUv7\STM32F103C8TX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "f103c8t6_ProtocolEmulatorForMCUv7.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"D:\04_Git\Git_Board_F103C8T6\07_f103c8t6_ProtocolEmulatorForMCUv7\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="f103c8t6_ProtocolEmulatorForMCUv7.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

f103c8t6_ProtocolEmulatorForMCUv7.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "f103c8t6_ProtocolEmulatorForMCUv7.list"
	@echo 'Finished building: $@'
	@echo ' '

f103c8t6_ProtocolEmulatorForMCUv7.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "f103c8t6_ProtocolEmulatorForMCUv7.bin"
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
