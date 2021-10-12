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
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Main" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Drivers" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Application" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/MiddleLevel" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/CMSIS/inc" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/CMSIS/src" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/Startup" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Sheduler" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/OLED" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/MiddleLevel/Encoder.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/MiddleLevel/LM6063D.o: ../My/MiddleLevel/LM6063D.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Main" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Drivers" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Application" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/MiddleLevel" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/CMSIS/inc" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/CMSIS/src" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/Startup" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Sheduler" -I"E:/011_STM32CubeIDE_Prj/Git_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/OLED" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/MiddleLevel/LM6063D.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

