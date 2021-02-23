################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../My/MiddleLevel/Encoder.c \
../My/MiddleLevel/LM6063D.c 

OBJS += \
./My/MiddleLevel/Encoder.o \
./My/MiddleLevel/LM6063D.o 

C_DEPS += \
./My/MiddleLevel/Encoder.d \
./My/MiddleLevel/LM6063D.d 


# Each subdirectory must supply rules for building sources it contributes
My/MiddleLevel/Encoder.o: ../My/MiddleLevel/Encoder.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -c -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/inc" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/src" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Drivers" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Application" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Main" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/MiddleLevel" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/OLED" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/MiddleLevel/Encoder.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/MiddleLevel/LM6063D.o: ../My/MiddleLevel/LM6063D.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -c -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/inc" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/src" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Drivers" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Application" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Main" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/MiddleLevel" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/OLED" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/MiddleLevel/LM6063D.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

