################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MCUv7/Encoder.c \
../MCUv7/Motor.c \
../MCUv7/OpticSensors.c \
../MCUv7/Protocol_I2C.c 

OBJS += \
./MCUv7/Encoder.o \
./MCUv7/Motor.o \
./MCUv7/OpticSensors.o \
./MCUv7/Protocol_I2C.o 

C_DEPS += \
./MCUv7/Encoder.d \
./MCUv7/Motor.d \
./MCUv7/OpticSensors.d \
./MCUv7/Protocol_I2C.d 


# Each subdirectory must supply rules for building sources it contributes
MCUv7/Encoder.o: ../MCUv7/Encoder.c MCUv7/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"MCUv7/Encoder.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
MCUv7/Motor.o: ../MCUv7/Motor.c MCUv7/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"MCUv7/Motor.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
MCUv7/OpticSensors.o: ../MCUv7/OpticSensors.c MCUv7/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"MCUv7/OpticSensors.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
MCUv7/Protocol_I2C.o: ../MCUv7/Protocol_I2C.c MCUv7/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"MCUv7/Protocol_I2C.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

