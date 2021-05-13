################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../My/CMSIS/src/system_stm32f1xx.c 

OBJS += \
./My/CMSIS/src/system_stm32f1xx.o 

C_DEPS += \
./My/CMSIS/src/system_stm32f1xx.d 


# Each subdirectory must supply rules for building sources it contributes
My/CMSIS/src/system_stm32f1xx.o: ../My/CMSIS/src/system_stm32f1xx.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_STDPERIPH_DRIVER -c -I"D:/04_Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/inc" -I"D:/04_Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/src" -I"D:/04_Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/CMSIS/src/system_stm32f1xx.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

