################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../My/Main/main.c 

OBJS += \
./My/Main/main.o 

C_DEPS += \
./My/Main/main.d 


# Each subdirectory must supply rules for building sources it contributes
My/Main/main.o: ../My/Main/main.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -c -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/inc" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/src" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Drivers" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Application" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Main" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/MiddleLevel" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/OLED" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Main/main.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
