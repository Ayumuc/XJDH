################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Air820.c \
../Core/Src/Data_Collect.c \
../Core/Src/Delay.c \
../Core/Src/Power_Control.c \
../Core/Src/adc.c \
../Core/Src/cJSON.c \
../Core/Src/can.c \
../Core/Src/can1.c \
../Core/Src/dma.c \
../Core/Src/gpio.c \
../Core/Src/gps_Tobd09.c \
../Core/Src/main.c \
../Core/Src/motor_contorl.c \
../Core/Src/pid.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/tim.c \
../Core/Src/u_RockerKey.c \
../Core/Src/usart.c \
../Core/Src/wt1_imu.c 

OBJS += \
./Core/Src/Air820.o \
./Core/Src/Data_Collect.o \
./Core/Src/Delay.o \
./Core/Src/Power_Control.o \
./Core/Src/adc.o \
./Core/Src/cJSON.o \
./Core/Src/can.o \
./Core/Src/can1.o \
./Core/Src/dma.o \
./Core/Src/gpio.o \
./Core/Src/gps_Tobd09.o \
./Core/Src/main.o \
./Core/Src/motor_contorl.o \
./Core/Src/pid.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/tim.o \
./Core/Src/u_RockerKey.o \
./Core/Src/usart.o \
./Core/Src/wt1_imu.o 

C_DEPS += \
./Core/Src/Air820.d \
./Core/Src/Data_Collect.d \
./Core/Src/Delay.d \
./Core/Src/Power_Control.d \
./Core/Src/adc.d \
./Core/Src/cJSON.d \
./Core/Src/can.d \
./Core/Src/can1.d \
./Core/Src/dma.d \
./Core/Src/gpio.d \
./Core/Src/gps_Tobd09.d \
./Core/Src/main.d \
./Core/Src/motor_contorl.d \
./Core/Src/pid.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/tim.d \
./Core/Src/u_RockerKey.d \
./Core/Src/usart.d \
./Core/Src/wt1_imu.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Air820.cyclo ./Core/Src/Air820.d ./Core/Src/Air820.o ./Core/Src/Air820.su ./Core/Src/Data_Collect.cyclo ./Core/Src/Data_Collect.d ./Core/Src/Data_Collect.o ./Core/Src/Data_Collect.su ./Core/Src/Delay.cyclo ./Core/Src/Delay.d ./Core/Src/Delay.o ./Core/Src/Delay.su ./Core/Src/Power_Control.cyclo ./Core/Src/Power_Control.d ./Core/Src/Power_Control.o ./Core/Src/Power_Control.su ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/cJSON.cyclo ./Core/Src/cJSON.d ./Core/Src/cJSON.o ./Core/Src/cJSON.su ./Core/Src/can.cyclo ./Core/Src/can.d ./Core/Src/can.o ./Core/Src/can.su ./Core/Src/can1.cyclo ./Core/Src/can1.d ./Core/Src/can1.o ./Core/Src/can1.su ./Core/Src/dma.cyclo ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/dma.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/gps_Tobd09.cyclo ./Core/Src/gps_Tobd09.d ./Core/Src/gps_Tobd09.o ./Core/Src/gps_Tobd09.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/motor_contorl.cyclo ./Core/Src/motor_contorl.d ./Core/Src/motor_contorl.o ./Core/Src/motor_contorl.su ./Core/Src/pid.cyclo ./Core/Src/pid.d ./Core/Src/pid.o ./Core/Src/pid.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/u_RockerKey.cyclo ./Core/Src/u_RockerKey.d ./Core/Src/u_RockerKey.o ./Core/Src/u_RockerKey.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su ./Core/Src/wt1_imu.cyclo ./Core/Src/wt1_imu.d ./Core/Src/wt1_imu.o ./Core/Src/wt1_imu.su

.PHONY: clean-Core-2f-Src

