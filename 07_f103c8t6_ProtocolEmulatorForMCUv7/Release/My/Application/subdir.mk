################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../My/Application/AHT10.c \
../My/Application/Ascii_Tab_12864.c \
../My/Application/Blink.c \
../My/Application/Crc.c \
../My/Application/DS18B20.c \
../My/Application/DS2782.c \
../My/Application/Delay.c \
../My/Application/Encoder.c \
../My/Application/Filters.c \
../My/Application/LcdGraphic.c \
../My/Application/Lcd_TIC32.c \
../My/Application/Lcd_i2c_1602.c \
../My/Application/MyString.c \
../My/Application/PID.c \
../My/Application/Time.c 

OBJS += \
./My/Application/AHT10.o \
./My/Application/Ascii_Tab_12864.o \
./My/Application/Blink.o \
./My/Application/Crc.o \
./My/Application/DS18B20.o \
./My/Application/DS2782.o \
./My/Application/Delay.o \
./My/Application/Encoder.o \
./My/Application/Filters.o \
./My/Application/LcdGraphic.o \
./My/Application/Lcd_TIC32.o \
./My/Application/Lcd_i2c_1602.o \
./My/Application/MyString.o \
./My/Application/PID.o \
./My/Application/Time.o 

C_DEPS += \
./My/Application/AHT10.d \
./My/Application/Ascii_Tab_12864.d \
./My/Application/Blink.d \
./My/Application/Crc.d \
./My/Application/DS18B20.d \
./My/Application/DS2782.d \
./My/Application/Delay.d \
./My/Application/Encoder.d \
./My/Application/Filters.d \
./My/Application/LcdGraphic.d \
./My/Application/Lcd_TIC32.d \
./My/Application/Lcd_i2c_1602.d \
./My/Application/MyString.d \
./My/Application/PID.d \
./My/Application/Time.d 


# Each subdirectory must supply rules for building sources it contributes
My/Application/AHT10.o: ../My/Application/AHT10.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/AHT10.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Ascii_Tab_12864.o: ../My/Application/Ascii_Tab_12864.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Ascii_Tab_12864.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Blink.o: ../My/Application/Blink.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Blink.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Crc.o: ../My/Application/Crc.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Crc.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/DS18B20.o: ../My/Application/DS18B20.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/DS18B20.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/DS2782.o: ../My/Application/DS2782.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/DS2782.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Delay.o: ../My/Application/Delay.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Delay.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Encoder.o: ../My/Application/Encoder.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Encoder.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Filters.o: ../My/Application/Filters.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Filters.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/LcdGraphic.o: ../My/Application/LcdGraphic.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/LcdGraphic.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Lcd_TIC32.o: ../My/Application/Lcd_TIC32.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Lcd_TIC32.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Lcd_i2c_1602.o: ../My/Application/Lcd_i2c_1602.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Lcd_i2c_1602.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/MyString.o: ../My/Application/MyString.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/MyString.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/PID.o: ../My/Application/PID.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/PID.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Application/Time.o: ../My/Application/Time.c My/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/MCUv7" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Application" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/CMSIS" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Drivers" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Main" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/MiddleLevel" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/OLED" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Sheduler" -I"D:/04_Git/Git_Board_F103C8T6/07_f103c8t6_ProtocolEmulatorForMCUv7/My/Startup" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Application/Time.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

