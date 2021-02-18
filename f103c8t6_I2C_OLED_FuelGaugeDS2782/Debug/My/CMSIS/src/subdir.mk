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
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/inc" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/CMSIS/src" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Drivers" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Application" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Main" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/MiddleLevel" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/OLED" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_I2C_OLED_FuelGaugeDS2782/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/CMSIS/src/system_stm32f1xx.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

