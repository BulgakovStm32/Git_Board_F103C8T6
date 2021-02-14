################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../My/Application/AHT10.c \
../My/Application/Ascii_Tab_12864.c \
../My/Application/Blink.c \
../My/Application/DS18B20.c \
../My/Application/DS2782.c \
../My/Application/Delay.c \
../My/Application/Filters.c \
../My/Application/LcdGraphic.c \
../My/Application/Lcd_TIC32.c \
../My/Application/Lcd_i2c_1602.c 

OBJS += \
./My/Application/AHT10.o \
./My/Application/Ascii_Tab_12864.o \
./My/Application/Blink.o \
./My/Application/DS18B20.o \
./My/Application/DS2782.o \
./My/Application/Delay.o \
./My/Application/Filters.o \
./My/Application/LcdGraphic.o \
./My/Application/Lcd_TIC32.o \
./My/Application/Lcd_i2c_1602.o 

C_DEPS += \
./My/Application/AHT10.d \
./My/Application/Ascii_Tab_12864.d \
./My/Application/Blink.d \
./My/Application/DS18B20.d \
./My/Application/DS2782.d \
./My/Application/Delay.d \
./My/Application/Filters.d \
./My/Application/LcdGraphic.d \
./My/Application/Lcd_TIC32.d \
./My/Application/Lcd_i2c_1602.d 


# Each subdirectory must supply rules for building sources it contributes
My/Application/AHT10.o: ../My/Application/AHT10.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/inc" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/src" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Drivers" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Application" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Main" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/MiddleLevel" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/OLED" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/AHT10.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Ascii_Tab_12864.o: ../My/Application/Ascii_Tab_12864.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/inc" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/src" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Drivers" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Application" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Main" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/MiddleLevel" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/OLED" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Ascii_Tab_12864.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Blink.o: ../My/Application/Blink.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/inc" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/src" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Drivers" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Application" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Main" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/MiddleLevel" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/OLED" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Blink.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/DS18B20.o: ../My/Application/DS18B20.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/inc" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/src" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Drivers" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Application" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Main" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/MiddleLevel" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/OLED" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/DS18B20.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/DS2782.o: ../My/Application/DS2782.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/inc" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/src" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Drivers" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Application" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Main" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/MiddleLevel" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/OLED" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/DS2782.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Delay.o: ../My/Application/Delay.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/inc" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/src" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Drivers" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Application" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Main" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/MiddleLevel" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/OLED" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Delay.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Filters.o: ../My/Application/Filters.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/inc" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/src" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Drivers" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Application" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Main" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/MiddleLevel" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/OLED" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Filters.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/LcdGraphic.o: ../My/Application/LcdGraphic.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/inc" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/src" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Drivers" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Application" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Main" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/MiddleLevel" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/OLED" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/LcdGraphic.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Lcd_TIC32.o: ../My/Application/Lcd_TIC32.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/inc" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/src" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Drivers" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Application" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Main" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/MiddleLevel" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/OLED" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Lcd_TIC32.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Lcd_i2c_1602.o: ../My/Application/Lcd_i2c_1602.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/inc" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/src" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Drivers" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Application" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Main" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/MiddleLevel" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/OLED" -I"C:/My/Git/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Lcd_i2c_1602.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

