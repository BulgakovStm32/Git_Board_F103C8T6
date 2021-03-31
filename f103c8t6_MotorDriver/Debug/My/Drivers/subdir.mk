################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../My/Drivers/dma_ST.c \
../My/Drivers/gpio_ST.c \
../My/Drivers/i2c_ST.c \
../My/Drivers/spi_ST.c \
../My/Drivers/sys_ST.c \
../My/Drivers/sys_tick_ST.c \
../My/Drivers/timers_ST.c 

OBJS += \
./My/Drivers/dma_ST.o \
./My/Drivers/gpio_ST.o \
./My/Drivers/i2c_ST.o \
./My/Drivers/spi_ST.o \
./My/Drivers/sys_ST.o \
./My/Drivers/sys_tick_ST.o \
./My/Drivers/timers_ST.o 

C_DEPS += \
./My/Drivers/dma_ST.d \
./My/Drivers/gpio_ST.d \
./My/Drivers/i2c_ST.d \
./My/Drivers/spi_ST.d \
./My/Drivers/sys_ST.d \
./My/Drivers/sys_tick_ST.d \
./My/Drivers/timers_ST.d 


# Each subdirectory must supply rules for building sources it contributes
My/Drivers/dma_ST.o: ../My/Drivers/dma_ST.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_STDPERIPH_DRIVER -c -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/inc" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/src" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Drivers" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Application" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Main" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/MiddleLevel" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/OLED" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Drivers/dma_ST.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Drivers/gpio_ST.o: ../My/Drivers/gpio_ST.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_STDPERIPH_DRIVER -c -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/inc" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/src" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Drivers" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Application" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Main" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/MiddleLevel" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/OLED" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Drivers/gpio_ST.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Drivers/i2c_ST.o: ../My/Drivers/i2c_ST.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_STDPERIPH_DRIVER -c -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/inc" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/src" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Drivers" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Application" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Main" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/MiddleLevel" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/OLED" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Drivers/i2c_ST.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Drivers/spi_ST.o: ../My/Drivers/spi_ST.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_STDPERIPH_DRIVER -c -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/inc" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/src" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Drivers" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Application" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Main" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/MiddleLevel" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/OLED" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Drivers/spi_ST.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Drivers/sys_ST.o: ../My/Drivers/sys_ST.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_STDPERIPH_DRIVER -c -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/inc" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/src" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Drivers" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Application" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Main" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/MiddleLevel" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/OLED" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Drivers/sys_ST.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Drivers/sys_tick_ST.o: ../My/Drivers/sys_tick_ST.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_STDPERIPH_DRIVER -c -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/inc" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/src" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Drivers" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Application" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Main" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/MiddleLevel" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/OLED" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Drivers/sys_tick_ST.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
My/Drivers/timers_ST.o: ../My/Drivers/timers_ST.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_STDPERIPH_DRIVER -c -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/inc" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/CMSIS/src" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Drivers" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Application" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Main" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/MiddleLevel" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/OLED" -I"D:/04_CubeIDE_Prj/01_GitHub/Git_Board_F103C8T6/f103c8t6_MotorDriver/My/Sheduler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"My/Drivers/timers_ST.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

