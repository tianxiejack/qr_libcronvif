################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../onvif/dom.c \
../onvif/duration.c \
../onvif/mecevp.c \
../onvif/onvif_comm.c \
../onvif/onvif_dump.c \
../onvif/smdevp.c \
../onvif/soapC.c \
../onvif/soapClient.c \
../onvif/stdsoap2.c \
../onvif/threads.c \
../onvif/wsaapi.c \
../onvif/wsseapi.c 

OBJS += \
./onvif/dom.o \
./onvif/duration.o \
./onvif/mecevp.o \
./onvif/onvif_comm.o \
./onvif/onvif_dump.o \
./onvif/smdevp.o \
./onvif/soapC.o \
./onvif/soapClient.o \
./onvif/stdsoap2.o \
./onvif/threads.o \
./onvif/wsaapi.o \
./onvif/wsseapi.o 

C_DEPS += \
./onvif/dom.d \
./onvif/duration.d \
./onvif/mecevp.d \
./onvif/onvif_comm.d \
./onvif/onvif_dump.d \
./onvif/smdevp.d \
./onvif/soapC.d \
./onvif/soapClient.d \
./onvif/stdsoap2.d \
./onvif/threads.d \
./onvif/wsaapi.d \
./onvif/wsseapi.d 


# Each subdirectory must supply rules for building sources it contributes
onvif/%.o: ../onvif/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -DWITH_OPENSSL -I../onvif -I../src -O3 -Xcompiler -fPIC -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "onvif" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -DWITH_OPENSSL -I../onvif -I../src -O3 -Xcompiler -fPIC --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


