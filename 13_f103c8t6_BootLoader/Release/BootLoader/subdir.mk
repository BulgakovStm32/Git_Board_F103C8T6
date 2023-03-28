################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BootLoader/bootLoader.c 

OBJS += \
./BootLoader/bootLoader.o 

C_DEPS += \
./BootLoader/bootLoader.d 


# Each subdirectory must supply rules for building sources it contributes
BootLoader/bootLoader.o: ../BootLoader/bootLoader.c BootLoader/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/13_f103c8t6_BootLoader/BootLoader" -I"D:/04_Git/Git_Board_F103C8T6/13_f103c8t6_BootLoader/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/13_f103c8t6_BootLoader/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/13_f103c8t6_BootLoader/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/13_f103c8t6_BootLoader/My/Startup" -I"D:/04_Git/Git_Board_F103C8T6/13_f103c8t6_BootLoader/My/STM32_Perif" -Os -ffunction-sections -fdata-sections -Wall -Wextra -fstack-usage -MMD -MP -MF"BootLoader/bootLoader.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

