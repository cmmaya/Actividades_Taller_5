################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/I2CMain.c \
../Src/IntroGPIOxMain.c \
../Src/PWMMain.c \
../Src/RTCMain.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/I2CMain.o \
./Src/IntroGPIOxMain.o \
./Src/PWMMain.o \
./Src/RTCMain.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/I2CMain.d \
./Src/IntroGPIOxMain.d \
./Src/PWMMain.d \
./Src/RTCMain.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -c -I../Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

