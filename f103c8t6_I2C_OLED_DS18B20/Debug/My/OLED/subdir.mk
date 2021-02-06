################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../My/OLED/fonts.c \
../My/OLED/ssd1306.c 

OBJS += \
./My/OLED/fonts.o \
./My/OLED/ssd1306.o 

C_DEPS += \
./My/OLED/fonts.d \
./My/OLED/ssd1306.d 


# Each subdirectory must supply rules for building sources it contributes
My/OLED/fonts.o: ../My/OLED/fonts.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Main" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Drivers" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Application" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/MiddleLevel" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/CMSIS/inc" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/CMSIS/src" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/Startup" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Sheduler" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/OLED" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/OLED/fonts.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/OLED/ssd1306.o: ../My/OLED/ssd1306.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Main" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Drivers" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Application" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/MiddleLevel" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/CMSIS/inc" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/CMSIS/src" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/Startup" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/Sheduler" -I"E:/011_STM32CubeIDE_Prj/01_Board_F103C8T6/f103c8t6_I2C_OLED_DS18B20/My/OLED" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/OLED/ssd1306.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

