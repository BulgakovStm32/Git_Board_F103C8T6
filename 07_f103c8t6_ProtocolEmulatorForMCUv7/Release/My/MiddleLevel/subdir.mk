################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../My/MiddleLevel/LM6063D.c 

OBJS += \
./My/MiddleLevel/LM6063D.o 

C_DEPS += \
./My/MiddleLevel/LM6063D.d 


# Each subdirectory must supply rules for building sources it contributes
My/MiddleLevel/LM6063D.o: ../My/MiddleLevel/LM6063D.c My/MiddleLevel/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/MiddleLevel/LM6063D.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

